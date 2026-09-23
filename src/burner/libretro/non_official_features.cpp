#ifdef __LIBRETRO__
#define SKIP_STDIO_REDEFINES
#endif

#include "non_official_features.h"
#include "retro_string.h"
#include "retro_common.h"
#include "burner.h"

#include <map>
#include <algorithm>

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
		"Display the command list and information for current rom(recommend using menu driver:ozone/glui)",
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
		"显示此ROM的出招表等信息(推荐使用菜单驱动ozone/glui)",
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
		"顯示此ROM的出招表等訊息(推薦使用選單主題ozone/glui)",
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

// TODO: mapping table should be aligned with MAME source:
//       https://github.com/mamedev/mame/blob/master/plugins/data/button_char.lua
// Unicode is used to approximate MAME's built-in icons as closely as possible.
static SymbolMapping SymbolList[] = {
	{ "_A", "Ⓐ" }, { "_$", "▲" }, { "@L-punch", "[ⓁⓅ]" },
	{ "_B", "Ⓑ" }, { "_#", "▣" }, { "@M-punch", "[ⓂⓅ]" },
	{ "_C", "Ⓒ" }, { "_]", "□" }, { "@S-punch", "[ⓈⓅ]" },
	{ "_D", "Ⓓ" }, { "_[", "■" }, { "@L-kick", "[ⓁⓀ]" },
	{ "_H", "Ⓗ" }, { "_{", "▽" }, { "@M-kick", "[ⓂⓀ]" },
	{ "_Z", "Ⓩ" }, { "_}", "▼" }, { "@S-kick", "[ⓈⓀ]" },
	{ "_a", "①" }, { "_<", "◇" }, { "@3-kick", "[③Ⓚ]" },
	{ "_b", "②" }, { "_>", "◆" }, { "@3-punch", "[③Ⓟ]" },
	{ "_c", "③" }, { "^s", "Ⓢ" }, { "@2-kick", "[②Ⓚ]" },
	{ "_d", "④" }, { "^S", "[Ⓢⓔⓛ]" }, { "@2-punch", "[②Ⓟ]" },
	{ "_e", "⑤" }, { "^E", "[ⓁⓅ]" }, { "@custom1", "①" },
	{ "_f", "⑥" }, { "^F", "[ⓂⓅ]" }, { "@custom2", "②" },
	{ "_g", "⑦" }, { "^G", "[ⓈⓅ]" }, { "@custom3", "③" },
	{ "_h", "⑧" }, { "^H", "[ⓁⓀ]" }, { "@custom4", "④" },
	{ "_i", "⑨" }, { "^I", "[ⓂⓀ]" }, { "@custom5", "⑤" },
	{ "_j", "⑩" }, { "^J", "[ⓈⓀ]" }, { "@custom6", "⑥" },
	{ "_+", "＋" }, { "^T", "[③Ⓚ]" }, { "@custom7", "⑦" },
	{ "_.", "…" }, { "^U", "[③Ⓟ]" }, { "@custom8", "⑧" },
	{ "_1", "↙" }, { "^V", "[②Ⓚ]" }, { "@up", "↑" },
	{ "_2", "↓" }, { "^W", "[②Ⓟ]" }, { "@down", "↓" },
	{ "_3", "↘" }, { "^!", "↳" }, { "@left", "←" },
	{ "_4", "←" }, { "^1", "⇙" }, { "@right", "→" },
	{ "_5", "●" }, { "^2", "⇓" }, { "@lever", "[Ⓟⓝ]" },
	{ "_6", "→" }, { "^3", "⇘" }, { "@nplayer", "[Ⓟⓝ]" },
	{ "_7", "↖" }, { "^4", "⇐" }, { "@1player", "[Ⓟ①]" },
	{ "_8", "↑" }, { "^6", "⇒" }, { "@2player", "[Ⓟ②]" },
	{ "_9", "↗" }, { "^7", "⇖" }, { "@3player", "[Ⓟ③]" },
	{ "_N", "N" }, { "^8", "⇑" }, { "@4player", "[Ⓟ④]" },
	{ "_S", "[Ⓢⓣ]" }, { "^9", "⇗" }, { "@5player", "[Ⓟ⑤]" },
	{ "_P", "Ⓟ" }, { "^M", "[Ⓜⓐⓧ]" }, { "@6player", "[Ⓟ⑥]" },
	{ "_K", "Ⓚ" }, { "^-", "⇥" }, { "@7player", "[Ⓟ⑦]" },
	{ "_G", "Ⓖ" }, { "^=", "⇤" }, { "@8player", "[Ⓟ⑧]" },
	{ "_!", "→" }, { "^*", "[ⓢⓉⓐⓟ]" }, { "@-->","→" },
	{ "_k", "[←◒]" }, { "^?", "[Ⓑⓣⓝ?]" }, { "@==>", "↳" },
	{ "_l", "[→◓]" }, { "@A-button", "Ⓐ" }, { "@hcb", "[←◒]" },
	{ "_m", "[→◒]" }, { "@B-button", "Ⓑ" }, { "@huf", "[→◓]" },
	{ "_n", "[←◓]" }, { "@C-button", "Ⓒ" }, { "@hcf", "[→◒]" },
	{ "_o", "[↓◶]" }, { "@D-button", "Ⓓ" }, { "@hub", "[←◓]" },
	{ "_p", "[←◵]" }, { "@E-button", "Ⓔ" }, { "@qfd", "[↓◶]" },
	{ "_q", "[↑◴]" }, { "@F-button", "Ⓕ" }, { "@qdb", "[←◵]" },
	{ "_r", "[→◷]" }, { "@G-button", "Ⓖ" }, { "@qbu", "[↑◴]" },
	{ "_s", "[↓◵]" }, { "@H-button", "Ⓗ" }, { "@quf", "[→◷]" },
	{ "_t", "[↑◶]" }, { "@I-button", "Ⓘ" }, { "@qbd", "[↓◵]" },
	{ "_u", "[↑◷]" }, { "@J-button", "Ⓙ" }, { "@qdf", "[↑◶]" },
	{ "_v", "[←◴]" }, { "@K-button", "Ⓚ" }, { "@qfu", "[↑◷]" },
	{ "_w", "[↻◯]" }, { "@L-button", "Ⓛ" }, { "@qub", "[←◴]" },
	{ "_x", "[↻◯]" }, { "@M-button", "Ⓜ" }, { "@fdf", "[↻◯]" },
	{ "_y", "[↺◯]" }, { "@N-button", "Ⓝ" }, { "@fub", "[↻◯]" },
	{ "_z", "[↺◯]" }, { "@O-button", "Ⓞ" }, { "@fuf", "[↺◯]" },
	{ "_L", "↠" }, { "@P-button", "Ⓟ" }, { "@fdb", "[↺◯]" },
	{ "_M", "↞" }, { "@Q-button", "Ⓠ" }, { "@xff", "⇥" },
	{ "_Q", "[Ⓓⓡⓐⓖⓞⓝ⇒]" }, { "@R-button", "Ⓡ" }, { "@xbb", "⇤" },
	{ "_R", "[Ⓓⓡⓐⓖⓞⓝ⇐]" }, { "@S-button", "Ⓢ" }, { "@dsf", "[Ⓓⓡⓐⓖⓞⓝ⇒]" },
	{ "_^", "[Ⓐⓘⓡ]" }, { "@T-button", "Ⓣ" }, { "@dsb", "[Ⓓⓡⓐⓖⓞⓝ⇐]" },
	{ "_?", "[Ⓓⓘⓡ]" }, { "@U-button", "Ⓤ" }, { "@AIR", "[Ⓐⓘⓡ]" },
	{ "_X", "[Ⓣⓐⓟ]" }, { "@V-button", "Ⓥ" }, { "@DIR", "[Ⓓⓘⓡ]" },
	{ "_|", "[Ⓙⓤⓜⓟ]" }, { "@W-button", "Ⓦ" }, { "@MAX", "[Ⓜⓐⓧ]" },
	{ "_O", "[Ⓗⓞⓛⓓ]" }, { "@X-button", "Ⓧ" }, { "@TAP", "[Ⓣⓐⓟ]" },
	{ "_-", "[Ⓐⓘⓡ]" }, { "@Y-button", "Ⓨ" }, { "@jump", "[Ⓙⓤⓜⓟ]" },
	{ "_=", "[Ⓢⓠⓤⓐⓣ]" }, { "@Z-button", "Ⓩ" }, { "@hold", "[Ⓗⓞⓛⓓ]" },
	{ "_~", "[Ⓒⓗⓐⓡⓖⓔ]" }, { "@decrease", "⊕" }, { "@air", "[ⓐⓘⓡ]" },
	{ "_`", "•" }, { "@increase", "⊖" }, { "@sit", "[Ⓢⓠⓤⓐⓣ]" },
	{ "_@", "◎" }, { "@BALL", "●" }, { "@close", "⇥" },
	{ "_)", "○" }, { "@start", "[Ⓢⓣ]" }, { "@away", "⇤" },
	{ "_(", "●" }, { "@select", "[Ⓢⓔⓛ]" }, { "@charge", "[Ⓒⓗⓐⓡⓖⓔ]" },
	{ "_*", "☆" }, { "@punch", "Ⓟ" }, { "@tap", "[ⓢⓉⓐⓟ]" },
	{ "_&", "★" }, { "@kick", "Ⓚ" }, { "@button", "[Ⓑⓣⓝ?]" },
	{ "_%", "△" }, { "@guard", "Ⓖ" }
};

