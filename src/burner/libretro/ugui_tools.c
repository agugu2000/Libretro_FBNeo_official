#include <stdlib.h>
#include <string/stdstring.h>
#include <ugui.h>
#include <stdio.h>
#include <string.h>
#include "utf8_util.h"

#define UGUI_MAX_OBJECTS 2
#define GUI_CONTENT_TOP_OFFSET 5
#define GUI_CONTENT_LEFT_OFFSET 10
static UG_GUI gui;
static UG_WINDOW gui_window;
static UG_TEXTBOX gui_textbox;
static UG_OBJECT gui_objbuf_wnd[UGUI_MAX_OBJECTS];
static unsigned *frame_buf = NULL;
static int width  = 0;
static int height = 0;
static char gui_message[8192] = {0};
static UG_DEVICE Fbneodevice;
void Fbneoflush(void);
int gui_get_inner_width(void);
int gui_get_glyph_advance(unsigned int codepoint);

void Fbneoflush(void)
{
	// nop
}

static void gui_window_callback(UG_MESSAGE *msg)
{
}

unsigned* gui_get_framebuffer(void)
{
   return frame_buf;
}

static void UserPixelSetFunction(UG_S16 x, UG_S16 y, UG_COLOR c)
{
   frame_buf[width * y + x] = c;
}

void gui_init(int w, int h, int bpp)
{
   if (width == w && height == h && frame_buf != NULL) {
      return;
   }

   width     = w;
   height    = h;
   if (frame_buf) {
      free(frame_buf);
   }
   frame_buf = (unsigned*)calloc(width * height, bpp);

   Fbneodevice.x_dim = width;
   Fbneodevice.y_dim = height;
   Fbneodevice.pset  = &UserPixelSetFunction;
   Fbneodevice.flush = &Fbneoflush;

   /* init uGUI */
   UG_Init(&gui, &Fbneodevice);
   UG_FontSelect(FONT_SIMSUN2_13X13);
   UG_FontSetShadow(1);

   /* create a single window with no buttons */
   UG_WindowCreate(&gui_window, gui_objbuf_wnd, UGUI_MAX_OBJECTS, gui_window_callback);
   UG_WindowSetStyle(&gui_window, WND_STYLE_SHOW_TITLE);
   UG_WindowSetForeColor(&gui_window, C_BLACK);
   UG_WindowSetTitleTextFont(&gui_window, FONT_8X8);

   UG_WindowSetXStart(&gui_window, 0);
   UG_WindowSetYStart(&gui_window, 0);
   UG_WindowSetXEnd(&gui_window, width - 1);
   UG_WindowSetYEnd(&gui_window, height - 1);

   UG_TextboxCreate(&gui_window, &gui_textbox, TXB_ID_0, GUI_CONTENT_LEFT_OFFSET, GUI_CONTENT_TOP_OFFSET, UG_WindowGetInnerWidth(&gui_window) - 1, UG_WindowGetInnerHeight(&gui_window) - 1);
   UG_TextboxSetAlignment(&gui_window, TXB_ID_0, ALIGN_TOP_LEFT);
   UG_WindowShow(&gui_window);

   /* 触发 _UG_FontSelect，填充 currentFont，供 gui_get_glyph_advance / gui_get_line_height 使用 */
   UG_PutString(0, 0, "");
}

void gui_set_message(const char *message)
{
   memset(gui_message, 0, sizeof(gui_message));
   snprintf(gui_message, sizeof(gui_message), "%s", message);
   gui_message[sizeof(gui_message) - 1] = '\0';
   UG_TextboxSetText(&gui_window, TXB_ID_0, gui_message);
}

void gui_window_resize(int x, int y, int width, int height)
{
   UG_WindowResize(&gui_window, x, y, width, height);
}

void gui_set_window_title(const char *title)
{
   static char s_title[128];

   if (!title) {
      s_title[0] = '\0';
      UG_WindowSetTitleText(&gui_window, s_title);
      return;
   }

   int max_w = gui_get_inner_width();
   if (max_w < (int)gui.currentFont.max_ink_w * 2) {
      max_w = (int)gui.currentFont.max_ink_w * 2;
   }

   char *p = (char *)title;
   size_t out = 0;
   int    w   = 0;

   while (*p && out + 1 < sizeof(s_title)) {
      int cp = 0;
      size_t len = utf8_val(&cp, p);
      if (cp == -1 || len == 0) {
         len = 1;
         cp = (unsigned char)*p;
      }
      p += len;

      if (cp == '\n' || cp == '\r') continue;

      int adv = gui_get_glyph_advance((unsigned int)cp);
      if (w + adv > max_w) break;

      if (out + len >= sizeof(s_title)) break;
      memcpy(&s_title[out], p - len, len);
      out += len;
      w += adv;
   }
   s_title[out] = '\0';

   UG_WindowSetTitleText(&gui_window, s_title);
}

