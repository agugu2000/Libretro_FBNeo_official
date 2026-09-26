#ifdef __LIBRETRO__
#define SKIP_STDIO_REDEFINES
#endif

#include <map>
#include <algorithm>

#include "non_official_features.h"
#include "retro_string.h"
#include "retro_common.h"
#include "burner.h"
#include "ugui_tools.h"
#include "retro_input.h"
#include "utf8_util.h"

// ---------------------------------------------------------------------------
// [NON-OFFICIAL HACK] externs for command.dat overlay
// ---------------------------------------------------------------------------

extern retro_environment_t        environ_cb;

// accessors implemented in libretro.cpp
extern void  NonOfficial_VideoCb(const void* data, unsigned w, unsigned h, size_t pitch);
extern void  NonOfficial_AudioBatchCb(const int16_t* data, size_t frames);
extern void* NonOfficial_GetPvidImage(void);
extern int   NonOfficial_GetNBurnBpp(void);
extern INT32 NonOfficial_GetNBurnPitch(void);
extern INT32 NonOfficial_GetNGameWidth(void);
extern INT32 NonOfficial_GetNGameHeight(void);
extern void* NonOfficial_GetPAudBuffer(void);
extern INT32 NonOfficial_GetNBurnSoundLen(void);


// ---------------------------------------------------------------------------
// Localized strings (18 entries, independent of official NUM_STRING)
// ---------------------------------------------------------------------------

const char* pSelLangStrNonOfficial[NUM_STRING_NONOFFICIAL];

const char* multi_language_strings_nonofficial[MAX_LANGUAGES][NUM_STRING_NONOFFICIAL] = {
	{
		"Shoulder Button L",
		"Shoulder Button R",
		"Shoulder Button L2",
		"Shoulder Button R2",
		"LeftStick Button L3",
		"RightStick Button R3",
		"Note: Keyboard users, please confirm the corresponding keys in <Quick Menu → Control>",
		"\"w m s\" represents Weak, Medium, Strong.\"P K\" represents Punch, Kick\nNote: Keyboard users, please confirm the corresponding keys in <Quick Menu → Control>",
		"Disable Combination",
		"Neo-Geo Combinations",
		"PGM Combinations",
		"CPS1 Combinations",
		"6keys-Layout Combinations",
		"Set and reload or reset game to take effect.",
		"Allow Ignore CRC",
		"The prerequisite is to enable 'Allow patched romsets'. Allowing rom with the correct file name and file size to run by ignoring CRC check. \nNote:By ignoring the CRC check, the game content loaded may not align with the intended game content",
		"Command",
		"Display the command list and information for current rom",
		"Off",
		"Show",
	},
	{
		"肩键 L",
		"肩键 R",
		"肩键 L2",
		"肩键 R2",
		"左摇杆键 L3",
		"右摇杆键 R3",
		"注：键盘使用者请于<快捷菜单→控制>中确认对应键",
		"\"w m s\"代表轻/中/重。\"P K\"代表拳和踢\n注：键盘使用者请于<快捷菜单→控制>中确认对应键",
		"取消组合键",
		"Neo-Geo 组合键",
		"PGM 组合键",
		"CPS1 组合键",
		"6键布局 组合键",
		"设定完毕后重新载入或重启游戏使其生效",
		"允许忽略CRC",
		"先决条件是启用「允许修补集组」。允许通过忽略CRC校验来运行具有正确文件名和文件大小的ROM。\n注意：忽略CRC校验可能会导致加载的游戏内容与预期的游戏内容不匹配",
		"出招表",
		"显示此ROM的出招表等信息",
		"关闭",
		"显示",
	},
	{
		"肩鍵 L",
		"肩鍵 R",
		"肩鍵 L2",
		"肩鍵 R2",
		"左搖桿鍵 L3",
		"右搖桿鍵 R3",
		"註：鍵盤使用者請於<快捷選单→控制器>中確認對應鍵",
		"\"w m s\"代表輕/中/重。\"P K\"代表拳和踢\n註：鍵盤使用者請於<快捷選单→控制器>中確認對應鍵",
		"取消組合鍵",
		"Neo-Geo 組合鍵",
		"PGM 組合鍵",
		"CPS1 組合鍵",
		"6鍵佈局 組合鍵",
		"設定完畢後重新載入或重啓游戲使其生效",
		"允許忽略CRC",
		"先决条件是啟用「允許修補集組」。允許通過忽略CRC校驗來運行具有正確文件名和文件大小的ROM。\n注意：忽略CRC校驗可能會導致加載的遊戲內容與預期的遊戲內容不匹配",
		"出招表",
		"顯示此ROM的出招表等訊息",
		"關閉",
		"顯示",
	}
};

// ---------------------------------------------------------------------------
// Custom combination-key macros
// ---------------------------------------------------------------------------