static SymbolMapping SymbolList_ChineseAlignment[] = {
	{ "═", "\uFF1D" },	// full-width equals sign
	{ "│", "\uFF5C" },	// full-width vertical bar
	{ "  ", "\u3000" }	// full-width space
};

static std::map<std::string, std::string> symbolMap;
static std::map<std::string, std::string> symbolMap_ChineseAlignment;

static void InitializeSymbolMap()
{
	int SymbolListSize = sizeof(SymbolList) / sizeof(SymbolList[0]);
	for (int i = 0; i < SymbolListSize; ++i) {
		symbolMap[SymbolList[i].key] = SymbolList[i].value;
	}
}

static void InitializeSymbolMap_ChineseAlignment()
{
	int SymbolListSize = sizeof(SymbolList_ChineseAlignment) / sizeof(SymbolList_ChineseAlignment[0]);
	for (int i = 0; i < SymbolListSize; ++i) {
		symbolMap_ChineseAlignment[SymbolList_ChineseAlignment[i].key] = SymbolList_ChineseAlignment[i].value;
	}
}

// Replace MAME move-list symbols using symbolMap
static std::string ReplaceSymbols(const std::string& input)
{
	std::string result = input;
	std::map<std::string, std::string>::iterator it;

	for (it = symbolMap.begin(); it != symbolMap.end(); ++it) {
		size_t pos = 0;
		while ((pos = result.find(it->first, pos)) != std::string::npos) {
			result.replace(pos, it->first.length(), it->second);
			pos += it->second.length();
		}
	}
	return result;
}

