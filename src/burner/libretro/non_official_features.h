#ifndef NON_OFFICIAL_FEATURES_H
#define NON_OFFICIAL_FEATURES_H

#ifdef __LIBRETRO__
#define SKIP_STDIO_REDEFINES
#endif

// Non-official features for FBNeo libretro core
//
// This module adds the following features on top of the official core:
//   1. Per-game cheat loading from zip/7z containers (ini / vct / nebula dat)
//   2. Custom combination-key macros (NeoGeo / PGM / CPS1 / Street Fighter 6-key)
//   3. "Allow ignore CRC" option for patched romsets
//   4. DIP region default preference (China > Taiwan > Hong Kong)
//   5. command.dat move-list display
//   6. Extended localized strings (18 entries, independent of official)

#include "retro_common.h"
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// Localized strings (18 entries, independent of official NUM_STRING)
// ---------------------------------------------------------------------------

#define NUM_STRING_NONOFFICIAL	18

extern const char* pSelLangStrNonOfficial[NUM_STRING_NONOFFICIAL];
extern const char* multi_language_strings_nonofficial[MAX_LANGUAGES][NUM_STRING_NONOFFICIAL];

#define macro_desc_button_l					pSelLangStrNonOfficial[ 0]
#define macro_desc_button_r					pSelLangStrNonOfficial[ 1]
#define macro_desc_button_l2				pSelLangStrNonOfficial[ 2]
#define macro_desc_button_r2				pSelLangStrNonOfficial[ 3]
#define macro_desc_button_l3				pSelLangStrNonOfficial[ 4]
#define macro_desc_button_r3				pSelLangStrNonOfficial[ 5]
#define macro_info_button					pSelLangStrNonOfficial[ 6]
#define macro_info_streetfighter_button		pSelLangStrNonOfficial[ 7]
#define macro_disabled						pSelLangStrNonOfficial[ 8]
#define neogeo_macro_desc					pSelLangStrNonOfficial[ 9]
#define pgm_macro_desc						pSelLangStrNonOfficial[10]
#define cps1_macro_desc						pSelLangStrNonOfficial[11]
#define streetfighter_macro_desc			pSelLangStrNonOfficial[12]
#define macro_info_general					pSelLangStrNonOfficial[13]
#define RETRO_IGNORE_CRC_DESC				pSelLangStrNonOfficial[14]
#define RETRO_IGNORE_CRC_INFO				pSelLangStrNonOfficial[15]
#define RETRO_COMMAND_DAT_CAT_DESC			pSelLangStrNonOfficial[16]
#define RETRO_COMMAND_DAT_CAT_INFO			pSelLangStrNonOfficial[17]

// ---------------------------------------------------------------------------
// Custom combination-key macros
// ---------------------------------------------------------------------------

struct macro_option_value {
	const char* value;
	const char* label;
};

struct macro_option {
	const char* key;
	const char* button;			// "L" / "R" / "L2" / "R2" / "L3" / "R3"
	const char* option_name;
	const char* info;
	const char* default_value;
	macro_option_value values[20];		// up to 15 combinations + "disabled" + NULL
};

struct macro_category {
	const char* system;			// "pgm" / "neogeo" / "cps1" / "streetfighter"
	const char* category_name;
	macro_option options[4];		// one entry per bindable shoulder button
};

struct CustomMacroKey {
	std::string button;			// "L" / "R" / "L2" / "R2" / "L3" / "R3"
	std::string macroKey;			// user-selected combination, e.g. "Buttons AB"
};

struct CustomMacroKeys {
	std::vector<CustomMacroKey> macrocontent;
};

// Return the number of bindable shoulder-button slots for the current macro category
int get_macro_count();

// Load the user's macro selection for the given system from core options
CustomMacroKeys LoadCustomMacroKeys(const char* system);

// Register the macro-related core options for the given system
int AddMacroOptions(const char* system, int nbr_macros, int idx_var);

// Bind the user-selected macros to the corresponding GameInp entries
void BindCustomMacroKeys(const CustomMacroKeys& macrosdata, char* description,
                         int nPlayer, unsigned int* nDeviceType, struct GameInp* pgi);

// Bulk-create macro nodes during layout analysis
struct GameInp* AddMacroKeys(struct GameInp* pgi, int nPlayer,
                             int nButtonsTwo[][2], int nButtonsFour[][4],
                             int nPunchInputs[][3], int nKickInputs[][3],
                             const char* system, UINT32& numMacroCount);

// Button assignment helpers
void AssignButtons(const char* system, const char* szName, const char* szInfo,
                   int nPlayer, int i, int nButtons[][4]);
void AssignButtons(const char* system, const char* szName, const char* szInfo,
                   int nPlayer, int i, int nButtons[][2], INT32 nCps1Tradition[]);

// When true, the official hardcoded Neo-Geo macros (AB / CD / ABC / BCD) are
// hidden and only the custom macro system is available. Default: true.
extern bool bUseCustomMacroOnly;

// ---------------------------------------------------------------------------
// Zip/7z container support for per-game cheat files
//
// Uses the official Zip* API (zipfn.cpp). The official ZipOpen() expects a
// path without extension and internally tries ".zip" then ".7z", so the
// caller just passes "cheat" and the correct container is opened
// automatically.
// ---------------------------------------------------------------------------

// Extract "<drvName><ext>" from "<szAppCheatsPath><zipFileName>" into outContent.
// drvName must be the same name the official core uses when looking for that
// file on disk (i.e. already normalized by IsCDGame / CDInfo_GamePrefix).
// zipFileName must be without extension (e.g. "cheat").
// Returns 0 on success, non-zero otherwise.
INT32 LoadCheatContentFromZip(const char* drvName,
                              const char* ext,
                              const char* zipFileName,
                              std::vector<char>& outContent);

// [NON-OFFICIAL HACK] cheat content caches
extern std::vector<char> g_CurrentMameCheatContent;
extern std::vector<char> g_CurrentWayderCheatContent;
extern std::vector<char> g_CurrentIniCheatContent;
extern std::vector<char> g_CurrentVctContent;

// [NON-OFFICIAL HACK] parent caches (for clone fallback, "child first, parent fallback")
extern std::vector<char> g_CurrentParentCheatContent;
extern std::vector<char> g_CurrentWayderParentCheatContent;

// extracted flags
extern bool g_bMameCheatExtracted;
extern bool g_bWayderCheatExtracted;

// source flags
extern bool g_bVctFromZip;
extern bool g_bIniFromZip;

// Extract the first contiguous block matching matchDrvName from a MAME cheat.dat
// into cache. Stops at "----:REASON" or at the first non-matching line after a match.
INT32 ExtractMameCheatFromDat(FILE* MameDatCheat, const TCHAR* matchDrvName, std::vector<char>& cache);

// extract <drvname>.vct / <drvname>.ini from zip/7z
INT32 ExtractVctFromZip(const char* DrvName, const char* zipFileName, std::vector<char>& CurrentVct);
INT32 ExtractIniFromZip(const char* DrvName, const char* zipFileName, std::vector<char>& CurrentIniCheat);

// ---------------------------------------------------------------------------
// command.dat move-list display
// ---------------------------------------------------------------------------

struct SymbolMapping {
	const char* key;
	const char* value;
};

int get_command_dat_count();
int AddCommandDatOptions(int command_idx_var);

#endif // NON_OFFICIAL_FEATURES_H