static int TitleAdvance(unsigned int cp)
{
   /* 标题字体是 FONT_8X8，旧格式等宽 */
   UG_FONT *title_font = gui_window.title.font;
   if (title_font) {
      const UG_U8 *p = (const UG_U8 *)title_font;
      if (p[0] & 0x80) {
         /* 旧格式，char_width 在 p[1] */
         return (int)p[1] + (int)gui_window.title.h_space;
      }
      /* 新格式，max_ink_w 在 p[2..3] */
      return (int)((p[2] << 8) | p[3]) + (int)gui_window.title.h_space;
   }
   return 8 + (int)gui_window.title.h_space;
}

void gui_set_window_title_with_page(const char *title, int page, int total)
{
   static char s_title[256];

   if (!title) {
      s_title[0] = '\0';
      UG_WindowSetTitleText(&gui_window, s_title);
      return;
   }

   int max_w = gui_get_inner_width() - 4;
   if (max_w < (int)gui.currentFont.max_ink_w * 2) {
      max_w = (int)gui.currentFont.max_ink_w * 2;
   }

   /* 1. 拼页码部分 "n/total" */
   char page_str[32];
   if (page > 0 && total > 0) {
      snprintf(page_str, sizeof(page_str), "%d/%d", page, total);
   } else {
      page_str[0] = '\0';
   }

   /* 2. 算页码宽度 */
   int page_w = 0;
   {
      char *p = (char *)page_str;
      while (*p) {
         int cp = 0;
         size_t len = utf8_val(&cp, p);
         if (cp == -1 || len == 0) {
            len = 1;
            cp = (unsigned char)*p;
         }
         p += len;
         page_w += TitleAdvance((unsigned int)cp);
      }
   }

   /* 3. 标题可用宽度 = 总宽 - 页码宽 - 最小空隙 */
   int min_gap = (int)gui.currentFont.max_ink_w;
   if (min_gap < 1) min_gap = 1;

   int title_max_w = max_w - page_w - min_gap;
   if (title_max_w < 0) title_max_w = 0;

   /* 4. 截断标题 */
   char truncated[192];
   {
      char *p = (char *)title;
      size_t out = 0;
      int    w   = 0;
      while (*p && out + 1 < sizeof(truncated)) {
         int cp = 0;
         size_t len = utf8_val(&cp, p);
         if (cp == -1 || len == 0) {
            len = 1;
            cp = (unsigned char)*p;
         }
         p += len;

         if (cp == '\n' || cp == '\r') continue;

         int adv = TitleAdvance((unsigned int)cp);
         if (w + adv > title_max_w) break;

         if (out + len >= sizeof(truncated)) break;
         memcpy(&truncated[out], p - len, len);
         out += len;
         w += adv;
      }
      truncated[out] = '\0';
   }

   /* 5. 拼最终标题 */
   size_t pos = 0;
   size_t tlen = strlen(truncated);
   if (tlen < sizeof(s_title) - 1) {
      memcpy(s_title, truncated, tlen);
      pos = tlen;
   }

   if (page_str[0]) {
      int cur_w = 0;
      {
         char *p = (char *)truncated;
         while (*p) {
            int cp = 0;
            size_t len = utf8_val(&cp, p);
            if (cp == -1 || len == 0) {
               len = 1;
               cp = (unsigned char)*p;
            }
            p += len;
            cur_w += TitleAdvance((unsigned int)cp);
         }
      }

      int space_w = TitleAdvance(' ');
      if (space_w <= 0) space_w = 1;

      int need = max_w - cur_w - page_w;
      int spaces = need / space_w;
      if (spaces < 1) spaces = 1;

      size_t plen = strlen(page_str);

      int max_spaces_by_buf = (int)(sizeof(s_title) - 1 - pos - plen);
      if (max_spaces_by_buf < 0) max_spaces_by_buf = 0;
      if (spaces > max_spaces_by_buf) spaces = max_spaces_by_buf;

      for (int i = 0; i < spaces; i++) {
         s_title[pos++] = ' ';
      }

      if (pos + plen < sizeof(s_title)) {
         memcpy(&s_title[pos], page_str, plen);
         pos += plen;
      }
   }
   s_title[pos] = '\0';

   UG_WindowSetTitleText(&gui_window, s_title);
}

void gui_draw(void)
{
   if (!string_is_empty(gui_message))
      UG_TextboxSetText(&gui_window, TXB_ID_0, gui_message);
   UG_Update();
}

/* [NON-OFFICIAL HACK] command.dat overlay */

enum {
   GUI_MODE_NONE = 0,
   GUI_MODE_ERROR,
   GUI_MODE_OVERLAY
};

static int gui_mode = GUI_MODE_NONE;