// Persistent descriptions for RA input descriptors (must outlive the call)
static char* R_button_description  = NULL;
static char* L_button_description  = NULL;
static char* R2_button_description = NULL;
static char* L2_button_description = NULL;
static char* R3_button_description = NULL;
static char* L3_button_description = NULL;

// Single-slot storage for the macro category of the currently active system
static macro_category var_fbneo_macro_categories;

bool bUseCustomMacroOnly = true;

// Update a persistent description string: only reallocate when the content
// actually changed. This avoids leaking a new char[] on every reassign.
static void UpdateButtonDescription(char*& desc, const std::string& key)
{
	if (desc != NULL && strcmp(desc, key.c_str()) == 0)
		return;						// unchanged, reuse existing pointer
	delete[] desc;
	desc = new char[key.size() + 1];
	strcpy(desc, key.c_str());
}

// Build the macro table for the given system and store it in
// var_fbneo_macro_categories. If "system" is not recognized, the storage is
// cleared.
static void initial_macro_categories(const char* system)
{
	macro_category table[4] = {
		{
			// PGM
			"pgm", "pgm_macro",
			{
				{ "fbneo-pgm-macro-l", "L", macro_desc_button_l, macro_info_button, "Buttons 3|4",
					{ { "Buttons 1|2", NULL }, { "Buttons 1|3", NULL }, { "Buttons 1|4", NULL }, { "Buttons 2|3", NULL }, { "Buttons 2|4", NULL }, { "Buttons 3|4", NULL }, { "Buttons 1|2|3", NULL }, { "Buttons 1|2|4", NULL }, { "Buttons 2|3|4", NULL }, { "Buttons 1|2|3|4", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-pgm-macro-r", "R", macro_desc_button_r, macro_info_button, "Buttons 1|2",
					{ { "Buttons 1|2", NULL }, { "Buttons 1|3", NULL }, { "Buttons 1|4", NULL }, { "Buttons 2|3", NULL }, { "Buttons 2|4", NULL }, { "Buttons 3|4", NULL }, { "Buttons 1|2|3", NULL }, { "Buttons 1|2|4", NULL }, { "Buttons 2|3|4", NULL }, { "Buttons 1|2|3|4", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-pgm-macro-l2", "L2", macro_desc_button_l2, macro_info_button, "Buttons 2|3",
					{ { "Buttons 1|2", NULL }, { "Buttons 1|3", NULL }, { "Buttons 1|4", NULL }, { "Buttons 2|3", NULL }, { "Buttons 2|4", NULL }, { "Buttons 3|4", NULL }, { "Buttons 1|2|3", NULL }, { "Buttons 1|2|4", NULL }, { "Buttons 2|3|4", NULL }, { "Buttons 1|2|3|4", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-pgm-macro-r2", "R2", macro_desc_button_r2, macro_info_button, "Buttons 1|2|3",
					{ { "Buttons 1|2", NULL }, { "Buttons 1|3", NULL }, { "Buttons 1|4", NULL }, { "Buttons 2|3", NULL }, { "Buttons 2|4", NULL }, { "Buttons 3|4", NULL }, { "Buttons 1|2|3", NULL }, { "Buttons 1|2|4", NULL }, { "Buttons 2|3|4", NULL }, { "Buttons 1|2|3|4", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
			}
		},
		{
			// Neo-Geo
			"neogeo", "neogeo_macro",
			{
				{ "fbneo-neogeo-macro-l", "L", macro_desc_button_l, macro_info_button, "Buttons CD",
					{ { "Buttons AB", NULL }, { "Buttons AC", NULL }, { "Buttons AD", NULL }, { "Buttons BC", NULL }, { "Buttons BD", NULL }, { "Buttons CD", NULL }, { "Buttons ABC", NULL }, { "Buttons ABD", NULL }, { "Buttons BCD", NULL }, { "Buttons ABCD", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-neogeo-macro-r", "R", macro_desc_button_r, macro_info_button, "Buttons AB",
					{ { "Buttons AB", NULL }, { "Buttons AC", NULL }, { "Buttons AD", NULL }, { "Buttons BC", NULL }, { "Buttons BD", NULL }, { "Buttons CD", NULL }, { "Buttons ABC", NULL }, { "Buttons ABD", NULL }, { "Buttons BCD", NULL }, { "Buttons ABCD", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-neogeo-macro-l2", "L2", macro_desc_button_l2, macro_info_button, "Buttons BCD",
					{ { "Buttons AB", NULL }, { "Buttons AC", NULL }, { "Buttons AD", NULL }, { "Buttons BC", NULL }, { "Buttons BD", NULL }, { "Buttons CD", NULL }, { "Buttons ABC", NULL }, { "Buttons ABD", NULL }, { "Buttons BCD", NULL }, { "Buttons ABCD", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-neogeo-macro-r2", "R2", macro_desc_button_r2, macro_info_button, "Buttons ABC",
					{ { "Buttons AB", NULL }, { "Buttons AC", NULL }, { "Buttons AD", NULL }, { "Buttons BC", NULL }, { "Buttons BD", NULL }, { "Buttons CD", NULL }, { "Buttons ABC", NULL }, { "Buttons ABD", NULL }, { "Buttons BCD", NULL }, { "Buttons ABCD", NULL }, { macro_disabled, NULL }, { NULL, NULL } } }
			}
		},
		{
			// CPS1
			"cps1", "cps1_macro",
			{
				{ "fbneo-cps1-macro-l", "L", macro_desc_button_l, macro_info_button, macro_disabled,
					{ { "Buttons Att|Jump", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-cps1-macro-r", "R", macro_desc_button_r, macro_info_button, macro_disabled,
					{ { "Buttons Att|Jump", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-cps1-macro-l2", "L2", macro_desc_button_l2, macro_info_button, macro_disabled,
					{ { "Buttons Att|Jump", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-cps1-macro-r2", "R2", macro_desc_button_r2, macro_info_button, "Buttons Att|Jump",
					{ { "Buttons Att|Jump", NULL }, { macro_disabled, NULL }, { NULL, NULL } } }
			}
		},
		{
			// Street Fighter 6-key layout
			"streetfighter", "streetfighter_macro",
			{
				{ "fbneo-streetfighter-macro-l2", "L2", macro_desc_button_l2, macro_info_streetfighter_button, "Buttons wP|mP|sP",
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-streetfighter-macro-r2", "R2", macro_desc_button_r2, macro_info_streetfighter_button, "Buttons wK|mK|sK",
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-streetfighter-macro-l3", "L3", macro_desc_button_l3, macro_info_streetfighter_button, macro_disabled,
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } } },
				{ "fbneo-streetfighter-macro-r3", "R3", macro_desc_button_r3, macro_info_streetfighter_button, macro_disabled,
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } } }
			}
		}
	};

	// Reset storage first, so an unknown system leaves it empty
	memset(&var_fbneo_macro_categories, 0, sizeof(var_fbneo_macro_categories));

	for (UINT32 i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
		if (strcmp(table[i].system, system) == 0) {
			var_fbneo_macro_categories = table[i];
			break;
		}
	}
}

// Return the number of bindable shoulder-button slots for the current macro
// category. var_fbneo_macro_categories holds a single system at a time.
int get_macro_count()
{
	if (var_fbneo_macro_categories.system == NULL)
		return 0;
	return sizeof(var_fbneo_macro_categories.options) / sizeof(var_fbneo_macro_categories.options[0]);
}

// Add the macro-related core options for the given system
int AddMacroOptions(const char* system, int nbr_macros, int idx_var)
{
	if (var_fbneo_macro_categories.system == NULL ||
		strcmp(var_fbneo_macro_categories.system, system) != 0)
		return idx_var;

	for (int macro_idx = 0; macro_idx < nbr_macros; macro_idx++) {
		macro_option* option = &var_fbneo_macro_categories.options[macro_idx];
		option_defs_us[idx_var].key = option->key;
		option_defs_us[idx_var].desc = option->option_name;
		option_defs_us[idx_var].desc_categorized = option->option_name;
		option_defs_us[idx_var].info = option->info;
		option_defs_us[idx_var].category_key = var_fbneo_macro_categories.category_name;
		for (int value_idx = 0; option->values[value_idx].value != NULL; value_idx++) {
			option_defs_us[idx_var].values[value_idx].value = option->values[value_idx].value;
		}
		option_defs_us[idx_var].default_value = option->default_value;
		idx_var++;
	}

	return idx_var;
}

// Extract the user-selected macro values from the core option variables
static void ProcessMacroContents(int nbr_macros, CustomMacroKeys& macrodata)
{
	for (int macro_idx = 0; macro_idx < nbr_macros; macro_idx++) {
		macro_option* option = &var_fbneo_macro_categories.options[macro_idx];
		struct retro_variable var = {0};
		var.key = option->key;

		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
			CustomMacroKey macroKey;
			macroKey.button = option->button;
			macroKey.macroKey = var.value;
			macrodata.macrocontent.push_back(macroKey);
		}
	}
}

// Load the user's macro selection for the given system
CustomMacroKeys LoadCustomMacroKeys(const char* system)
{
	CustomMacroKeys macrodata;

	if (var_fbneo_macro_categories.system != NULL &&
		strcmp(var_fbneo_macro_categories.system, system) == 0) {
		ProcessMacroContents(get_macro_count(), macrodata);
	}

	return macrodata;
}

// Translate the human-readable macro string (e.g. "Buttons 1|2", "Buttons
// Att|Jump", "Buttons wP|mP") into the internal compact form (e.g. "Buttons
// AB").
static std::string ReWriteMacro(const std::string& input, const std::string& system)
{
	std::string result = input;

	if (system == "pgm") {
		for (size_t i = 0; i < result.size(); ++i) {
			if (result[i] == '1') result[i] = 'A';
			else if (result[i] == '2') result[i] = 'B';
			else if (result[i] == '3') result[i] = 'C';
			else if (result[i] == '4') result[i] = 'D';
		}
	} else if (system == "cps1") {
		size_t pos;
		while ((pos = result.find("Att")) != std::string::npos) result.replace(pos, 3, "A");
		while ((pos = result.find("Jump")) != std::string::npos) result.replace(pos, 4, "B");
	} else if (system == "streetfighter") {
		size_t pos;
		while ((pos = result.find("wP")) != std::string::npos) result.replace(pos, 2, "A");
		while ((pos = result.find("mP")) != std::string::npos) result.replace(pos, 2, "B");
		while ((pos = result.find("sP")) != std::string::npos) result.replace(pos, 2, "C");
		while ((pos = result.find("wK")) != std::string::npos) result.replace(pos, 2, "D");
		while ((pos = result.find("mK")) != std::string::npos) result.replace(pos, 2, "E");
		while ((pos = result.find("sK")) != std::string::npos) result.replace(pos, 2, "F");
	}

	result.erase(std::remove(result.begin(), result.end(), '|'), result.end());
	return result;
}

// Forward declaration (implemented in retro_input.cpp)
INT32 GameInpDigital2RetroInpKey(struct GameInp* pgi, unsigned port, unsigned id,
                                 char* szn, unsigned device, unsigned nInput);

// Bind the user-selected macros to the corresponding GameInp entries
void BindCustomMacroKeys(const CustomMacroKeys& macrosdata, char* description,
                         int nPlayer, unsigned int* nDeviceType, struct GameInp* pgi)
{
	(void)nDeviceType;

	std::map<std::string, int> keyCount;

	for (size_t i = 0; i < macrosdata.macrocontent.size(); ++i) {
		std::string key = macrosdata.macrocontent[i].macroKey;
		const char* button = macrosdata.macrocontent[i].button.c_str();

		// Count current occurrence of this key so each slot has a unique
		// suffix (e.g. "Buttons AB01" .. "Buttons AB04")
		if (keyCount.find(key) == keyCount.end()) {
			keyCount[key] = 1;
		} else {
			keyCount[key]++;
		}

		char keyWithSuffix[64];
		snprintf_nowarn(keyWithSuffix, sizeof(keyWithSuffix), "%s%02d", key.c_str(), keyCount[key]);

		if (strcmp(keyWithSuffix, description) != 0)
			continue;

		if (strcmp("R", button) == 0) {
			UpdateButtonDescription(R_button_description, key);
			GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_R, R_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
		}
		if (strcmp("L", button) == 0) {
			UpdateButtonDescription(L_button_description, key);
			GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_L, L_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
		}
		if (strcmp("R2", button) == 0) {
			UpdateButtonDescription(R2_button_description, key);
			GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_R2, R2_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
		}
		if (strcmp("L2", button) == 0) {
			UpdateButtonDescription(L2_button_description, key);
			GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_L2, L2_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
		}
		if (strcmp("R3", button) == 0) {
			UpdateButtonDescription(R3_button_description, key);
			GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_R3, R3_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
		}
		if (strcmp("L3", button) == 0) {
			UpdateButtonDescription(L3_button_description, key);
			GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_L3, L3_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
		}
	}
}

// ---------------------------------------------------------------------------
// Button assignment helpers
// ---------------------------------------------------------------------------

// 4-button version (Neo-Geo / PGM)
void AssignButtons(const char* system, const char* szName, const char* szInfo,
                   int nPlayer, int i, int nButtons[][4])
{
	(void)szInfo;

	if (strcmp(system, "neogeo") == 0) {
		if (_stricmp(" Button A", szName + 2) == 0) nButtons[nPlayer][0] = i;
		if (_stricmp(" Button B", szName + 2) == 0) nButtons[nPlayer][1] = i;
		if (_stricmp(" Button C", szName + 2) == 0) nButtons[nPlayer][2] = i;
		if (_stricmp(" Button D", szName + 2) == 0) nButtons[nPlayer][3] = i;
	} else if (strcmp(system, "pgm") == 0) {
		if (_stricmp(" Button 1", szName + 2) == 0) nButtons[nPlayer][0] = i;
		if (_stricmp(" Button 2", szName + 2) == 0) nButtons[nPlayer][1] = i;
		if (_stricmp(" Button 3", szName + 2) == 0) nButtons[nPlayer][2] = i;
		if (_stricmp(" Button 4", szName + 2) == 0) nButtons[nPlayer][3] = i;
	}
}

// 2-button version (CPS1, must have both Attack and Jump)
void AssignButtons(const char* system, const char* szName, const char* szInfo,
                   int nPlayer, int i, int nButtons[][2], INT32 nCps1Tradition[])
{
	(void)szInfo;

	if (strcmp(system, "cps1") != 0)
		return;

	if (_stricmp(" Attack", szName + 2) == 0) {
		nButtons[nPlayer][0] = i;
		nCps1Tradition[nPlayer] |= 1;
	}
	if (_stricmp(" Jump", szName + 2) == 0) {
		nButtons[nPlayer][1] = i;
		nCps1Tradition[nPlayer] |= 2;
	}
}

// ---------------------------------------------------------------------------
// Bulk macro node creation
// ---------------------------------------------------------------------------

// Create macro nodes for every combination x every bindable button
struct GameInp* AddMacroKeys(struct GameInp* pgi, int nPlayer,
                             int nButtonsTwo[][2], int nButtonsFour[][4],
                             int nPunchInputs[][3], int nKickInputs[][3],
                             const char* system, UINT32& numMacroCount)
{
	struct BurnInputInfo bii;

	initial_macro_categories(system);

	if (var_fbneo_macro_categories.system == NULL ||
		strcmp(var_fbneo_macro_categories.system, system) != 0)
		return pgi;

	int num_prebindkeys = get_macro_count();
	int num_combs = 0;
	while (var_fbneo_macro_categories.options[0].values[num_combs].value != NULL &&
	       strncmp(var_fbneo_macro_categories.options[0].values[num_combs].value, "Buttons ", 8) == 0) {
		num_combs++;
	}

	// Stack-allocated work arrays: up to 20 combinations, each up to 6 buttons
	// plus a -1 terminator
	int button_combinations[20][7];
	const char* macros[20];

	for (int i = 0; i < num_combs; i++) {
		macros[i] = var_fbneo_macro_categories.options[0].values[i].value;

		// Rewrite "Buttons 1|2" etc. into "Buttons AB"
		std::string temp_macro = ReWriteMacro(macros[i], system);
		const char* button_str = temp_macro.c_str() + 8; // skip "Buttons "

		int j = 0;
		if (strstr(button_str, "A") != NULL) button_combinations[i][j++] = 0;
		if (strstr(button_str, "B") != NULL) button_combinations[i][j++] = 1;
		if (strstr(button_str, "C") != NULL) button_combinations[i][j++] = 2;
		if (strstr(button_str, "D") != NULL) button_combinations[i][j++] = 3;
		if (strstr(button_str, "E") != NULL) button_combinations[i][j++] = 4;
		if (strstr(button_str, "F") != NULL) button_combinations[i][j++] = 5;
		button_combinations[i][j] = -1;
	}

	for (int i = 0; i < num_combs; i++) {
		// Create num_prebindkeys copies of each combination so the user can bind
		// the same combination to every bindable shoulder button.
		for (int k = 1; k <= num_prebindkeys; k++) {
			pgi->nInput = GIT_MACRO_AUTO;
			pgi->nType = BIT_DIGITAL;
			if (nPunchInputs == NULL && nKickInputs == NULL) {
				pgi->Macro.nMode = 0;
			}

			snprintf_nowarn(pgi->Macro.szName, sizeof(pgi->Macro.szName),
			                "P%i %s%02d", nPlayer + 1, macros[i], k);

			for (int j = 0; j < 7 && button_combinations[i][j] != -1; j++) {
				if (nButtonsTwo != NULL) {
					BurnDrvGetInputInfo(&bii, nButtonsTwo[nPlayer][button_combinations[i][j]]);
				} else if (nButtonsFour != NULL) {
					BurnDrvGetInputInfo(&bii, nButtonsFour[nPlayer][button_combinations[i][j]]);
				} else if (nPunchInputs != NULL && nKickInputs != NULL) {
					if (button_combinations[i][j] < 3) {
						BurnDrvGetInputInfo(&bii, nPunchInputs[nPlayer][button_combinations[i][j]]);
					} else {
						BurnDrvGetInputInfo(&bii, nKickInputs[nPlayer][button_combinations[i][j] - 3]);
					}
				}
				pgi->Macro.pVal[j] = bii.pVal;
				pgi->Macro.nVal[j] = 1;
			}

			numMacroCount++;
			pgi++;
		}
	}

	return pgi;
}


// Extract "<drvName><ext>" from "<szAppCheatsPath><zipFileName>".
// drvName must be the same name the official core uses when looking for the
// same file on disk.
INT32 LoadCheatContentFromZip(const char* drvName, const char* ext,
                              const char* zipFileName, std::vector<char>& outContent)
{
	outContent.clear();

	// Build target file name: <drvName><ext>
	TCHAR targetName[MAX_PATH] = _T("");
	snprintf_nowarn(targetName, sizeof(targetName), "%s%s", drvName, ext);

	// Build container path: <szAppCheatsPath><zipFileName>
	// zipFileName must be without extension (e.g. "cheat"), the official
	// ZipOpen() will try ".zip" then ".7z" automatically.
	TCHAR zipPath[MAX_PATH] = _T("");
	snprintf_nowarn(zipPath, sizeof(zipPath), "%s%s", szAppCheatsPath, zipFileName);

	void* dest = NULL;
	INT32 pnWrote = 0;

	// Official ZipLoadOneFile() opens the container, finds the file by name,
	// extracts it and allocates the destination buffer.
	// On error, it frees *Dest itself, so we must not free it again.
	INT32 ret = ZipLoadOneFile((char*)zipPath, targetName, &dest, &pnWrote);
	if (ret != 0) {
		return 1;
	}

	if (dest && pnWrote > 0) {
		char* content = (char*)dest;
		outContent.insert(outContent.end(), content, content + pnWrote);
	}
	if (dest) free(dest);

	return 0;
}

// ---------------------------------------------------------------------------
// Cheat content caches (zip/7z container support is above)
// ---------------------------------------------------------------------------

// cheat content caches
std::vector<char> g_CurrentMameCheatContent;
std::vector<char> g_CurrentWayderCheatContent;
std::vector<char> g_CurrentIniCheatContent;
std::vector<char> g_CurrentVctContent;

// parent caches
std::vector<char> g_CurrentParentCheatContent;
std::vector<char> g_CurrentWayderParentCheatContent;

// extracted flags
bool g_bMameCheatExtracted = false;
bool g_bWayderCheatExtracted = false;

// source flags
bool g_bVctFromZip = false;
bool g_bIniFromZip = false;


// ---------------------------------------------------------------------------
// command.dat move-list display
// ---------------------------------------------------------------------------
static std::vector<std::string> g_display_lines;
static void BuildDisplayLines(const std::string& src, int max_width) {
    g_display_lines.clear();

    size_t line_start = 0;
    while (line_start <= src.size()) {
        size_t line_end = src.find('\n', line_start);
        if (line_end == std::string::npos) line_end = src.size();

        std::string line = src.substr(line_start, line_end - line_start);

        if (line.empty()) {
            g_display_lines.push_back("");
        } else {
            size_t pos = 0;
            while (pos < line.size()) {
                int width = 0;
                size_t end = pos;
                while (end < line.size()) {
                    int cp = 0;
                    size_t len = utf8_val(&cp, &line[end]);
                    if (cp == -1 || len == 0) {
                        len = 1;
                        cp = (unsigned char)line[end];
                    }
                    int adv = gui_get_glyph_advance((unsigned int)cp);
                    if (width + adv > max_width) break;
                    width += adv;
                    end += len;
                }

                if (end == pos) {
                    end = pos + 1;
                }

                g_display_lines.push_back(line.substr(pos, end - pos));
                pos = end;
            }
        }

        if (line_end == src.size()) break;
        line_start = line_end + 1;
    }
}

// ---------------------------------------------------------------------------
// command.dat move-list overlay (uGUI)
// ---------------------------------------------------------------------------

static int      s_cmd_page           = 0;
static int      s_cmd_lines_per_page = 0;
static int      s_cmd_page_count     = 0;
static bool     s_cmd_overlay_on     = false;
static uint16_t s_cmd_last_joy       = 0;
static bool     s_cmd_wait_b_release = false;

static int CommandDatPageCount(int lines_per_page)
{
    int total = (int)g_display_lines.size();
    if (total <= 0) return 0;
    return (total + lines_per_page - 1) / lines_per_page;
}

static bool CommandDatGetPage(int page, int lines_per_page, std::string& out)
{
    int total = (int)g_display_lines.size();
    if (total <= 0) return false;

    int start = page * lines_per_page;
    if (start >= total) return false;
    int end = start + lines_per_page;
    if (end > total) end = total;

    out.clear();
    for (int i = start; i < end; i++) {
        out += g_display_lines[i];
        out += "\n";
    }
    return true;
}

static void MakeCommandTitle(char *buf, size_t size)
{
    const char *name = BurnDrvGetTextA(DRV_NAME);
    if (name && name[0]) {
        snprintf(buf, size, "Command:<%s>", name);
    } else {
        snprintf(buf, size, "Command");
    }
}

int AddCommandDatOption(int idx_var)
{
    option_defs_us[idx_var].key              = "fbneo-command-dat";
    option_defs_us[idx_var].desc             = RETRO_COMMAND_DAT_CAT_DESC;
    option_defs_us[idx_var].desc_categorized = RETRO_COMMAND_DAT_CAT_DESC;
    option_defs_us[idx_var].info             = RETRO_COMMAND_DAT_CAT_INFO;
    option_defs_us[idx_var].category_key     = "command_dat";
    option_defs_us[idx_var].values[0].value  = "Off";
    option_defs_us[idx_var].values[0].label  = RETRO_COMMAND_OFF_LABEL;
    option_defs_us[idx_var].values[1].value  = "Show";
    option_defs_us[idx_var].values[1].label  = RETRO_COMMAND_SHOW_LABEL;
    option_defs_us[idx_var].values[2].value  = NULL;
    option_defs_us[idx_var].values[2].label  = NULL;
    option_defs_us[idx_var].default_value    = "Off";
    return idx_var + 1;
}

void UpdateCommandDatOptionVisibility(void)
{
    struct retro_core_option_display option_display;
    option_display.key = "fbneo-command-dat";
    option_display.visible = !CommandDat::Get().records.empty();
    environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY, &option_display);
}

int CommandDatOverlayTick(void)
{
    static int16_t silence_buffer[4096];

    // ---- 等待 B 释放（关闭 overlay 后吞掉 B 直到松开） ----
    if (s_cmd_wait_b_release) {
        uint16_t joy = input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
        if (joy & (1 << RETRO_DEVICE_ID_JOYPAD_B)) {
            NonOfficial_VideoCb(NonOfficial_GetPvidImage(),
                                NonOfficial_GetNGameWidth(),
                                NonOfficial_GetNGameHeight(),
                                NonOfficial_GetNBurnPitch());
            INT32 n = NonOfficial_GetNBurnSoundLen();
            if (n > 4096) n = 4096;
            if (n > 0) {
                memset(silence_buffer, 0, sizeof(int16_t) * n);
                NonOfficial_AudioBatchCb(silence_buffer, n);
            }
            return 1;
        }
        s_cmd_wait_b_release = false;
    }

    struct retro_variable var = {0};
    var.key = "fbneo-command-dat";

    bool want_show = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
        want_show = (strcmp(var.value, "Show") == 0);
    }

	// ---- 打开 ----
    if (want_show && !s_cmd_overlay_on) {
		const CommandDat::Result& result = CommandDat::Get();
		if (result.records.empty()) return 0;

		std::string all;
		for (size_t i = 0; i < result.records.size(); i++) {
			all += result.records[i].data;
			all += "\n";
		}

        gui_init(NonOfficial_GetNGameWidth(), NonOfficial_GetNGameHeight(), sizeof(unsigned));
		// gui_init(640, 480, sizeof(unsigned));

		int inner_w = gui_get_content_width();
		BuildDisplayLines(all, inner_w);

		int inner_h = gui_get_content_height();
		int line_h  = gui_get_line_height();
		if (line_h < 1) line_h = 1;
		s_cmd_lines_per_page = inner_h / line_h;
		if (s_cmd_lines_per_page < 1) s_cmd_lines_per_page = 1;
		s_cmd_page_count = CommandDatPageCount(s_cmd_lines_per_page);

        s_cmd_page = 0;
        std::string page_text;
        if (!CommandDatGetPage(s_cmd_page, s_cmd_lines_per_page, page_text))
            return 0;

		gui_set_message(page_text.c_str());
		{
			char title[160];
			MakeCommandTitle(title, sizeof(title));
			gui_set_window_title_with_page(title, s_cmd_page + 1, s_cmd_page_count);
		}
		gui_show_overlay();

        s_cmd_overlay_on = true;
        s_cmd_last_joy   = input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
    }
    // ---- 关闭（option 切回 Off） ----
    else if (!want_show && s_cmd_overlay_on) {
        gui_hide_overlay();
        s_cmd_overlay_on = false;
    }

    if (!s_cmd_overlay_on || !gui_is_overlay_visible())
        return 0;

    // ---- 输入 ----
    uint16_t joy     = input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
    uint16_t pressed = joy & ~s_cmd_last_joy;
    s_cmd_last_joy   = joy;

    bool need_redraw = false;

    // L 或 左 → 上一页
    if (pressed & ((1 << RETRO_DEVICE_ID_JOYPAD_L) | (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))) {
        if (s_cmd_page > 0) { s_cmd_page--; need_redraw = true; }
    }
    // R 或 右 → 下一页
	if (pressed & ((1 << RETRO_DEVICE_ID_JOYPAD_R) | (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))) {
		if (s_cmd_page + 1 < s_cmd_page_count) { s_cmd_page++; need_redraw = true; }
	}
    // B → 关闭
    if (pressed & (1 << RETRO_DEVICE_ID_JOYPAD_B)) {
        struct retro_variable v = {0};
        v.key   = "fbneo-command-dat";
        v.value = "Off";
        environ_cb(RETRO_ENVIRONMENT_SET_VARIABLE, &v);
        gui_hide_overlay();
        s_cmd_overlay_on     = false;
        s_cmd_wait_b_release = true;
        return 1;
    }

	if (need_redraw) {
		std::string page_text;
		if (CommandDatGetPage(s_cmd_page, s_cmd_lines_per_page, page_text)) {
			gui_set_message(page_text.c_str());
			{
				char title[160];
				MakeCommandTitle(title, sizeof(title));
				gui_set_window_title_with_page(title, s_cmd_page + 1, s_cmd_page_count);
			}
		}
	}

    // ---- 渲染 + 合成 + 静音 ----
    gui_draw();
	gui_blend_onto(NonOfficial_GetPvidImage(), NonOfficial_GetNBurnBpp(),
				NonOfficial_GetNGameWidth(), NonOfficial_GetNGameHeight(),
				NonOfficial_GetNBurnPitch());
    NonOfficial_VideoCb(NonOfficial_GetPvidImage(),
                        NonOfficial_GetNGameWidth(),
                        NonOfficial_GetNGameHeight(),
                        NonOfficial_GetNBurnPitch());
	// NonOfficial_VideoCb(gui_get_framebuffer(),
	// 					640, 480,
	// 					640 * sizeof(unsigned));

    {
        INT32 n = NonOfficial_GetNBurnSoundLen();
        if (n > 4096) n = 4096;
        if (n > 0) {
            memset(silence_buffer, 0, sizeof(int16_t) * n);
            NonOfficial_AudioBatchCb(silence_buffer, n);
        }
    }

    return 1;
}

void ResetCommandDatOption(void)
{
    struct retro_variable var = {0};
    var.key   = "fbneo-command-dat";
    var.value = "Off";
    environ_cb(RETRO_ENVIRONMENT_SET_VARIABLE, &var);
}

void ResetCommandDatCache(void)
{
    CommandDat::Unload();
    g_display_lines.clear();
    s_cmd_page = 0;
    s_cmd_lines_per_page = 0;
    s_cmd_page_count = 0;
    s_cmd_overlay_on = false;
    s_cmd_wait_b_release = false;
}

// [NON-OFFICIAL HACK] Extract the first contiguous block matching matchDrvName
// from a MAME cheat.dat into cache. Stops at "----:REASON" or at the first
// non-matching line after a match, mirroring official ConfigParseMAMEFile_internal.
INT32 ExtractMameCheatFromDat(FILE* MameDatCheat, const TCHAR* matchDrvName, std::vector<char>& cache)
{
    cache.clear();

    TCHAR szLine[1024];
    TCHAR gName[64];
    _stprintf(gName, _T(":%s:"), matchDrvName);

    bool foundData = false;

    while (_fgetts(szLine, 1024, MameDatCheat) != NULL) {
        // 对齐官方行为 C：遇到 ----:REASON，停止
        if (foundData && _tcsstr(szLine, _T("----:REASON"))) {
            break;
        }

        // 对齐官方行为 A/B
#if defined(BUILD_WIN32)
        bool matches = (_tcsncmp(szLine, gName, lstrlen(gName)) == 0);
#else
        bool matches = (_tcsncmp(szLine, gName, strlen(gName)) == 0);
#endif

        if (!matches) {
            if (foundData) break;      // 已找到当前驱动，遇到不匹配行，停止
            else continue;             // 还没找到，跳过
        }

        if (!foundData) foundData = true;
        for (TCHAR* p = szLine; *p; ++p) {
            cache.push_back(*p);
        }
    }

    return foundData ? 0 : 1;
}

// [NON-OFFICIAL HACK] Extract <DrvName>.ini from zip/7z, resolving include recursively (depth 5)
INT32 ExtractIniFromZip(const char* DrvName, const char* zipFileName, std::vector<char>& CurrentIniCheat)
{
	CurrentIniCheat.clear();

	if (LoadCheatContentFromZip(DrvName, ".ini", zipFileName, CurrentIniCheat) != 0) {
		return 1;
	}

	int depth = 0;
	bool processInclude = true;
	// max searching included files 5 depth
	while (processInclude && depth < 5) {
		processInclude = false;
		if (CurrentIniCheat.empty()) break;
		std::vector<char> newContent;
		const char* iniPtr = CurrentIniCheat.data();
		std::vector<char> szLine;

		// Let's check each line of CurrentIniCheat
		// Looking for include file and hooking them to CurrentIniCheat
		while (*iniPtr) {
			szLine.clear();
			while (*iniPtr && *iniPtr != '\n') {
				szLine.push_back(*iniPtr++);
			}
			if (*iniPtr == '\n') {
				szLine.push_back(*iniPtr++);
			}
			szLine.push_back('\0');

			char* t;
			char include_kw[] = "include";
			if ((t = LabelCheck(szLine.data(), include_kw)) != 0) {
				processInclude = true;
				char* szQuote = NULL;
				QuoteRead(&szQuote, NULL, t);

				if (szQuote) {
					std::vector<char> includedContent;

					if (LoadCheatContentFromZip(szQuote, ".ini", zipFileName, includedContent) == 0) {
						newContent.insert(newContent.end(), includedContent.begin(), includedContent.end());
						newContent.push_back('\n');
					}
				}
			} else {
				newContent.insert(newContent.end(), szLine.begin(), szLine.end() - 1);
			}
		}

		CurrentIniCheat = newContent;
		depth++;
	}

	return 0;
}

// [NON-OFFICIAL HACK] Extract <DrvName>.vct from zip/7z
INT32 ExtractVctFromZip(const char* DrvName, const char* zipFileName, std::vector<char>& CurrentVct)
{
	return LoadCheatContentFromZip(DrvName, ".vct", zipFileName, CurrentVct);
}

#undef NUM_STRING_NONOFFICIAL