// Replace characters to keep graphical move-lists aligned in Chinese
static std::string ReplaceSymbols_ChineseAlignment(const std::string& input)
{
	std::string result = input;
	std::map<std::string, std::string>::iterator it;

	for (it = symbolMap_ChineseAlignment.begin(); it != symbolMap_ChineseAlignment.end(); ++it) {
		size_t pos = 0;
		while ((pos = result.find(it->first, pos)) != std::string::npos) {
			result.replace(pos, it->first.length(), it->second);
			pos += it->second.length();
		}
	}
	return result;
}

static std::vector<std::string> CommandDataLine;

static std::string TrimNewLine(char* line)
{
	std::string lineStr(line);
	lineStr.erase(std::remove(lineStr.begin(), lineStr.end(), '\r'), lineStr.end());
	lineStr.erase(std::remove(lineStr.begin(), lineStr.end(), '\n'), lineStr.end());

	return lineStr;
}

static bool ReadCommand_Dat()
{
	TCHAR line[4096] = {0};
	TCHAR buffer[256] = {0};
	std::string LineStr;
	std::string LastLineStr;
	std::string token;
	std::string drv_name(BurnDrvGetText(DRV_NAME));
	TCHAR szFilename[MAX_PATH] = _T("");
	bool foundInfo = false;
	FILE* cmdFile = NULL;
	bool containsChinese = false;

	InitializeSymbolMap();

	snprintf_nowarn(szFilename, sizeof(szFilename), "%scommand.dat", szAppCommandPath);
	cmdFile = fopen(szFilename, _T("rt"));
	if (cmdFile == NULL) {
		return false;
	}

	while (_fgetts(line, sizeof(line), cmdFile) != NULL) {
		// Skip comment lines
		if (line[0] == '#') {
			continue;
		}

		// Look for a matching "$info=" line
		if (!foundInfo) {
			if (strncmp(line, "$info=", 6) == 0) {
				std::string info_line(line + 6);
				std::istringstream iss(info_line);
				while (std::getline(iss, token, ',')) {
					strncpy(buffer, token.c_str(), sizeof(buffer) - 1);
					buffer[sizeof(buffer) - 1] = '\0';
					token = TrimNewLine(buffer);
					if (token == drv_name) {
						foundInfo = true;
						break;
					}
				}
				continue;
			}
		}

		// Once matched, collect every line until the next "$info=" or a blank line
		if (foundInfo) {
			LineStr = TrimNewLine(line);

			if (strncmp(LineStr.c_str(), "$cmd", 4) == 0) {
				// After a $end, insert a blank line for visual separation
				if (strncmp(LastLineStr.c_str(), "$end", 4) == 0) {
					CommandDataLine.push_back("");
				}
				LastLineStr = LineStr;
				continue;
			}
			if (strncmp(LineStr.c_str(), "$end", 4) == 0) {
				LastLineStr = LineStr;
				continue;
			}
			if (LineStr.empty()) {
				// Avoid duplicated consecutive blank lines
				if (LastLineStr.empty()) {
					continue;
				}
				CommandDataLine.push_back("");
				LastLineStr = LineStr;
				continue;
			}
			if (strncmp(LineStr.c_str(), "$info=", 6) == 0) {
				break;
			}
			CommandDataLine.push_back(ReplaceSymbols(LineStr));
			LastLineStr = LineStr;
		}
	}

	while (!CommandDataLine.empty() && CommandDataLine.back().empty()) {
		CommandDataLine.pop_back();
	}

	// Detect Chinese characters and adjust alignment if present
	if (!CommandDataLine.empty()) {
		for (size_t i = 0; i < CommandDataLine.size(); ++i) {
			for (size_t j = 0; j < CommandDataLine[i].size(); ++j) {
				unsigned char c1 = CommandDataLine[i][j];
				if ((c1 & 0xF0) == 0xE0) {
					if (j + 2 < CommandDataLine[i].size()) {
						unsigned char c2 = CommandDataLine[i][j + 1];
						unsigned char c3 = CommandDataLine[i][j + 2];
						if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80) {
							unsigned int unicode = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
							if (unicode >= 0x4E00 && unicode <= 0x9FFF) {
								containsChinese = true;
								break;
							}
						}
					}
				}
			}
		}
	}

	if (containsChinese) {
		InitializeSymbolMap_ChineseAlignment();
		for (size_t i = 0; i < CommandDataLine.size(); ++i) {
			CommandDataLine[i] = ReplaceSymbols_ChineseAlignment(CommandDataLine[i]);
		}
	}

	fclose(cmdFile);
	return foundInfo;
}

