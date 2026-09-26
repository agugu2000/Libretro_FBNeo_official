#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <stdint.h>
#include <file/file_path.h>
#include <streams/file_stream.h>

#include "decode_command.h"
#include "utf8_util.h"

namespace CommandDat {

static Result g_result;
static bool   g_loaded = false;

// ---------- 符号替换表 ----------

struct SymbolEntry {
    const char* key;
    unsigned int glyphid;
};

// @xxx 用，按 key 长度降序（长的先匹配）
static const SymbolEntry kConvertText[] = {
    {"L-punch", 57}, {"M-punch", 58}, {"S-punch", 59},
    {"L-kick", 60}, {"M-kick", 61}, {"S-kick", 62},
    {"3-kick", 63}, {"3-punch", 64}, {"2-kick", 65}, {"2-punch", 66},
    {"custom1", 67}, {"custom2", 68}, {"custom3", 69}, {"custom4", 70},
    {"custom5", 71}, {"custom6", 72}, {"custom7", 73}, {"custom8", 74},
    {"nplayer", 80}, {"1player", 81}, {"2player", 82}, {"3player", 83},
    {"4player", 84}, {"5player", 85}, {"6player", 86}, {"7player", 87},
    {"8player", 88},
    {"A-button", 1}, {"B-button", 2}, {"C-button", 3}, {"D-button", 4},
    {"E-button", 5}, {"F-button", 6}, {"G-button", 7}, {"H-button", 8},
    {"I-button", 9}, {"J-button", 10}, {"K-button", 11}, {"L-button", 12},
    {"M-button", 13}, {"N-button", 14}, {"O-button", 15}, {"P-button", 16},
    {"Q-button", 17}, {"R-button", 18}, {"S-button", 19}, {"T-button", 20},
    {"U-button", 21}, {"V-button", 22}, {"W-button", 23}, {"X-button", 24},
    {"Y-button", 25}, {"Z-button", 26},
    {"decrease", 37}, {"increase", 38}, {"BALL", 45},
    {"start", 51}, {"select", 52}, {"punch", 53}, {"kick", 54}, {"guard", 55},
    {"up", 75}, {"down", 76}, {"left", 77}, {"right", 78}, {"lever", 79},
    {"-->", 90}, {"==>", 91},
    {"hcb", 100}, {"huf", 101}, {"hcf", 102}, {"hub", 103},
    {"qfd", 104}, {"qdb", 105}, {"qbu", 106}, {"quf", 107},
    {"qbd", 108}, {"qdf", 109}, {"qfu", 110}, {"qub", 111},
    {"fdf", 112}, {"fub", 113}, {"fuf", 114}, {"fdb", 115},
    {"xff", 116}, {"xbb", 117}, {"dsf", 118}, {"dsb", 119},
    {"AIR", 121}, {"DIR", 122}, {"MAX", 123}, {"TAP", 124},
    {"jump", 125}, {"hold", 126}, {"air", 127}, {"sit", 128},
    {"close", 129}, {"away", 130}, {"charge", 131}, {"tap", 132},
    {"button", 133},
    {NULL, 0}
};

// _x 用，单字符 key
static const SymbolEntry kDefaultText[] = {
    {"A", 1}, {"B", 2}, {"C", 3}, {"D", 4}, {"H", 8}, {"Z", 26},
    {"a", 27}, {"b", 28}, {"c", 29}, {"d", 30}, {"e", 31},
    {"f", 32}, {"g", 33}, {"h", 34}, {"i", 35}, {"j", 36},
    {"+", 39}, {".", 40},
    {"1", 41}, {"2", 42}, {"3", 43}, {"4", 44}, {"5", 45},
    {"6", 46}, {"7", 47}, {"8", 48}, {"9", 49}, {"N", 50},
    {"S", 51}, {"P", 53}, {"K", 54}, {"G", 55},
    {"!", 90},
    {"k", 100}, {"l", 101}, {"m", 102}, {"n", 103},
    {"o", 104}, {"p", 105}, {"q", 106}, {"r", 107},
    {"s", 108}, {"t", 109}, {"u", 110}, {"v", 111},
    {"w", 112}, {"x", 113}, {"y", 114}, {"z", 115},
    {"L", 116}, {"M", 117}, {"Q", 118}, {"R", 119},
    {"^", 121}, {"?", 122}, {"X", 124},
    {"|", 125}, {"O", 126}, {"-", 127}, {"=", 128}, {"~", 131},
    {"`", 135}, {"@", 136}, {")", 137}, {"(", 138},
    {"*", 139}, {"&", 140}, {"%", 141}, {"$", 142},
    {"#", 143}, {"]", 144}, {"[", 145},
    {"{", 146}, {"}", 147}, {"<", 148}, {">", 149},
    {NULL, 0}
};

// ^x 用，单字符 key
static const SymbolEntry kExpandText[] = {
    {"s", 19}, {"S", 52},
    {"E", 57}, {"F", 58}, {"G", 59}, {"H", 60},
    {"I", 61}, {"J", 62}, {"T", 63}, {"U", 64},
    {"V", 65}, {"W", 66},
    {"!", 91},
    {"1", 92}, {"2", 93}, {"3", 94}, {"4", 95},
    {"6", 96}, {"7", 97}, {"8", 98}, {"9", 99},
    {"M", 123},
    {"-", 129}, {"=", 130}, {"*", 132}, {"?", 133},
    {NULL, 0}
};

// ---------- 文件读取 ----------
static bool ReadFile(const char* path, std::vector<char>& out) {
    out.clear();

    if (!path_is_valid(path))
        return false;

    RFILE* rf = filestream_open(path,
                                RETRO_VFS_FILE_ACCESS_READ,
                                RETRO_VFS_FILE_ACCESS_HINT_NONE);
    if (!rf)
        return false;

    int64_t sz = filestream_get_size(rf);
    if (sz <= 0 || sz > (int64_t)(256 * 1024 * 1024)) {
        filestream_close(rf);
        return false;
    }

    out.resize((size_t)sz);
    int64_t got = filestream_read(rf, &out[0], sz);
    filestream_close(rf);

    if (got != sz) {
        out.clear();
        return false;
    }

    return true;
}

// ---------- 行类型 ----------
enum LineKind { LINE_OTHER = 0, LINE_TAG = 1, LINE_TYPE = 2 };

static LineKind ClassifyLine(const char* p, const char* e) {
    if (p >= e || *p != '$') return LINE_OTHER;
    const char* eq = (const char*)memchr(p, '=', (size_t)(e - p));
    if (eq) return LINE_TAG;
    const char* q = p + 1;
    while (q < e && (isalnum((unsigned char)*q) || *q == '_')) q++;
    if (q == p + 1) return LINE_OTHER;
    for (const char* r = q; r < e; r++) {
        if (*r != ' ' && *r != '\t' && *r != '\r') return LINE_OTHER;
    }
    return LINE_TYPE;
}

// ---------- sets 解析 ----------
static void ParseTagSets(const char* p, const char* e,
                         std::vector<std::string>& sets) {
    const char* eq = (const char*)memchr(p, '=', (size_t)(e - p));
    if (!eq) return;
    const char* s = eq + 1;
    while (s <= e) {
        const char* comma = (const char*)memchr(s, ',', (size_t)(e - s));
        const char* end   = comma ? comma : e;
        if (end > s) sets.push_back(std::string(s, (size_t)(end - s)));
        if (!comma) break;
        s = comma + 1;
    }
}

// ---------- $end 三条 gsub 的等价实现（一遍扫描）----------
static std::string CleanupData(const char* p, const char* e) {
    std::string out;
    out.reserve((size_t)(e - p));

    const char* cur = p;
    while (cur < e) {
        const char* end_tag = NULL;
        {
            const char* scan = cur;
            while (scan < e) {
                const char* nl = (const char*)memchr(scan, '\n', (size_t)(e - scan));
                const char* line_end = nl ? nl : e;
                if ((size_t)(line_end - scan) >= 4 && memcmp(scan, "$end", 4) == 0) {
                    end_tag = scan;
                    break;
                }
                if (!nl) break;
                scan = nl + 1;
            }
        }
        if (!end_tag) {
            out.append(cur, (size_t)(e - cur));
            break;
        }

        out.append(cur, (size_t)(end_tag - cur));

        const char* nl = (const char*)memchr(end_tag, '\n', (size_t)(e - end_tag));
        if (!nl) break;
        const char* after_end = nl + 1;

        const char* next_type = NULL;
        {
            const char* scan = after_end;
            while (scan < e) {
                const char* l_nl = (const char*)memchr(scan, '\n', (size_t)(e - scan));
                const char* line_end = l_nl ? l_nl : e;
                if (line_end > scan && *scan == '$') {
                    const char* q = scan + 1;
                    while (q < line_end && (isalnum((unsigned char)*q) || *q == '_')) q++;
                    bool only_ws = true;
                    for (const char* r = q; r < line_end; r++) {
                        if (*r != ' ' && *r != '\t' && *r != '\r') { only_ws = false; break; }
                    }
                    if (q > scan + 1 && only_ws) { next_type = scan; break; }
                }
                if (!l_nl) break;
                scan = l_nl + 1;
            }
        }
        if (!next_type) break;

        out.push_back('\n');

        const char* nl2 = (const char*)memchr(next_type, '\n', (size_t)(e - next_type));
        cur = nl2 ? nl2 + 1 : e;
    }

    // 去 \r，同时去首尾 \n，一遍扫描
    size_t start = 0;
    while (start < out.size() && (out[start] == '\n' || out[start] == '\r')) start++;

    size_t end = out.size();
    while (end > start && (out[end - 1] == '\n' || out[end - 1] == '\r')) end--;

    size_t w = 0;
    for (size_t i = start; i < end; i++) {
        if (out[i] != '\r') out[w++] = out[i];
    }
    out.resize(w);

    return out;
}

// ---------- 符号替换 ----------
struct SingleMap {
    bool         has[256];
    unsigned int glyph[256];
    SingleMap() { memset(has, 0, sizeof(has)); memset(glyph, 0, sizeof(glyph)); }
    void build(const SymbolEntry* table) {
        for (int i = 0; table[i].key; i++) {
            if (table[i].key[1] == '\0') {
                unsigned char c = (unsigned char)table[i].key[0];
                has[c] = true;
                glyph[c] = table[i].glyphid;
            }
        }
    }
};
static SingleMap g_defaultMap;
static SingleMap g_expandMap;
static bool      g_maps_built = false;

static void BuildMaps() {
    if (g_maps_built) return;
    g_defaultMap.build(kDefaultText);
    g_expandMap.build(kExpandText);
    g_maps_built = true;
}

static void EncodePrivateUse(std::string& out, unsigned int glyphid) {
    unsigned int cp = 0xE000 + glyphid;
    out.push_back((char)(0xE0 | (cp >> 12)));
    out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
    out.push_back((char)(0x80 | (cp & 0x3F)));
}

static bool IsLuaG(unsigned char c) { return c > 32 && c < 127; }

static std::string ConvertSymbols(const std::string& in) {
    BuildMaps();
    std::string out;
    out.reserve(in.size());

    const char* p = in.c_str();
    const char* e = p + in.size();
    const char* cur = p;

    while (cur < e) {
        if (*cur == '@') {
            const char* start = cur + 1;
            const char* end = start;
            while (end < e && IsLuaG((unsigned char)*end)) end++;
            if (end > start) {
                size_t len = (size_t)(end - start);
                bool found = false;
                for (int i = 0; kConvertText[i].key; i++) {
                    size_t klen = strlen(kConvertText[i].key);
                    if (klen == len && memcmp(start, kConvertText[i].key, klen) == 0) {
                        EncodePrivateUse(out, kConvertText[i].glyphid);
                        found = true;
                        cur = end;
                        break;
                    }
                }
                if (found) continue;
                out.append(start, len);
                cur = end;
                continue;
            }
            out.push_back(*cur);
            cur++;
            continue;
        }

        if (*cur == '_' && cur + 1 < e && IsLuaG((unsigned char)cur[1])) {
            unsigned char c = (unsigned char)cur[1];
            if (g_defaultMap.has[c]) {
                EncodePrivateUse(out, g_defaultMap.glyph[c]);
            } else {
                out.push_back(cur[1]);
            }
            cur += 2;
            continue;
        }

        if (*cur == '^' && cur + 1 < e && IsLuaG((unsigned char)cur[1])) {
            unsigned char c = (unsigned char)cur[1];
            if (g_expandMap.has[c]) {
                EncodePrivateUse(out, g_expandMap.glyph[c]);
            } else {
                out.push_back(cur[1]);
            }
            cur += 2;
            continue;
        }

        out.push_back(*cur);
        cur++;
    }
    return out;
}

// ---------- 主解析 ----------
// 为 C++98 起见，把原本的 lambda 拆成静态函数。
// 状态用一个 struct 传递上下文。
struct ParseCtx {
    const char* b;
    size_t      n;
    const char* rom_name;
    std::vector<std::string> sets;
    const char* data_begin;