void gui_show_overlay(void)
{
   gui_mode = GUI_MODE_OVERLAY;
   if (frame_buf) {
      memset(frame_buf, 0, (size_t)width * height * sizeof(unsigned));
   }
   UG_TextboxSetAlignment(&gui_window, TXB_ID_0, ALIGN_TOP_LEFT);
   UG_WindowShow(&gui_window);
}

void gui_hide_overlay(void)
{
   if (gui_mode == GUI_MODE_OVERLAY) {
      gui_mode = GUI_MODE_NONE;
      UG_WindowHide(&gui_window);
   }
}

int gui_is_overlay_visible(void)
{
   return gui_mode == GUI_MODE_OVERLAY;
}

int gui_get_mode(void)
{
   return gui_mode;
}

void gui_show_error_mode(void)
{
   gui_mode = GUI_MODE_ERROR;
   if (frame_buf) {
      memset(frame_buf, 0, (size_t)width * height * sizeof(unsigned));
   }
   UG_TextboxSetAlignment(&gui_window, TXB_ID_0, ALIGN_CENTER);
   UG_WindowShow(&gui_window);
}

void gui_blend_onto(void* dst, int dst_bpp, int dst_w, int dst_h, int dst_pitch)
{
   if (!frame_buf || !dst || gui_mode != GUI_MODE_OVERLAY) {
      return;
   }

   int src_w = width;
   int src_h = height;

   if (src_w != dst_w || src_h != dst_h) {
      return;
   }

   unsigned char* base = (unsigned char*)dst;

   for (int y = 0; y < dst_h; y++) {
      unsigned char* row = base + (size_t)y * dst_pitch;

      if (dst_bpp == 4) {
         unsigned* d = (unsigned*)row;
         const unsigned* s = frame_buf + (size_t)y * src_w;
         memcpy(d, s, (size_t)src_w * sizeof(unsigned));
      } else if (dst_bpp == 2) {
         unsigned short* d = (unsigned short*)row;
         const unsigned* s = frame_buf + (size_t)y * src_w;
         for (int x = 0; x < dst_w; x++) {
            unsigned c = s[x];
            unsigned short r = (c >> 16) & 0xFF;
            unsigned short g = (c >> 8)  & 0xFF;
            unsigned short b =  c        & 0xFF;
            d[x] = (unsigned short)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
         }
      }
   }
}

int gui_get_inner_width(void)
{
   return UG_WindowGetInnerWidth(&gui_window);
}

int gui_get_inner_height(void)
{
   return UG_WindowGetInnerHeight(&gui_window) - 1;
}

int gui_get_content_width(void)
{
   int w = gui_get_inner_width() - GUI_CONTENT_LEFT_OFFSET;
   if (w < 0) w = 0;
   return w;
}

int gui_get_content_height(void)
{
   int h = gui_get_inner_height() - GUI_CONTENT_TOP_OFFSET;
   if (h < 0) h = 0;
   return h;
}

int gui_get_line_height(void)
{
   int lh = (int)gui.currentFont.ascender - (int)gui.currentFont.descender + (int)gui.char_v_space;
   if (lh < 1) lh = (int)gui.currentFont.max_ink_h + gui.char_v_space;
   if (lh < 1) lh = 1;
   return lh;
}

int gui_get_glyph_advance(unsigned int codepoint)
{
   int adv = 0;
   if (gui.currentFont.format != UG_FONT_FMT_NEW) {
      adv = gui.currentFont.old_char_width;
   } else {
      UG_U32 n = gui.currentFont.number_of_chars;
      const UG_U8 *cps = gui.currentFont.codepoints;
      const UG_U8 *m = gui.currentFont.metrics;

      if (n == 0 || !cps || !m) {
         adv = gui.currentFont.notdef_adv;
         if (adv <= 0) adv = gui.currentFont.max_ink_w;
      } else {
         UG_U32 lo = 0, hi = n;
         int found = 0;
         while (lo < hi) {
            UG_U32 mid = lo + (hi - lo) / 2;
            int c = (cps[mid * UG_FONT_CODEPOINT_SIZE] << 8) |
                     cps[mid * UG_FONT_CODEPOINT_SIZE + 1];
            if (c == (int)codepoint) {
               adv = (int)((m[mid * UG_FONT_METRICS_SIZE + 8] << 8) |
                            m[mid * UG_FONT_METRICS_SIZE + 9]);
               found = 1;
               break;
            }
            if (c < (int)codepoint) lo = mid + 1;
            else hi = mid;
         }
         if (!found) {
            adv = gui.currentFont.notdef_adv;
            if (adv <= 0) adv = gui.currentFont.max_ink_w;
         }
      }
   }

   if (adv <= 0) adv = gui.currentFont.max_ink_w;
   if (adv <= 0) adv = 1;
   adv += gui_textbox.h_space;
   return adv;
}