int get_command_dat_count()
{
	if (CommandDataLine.size() == 0) {
		if (!ReadCommand_Dat()) {
			return 0;
		}
	}
	return CommandDataLine.size();
}

static std::vector<std::string> CommandKeys;

int AddCommandDatOptions(int command_idx_var)
{
	// Guard against stale keys from a previous call
	CommandKeys.clear();

	if (CommandDataLine.size() == 0) {
		if (!ReadCommand_Dat()) {
			return command_idx_var;
		}
	}

	const int maxCommandBlocks = CommandDataLine.size();
	char key[64];

	for (int i = 0; i < maxCommandBlocks; i++) {
		snprintf_nowarn(key, sizeof(key), "fbneo-commanddat-%d", i);
		CommandKeys.push_back(key);
	}

	for (int j = 0; j < maxCommandBlocks; j++) {
		option_defs_us[command_idx_var].key              = CommandKeys[j].c_str();
		option_defs_us[command_idx_var].desc             = " ";
		option_defs_us[command_idx_var].desc_categorized = CommandDataLine[j].c_str();
		option_defs_us[command_idx_var].info             = NULL;
		option_defs_us[command_idx_var].category_key     = "command_dat";
		option_defs_us[command_idx_var].values[0].value  = " ";
		option_defs_us[command_idx_var].values[1].value  = NULL;
		option_defs_us[command_idx_var].default_value    = " ";
		command_idx_var++;
	}

	return command_idx_var;
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