    ParseCtx() : b(NULL), n(0), rom_name(NULL), data_begin(NULL) {}
};

static void CtxReset(ParseCtx& ctx) {
    ctx.sets.clear();
    ctx.data_begin = NULL;
}

static void CtxEmit(ParseCtx& ctx, const char* data_end) {
    if (ctx.data_begin == NULL) return;
    bool match = false;
    for (size_t i = 0; i < ctx.sets.size(); i++) {
        if (ctx.sets[i] == ctx.rom_name) { match = true; break; }
    }
    if (!match) return;

    Record rec;
    rec.sets = ctx.sets;
    std::string cleaned = CleanupData(ctx.data_begin, data_end);
    rec.data = ConvertSymbols(cleaned);
    g_result.records.push_back(rec);
}

bool Load(const char* command_dat_path, const char* rom_name) {
    Unload();
    if (!command_dat_path || !rom_name) return false;

    std::vector<char> file;
    if (!ReadFile(command_dat_path, file)) return false;

    // 开头补一个 \n
    std::vector<char> work;
    work.reserve(file.size() + 2);
    work.push_back('\n');
    work.insert(work.end(), file.begin(), file.end());
    work.push_back('\0');
    std::vector<char>().swap(file);   // 释放

    const char* b = &work[0];
    const size_t n = work.size() - 1;

    size_t scan_start = 1;
    if (n >= scan_start + 3 &&
        (unsigned char)b[scan_start]     == 0xEF &&
        (unsigned char)b[scan_start + 1] == 0xBB &&
        (unsigned char)b[scan_start + 2] == 0xBF) {
        scan_start += 3;
    }

    // 不是合法 UTF-8 就拒绝，records 保持空
    if (!is_valid_utf8(b + scan_start, n - scan_start)) {
        std::vector<char>().swap(work);
        return false;
    }

    enum State { STATE_WAIT_TAG = 0, STATE_WAIT_TYPE = 1, STATE_COLLECT = 2 };
    State state = STATE_WAIT_TAG;

    ParseCtx ctx;
    ctx.b = b;
    ctx.n = n;
    ctx.rom_name = rom_name;

    size_t pos = scan_start;

    while (pos < n) {
        const char* line_start = b + pos;
        const char* nl = (const char*)memchr(line_start, '\n', (size_t)(b + n - line_start));
        const char* line_end = nl ? nl : b + n;
        const char* next = nl ? nl + 1 : b + n;

        const char* e_strip = line_end;
        if (e_strip > line_start && e_strip[-1] == '\r') e_strip--;

        LineKind kind = ClassifyLine(line_start, e_strip);

        switch (state) {
        case STATE_WAIT_TAG:
            if (kind == LINE_TAG) {
                CtxReset(ctx);
                ParseTagSets(line_start, e_strip, ctx.sets);
                state = STATE_WAIT_TYPE;
            }
            break;

        case STATE_WAIT_TYPE:
            if (kind == LINE_TYPE) {
                ctx.data_begin = next;
                state = STATE_COLLECT;
            } else if (kind == LINE_TAG) {
                pos = n;
                goto done;
            }
            break;

        case STATE_COLLECT:
            if (kind == LINE_TAG) {
                CtxEmit(ctx, line_start);
                CtxReset(ctx);
                ParseTagSets(line_start, e_strip, ctx.sets);
                state = STATE_WAIT_TYPE;
            }
            break;
        }

        pos = (size_t)(next - b);
    }

    if (state == STATE_COLLECT) {
        CtxEmit(ctx, b + n);
    }

done:
    g_loaded = true;
    std::vector<char>().swap(work);
    return true;
}

void Unload(void) {
    g_result.records.clear();
    std::vector<Record>().swap(g_result.records);
    g_loaded = false;
}

const Result& Get(void) { return g_result; }
bool IsLoaded(void) { return g_loaded; }

} // namespace CommandDat