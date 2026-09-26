#ifndef UGUI_TOOLS_H_
#define UGUI_TOOLS_H_

#include <ugui.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* bpp = bytes per pixel */
void gui_init(int width, int height, int bpp);

void gui_draw(void);

void gui_set_window_title(const char *title);

void gui_set_window_title_with_page(const char *title, int page, int total);

void gui_set_message(const char *message);

void gui_window_resize(int x, int y, int width, int height);

unsigned* gui_get_framebuffer(void);

/* [NON-OFFICIAL HACK] command.dat overlay */
void gui_show_overlay(void);
void gui_hide_overlay(void);
int  gui_is_overlay_visible(void);
int  gui_get_mode(void);
void gui_show_error_mode(void);
void gui_blend_onto(void* dst, int dst_bpp, int dst_w, int dst_h, int dst_pitch);
int gui_get_line_height(void);
int gui_get_inner_height(void);
int gui_get_inner_width(void);
int gui_get_content_height(void);
int gui_get_content_width(void);
int gui_get_glyph_advance(unsigned int codepoint);

#ifdef __cplusplus
}
#endif

#endif