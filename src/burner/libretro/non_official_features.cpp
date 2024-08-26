#include "non_official_features.h"
#include "retro_string.h"
#include "retro_common.h"
#include <map>
#include <algorithm>

/* 字符串资源相关*/
const char *pSelLangStrNonOfficial[NUM_STRING_NONOFFICIAL];

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
		"Choose Cheat Source File",
		#ifdef INCLUDE_7Z_SUPPORT
		"Select Cheat Source File in 'system/fbneo/cheats/'.If setting to <Default>,cheats will be loaded in the order of cheat.dat -> ini file compressed in zip file -> ini file compressed in 7z file-> ini file uncompressed.The cheat loading will stop once found\nRestart or Reload to take effect!",
		#else
		"Select Cheat Source File in 'system/fbneo/cheats/'.If setting to <Default>,cheats will be loaded in the order of cheat.dat -> ini file compressed in zip file -> ini file uncompressed.The cheat loading will stop once found\nRestart or Reload to take effect!",
		#endif
		"Default",
		"Command",
		"Display the command list and information for current rom(recommend using menu driver:ozone/glui)",
		"Error:Failed to set cheat file.\nPlease check the file permissions for 'system/fbneo/cheats/%s.SetToLoad'.\n"
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
		"选择金手指源文件",
		#ifdef INCLUDE_7Z_SUPPORT
		"在 'system/fbneo/cheats/' 目录下选择金手指源文件。如果设置为 <默认值>，作弊码将按照以下顺序加载：cheat.dat -> zip中的 ini 文件 -> 7z中的 ini 文件-> 未压缩的 ini 文件。一旦找到作弊码，加载将停止\n重启或关闭游戏再次运行以生效！",
		#else
		"在 'system/fbneo/cheats/' 目录下选择金手指源文件。如果设置为 <默认值>，作弊码将按照以下顺序加载：cheat.dat -> zip中的 ini 文件 -> 未压缩的 ini 文件。一旦找到作弊码，加载将停止\n重启或关闭游戏再次运行以生效！",
		#endif
		"默认值",
		"出招表",
		"显示此ROM的出招表等信息(推荐使用菜单驱动ozone/glui)",
		"错误:设置金手指源文件失败。\n请检查'system/fbneo/cheats/%s.SetToLoad'文件权限。\n"
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
		"選擇金手指源文件",
		#ifdef INCLUDE_7Z_SUPPORT
		"在 'system/fbneo/cheats/' 目錄下選擇金手指源文件。如果設定為 <預設值>，作弊碼將按照以下順序載入：cheat.dat -> zip中的 ini 檔案 -> 7z中的 ini 檔案-> 未壓縮的 ini 檔案。一旦找到作弊碼，載入將停止\n重启或關閉游戲再次運行以生效！",
		#else
		"在 'system/fbneo/cheats/' 目錄下選擇金手指源文件。如果設定為 <預設值>，作弊碼將按照以下順序載入：cheat.dat -> zip中的 ini 檔案 -> 未壓縮的 ini 檔案。一旦找到作弊碼，載入將停止\n重启或關閉游戲再次運行以生效！",
		#endif
		"預設值",
		"出招表",
		"顯示此ROM的出招表等訊息(推薦使用選單主題ozone/glui)",
		"錯誤：設置金手指源文件失敗。\n請檢查'system/fbneo/cheats/%s.SetToLoad'文件權限。\n"
	}
};

/* 自定义组合键功能开始 */
char* R_button_description;
char* L_button_description;
char* R2_button_description;
char* L2_button_description;
char* R3_button_description;
char* L3_button_description;

macro_category var_fbneo_macro_categories[4];

// 初始化宏选项,目前包含4个系统
static void initial_macro_categories() {
	macro_category temp_var_fbneo_macro_categories[4] = {
		{
			// 全部的11种组合中"Buttons ACD"用不上-感谢ppx的jjsnake帮助查询整理
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
			// 全部的11种组合中"Buttons ACD"用不上-感谢ppx的jjsnake帮助查询整理
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
		/*6键的57种组合可能，用不上这么多，但是先存着吧
		{
			"streetfighter", "streetfighter_macro",
			{
				{ "fbneo-streetfighter-macro-l2", "L2", macro_desc_button_l2, macro_info_button, "Buttons ABC",
					{ { "Buttons AB", NULL }, { "Buttons AC", NULL }, { "Buttons AD", NULL }, { "Buttons AE", NULL }, { "Buttons AF", NULL }, { "Buttons BC", NULL }, { "Buttons BD", NULL }, { "Buttons BE", NULL }, { "Buttons BF", NULL }, { "Buttons CD", NULL }, { "Buttons CE", NULL }, { "Buttons CF", NULL }, { "Buttons DE", NULL }, { "Buttons DF", NULL }, { "Buttons EF", NULL },
					{ "Buttons ABC", NULL }, { "Buttons ABD", NULL }, { "Buttons ABE", NULL }, { "Buttons ABF", NULL }, { "Buttons ACD", NULL }, { "Buttons ACE", NULL }, { "Buttons ACF", NULL }, { "Buttons ADE", NULL }, { "Buttons ADF", NULL }, { "Buttons AEF", NULL }, { "Buttons BCD", NULL }, { "Buttons BCE", NULL }, { "Buttons BCF", NULL }, { "Buttons BDE", NULL }, { "Buttons BDF", NULL }, { "Buttons BEF", NULL }, { "Buttons CDE", NULL }, { "Buttons CDF", NULL }, { "Buttons CEF", NULL }, { "Buttons DEF", NULL },
					{ "Buttons ABCD", NULL }, { "Buttons ABCE", NULL }, { "Buttons ABCF", NULL }, { "Buttons ABDE", NULL }, { "Buttons ABDF", NULL }, { "Buttons ABEF", NULL }, { "Buttons ACDE", NULL }, { "Buttons ACDF", NULL }, { "Buttons ACEF", NULL }, { "Buttons ADEF", NULL }, { "Buttons BCDE", NULL }, { "Buttons BCDF", NULL }, { "Buttons BCEF", NULL }, { "Buttons BDEF", NULL }, { "Buttons CDEF", NULL },
					{ "Buttons ABCDE", NULL }, { "Buttons ABCDF", NULL }, { "Buttons ABCEF", NULL }, { "Buttons ABDEF", NULL }, { "Buttons ACDEF", NULL }, { "Buttons BCDEF", NULL }, { "Buttons ABCDEF", NULL }, { macro_disabled, NULL }, { NULL, NULL } }
				},
				{ "fbneo-streetfighter-macro-r2", "R2", macro_desc_button_r2, macro_info_button, "Buttons DEF",
					{ { "Buttons AB", NULL }, { "Buttons AC", NULL }, { "Buttons AD", NULL }, { "Buttons AE", NULL }, { "Buttons AF", NULL }, { "Buttons BC", NULL }, { "Buttons BD", NULL }, { "Buttons BE", NULL }, { "Buttons BF", NULL }, { "Buttons CD", NULL }, { "Buttons CE", NULL }, { "Buttons CF", NULL }, { "Buttons DE", NULL }, { "Buttons DF", NULL }, { "Buttons EF", NULL },
					{ "Buttons ABC", NULL }, { "Buttons ABD", NULL }, { "Buttons ABE", NULL }, { "Buttons ABF", NULL }, { "Buttons ACD", NULL }, { "Buttons ACE", NULL }, { "Buttons ACF", NULL }, { "Buttons ADE", NULL }, { "Buttons ADF", NULL }, { "Buttons AEF", NULL }, { "Buttons BCD", NULL }, { "Buttons BCE", NULL }, { "Buttons BCF", NULL }, { "Buttons BDE", NULL }, { "Buttons BDF", NULL }, { "Buttons BEF", NULL }, { "Buttons CDE", NULL }, { "Buttons CDF", NULL }, { "Buttons CEF", NULL }, { "Buttons DEF", NULL },
					{ "Buttons ABCD", NULL }, { "Buttons ABCE", NULL }, { "Buttons ABCF", NULL }, { "Buttons ABDE", NULL }, { "Buttons ABDF", NULL }, { "Buttons ABEF", NULL }, { "Buttons ACDE", NULL }, { "Buttons ACDF", NULL }, { "Buttons ACEF", NULL }, { "Buttons ADEF", NULL }, { "Buttons BCDE", NULL }, { "Buttons BCDF", NULL }, { "Buttons BCEF", NULL }, { "Buttons BDEF", NULL }, { "Buttons CDEF", NULL },
					{ "Buttons ABCDE", NULL }, { "Buttons ABCDF", NULL }, { "Buttons ABCEF", NULL }, { "Buttons ABDEF", NULL }, { "Buttons ACDEF", NULL }, { "Buttons BCDEF", NULL }, { "Buttons ABCDEF", NULL }, { macro_disabled, NULL }, { NULL, NULL } }
				}
			}
		}
		*/
		{
			// 全部的57种组合中下述13种组合大概率有效-感谢ppx的jjsnake帮助查询整理
			"streetfighter", "streetfighter_macro",
			{
				{ "fbneo-streetfighter-macro-l2", "L2", macro_desc_button_l2, macro_info_streetfighter_button, "Buttons wP|mP|sP",
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } }
				},
				{ "fbneo-streetfighter-macro-r2", "R2", macro_desc_button_r2, macro_info_streetfighter_button, "Buttons wK|mK|sK",
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } }
				},
				{ "fbneo-streetfighter-macro-l3", "L3", macro_desc_button_l3, macro_info_streetfighter_button, macro_disabled,
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } }
				},
				{ "fbneo-streetfighter-macro-r3", "R3", macro_desc_button_r3, macro_info_streetfighter_button, macro_disabled,
					{ { "Buttons wP|mP", NULL }, { "Buttons wP|sP", NULL }, { "Buttons wP|wK", NULL }, { "Buttons mP|sP", NULL }, { "Buttons mP|mK", NULL }, { "Buttons sP|wK", NULL }, { "Buttons sP|mK", NULL }, { "Buttons sP|sK", NULL }, { "Buttons wK|mK", NULL }, { "Buttons wK|sK", NULL }, { "Buttons mK|sK", NULL },{ "Buttons wP|mP|sP", NULL }, { "Buttons wK|mK|sK", NULL }, { macro_disabled, NULL }, { NULL, NULL } }
				}
			}
		}
	};
	memcpy(var_fbneo_macro_categories, temp_var_fbneo_macro_categories, sizeof(temp_var_fbneo_macro_categories));
}


// 计算 var_fbneo_macro_categories 的大小
size_t get_macro_categories_size() {
	return sizeof(var_fbneo_macro_categories) / sizeof(var_fbneo_macro_categories[0]);
}

// 得到宏按键的数量（也即是预设的L R L2 R2有多少，做成函数获得）
int get_macro_count(const char* system) {
	int count = 0;
	for (int i = 0; i < sizeof(var_fbneo_macro_categories) / sizeof(var_fbneo_macro_categories[0]); i++) {
		if (strcmp(var_fbneo_macro_categories[i].system, system) == 0) {
			count = sizeof(var_fbneo_macro_categories[i].options) / sizeof(var_fbneo_macro_categories[i].options[0]);
			break;
		}
	}
	return count;
}

// 添加核心选项
int AddMacroOptions(const char* system, int nbr_macros, int idx_var) {
	size_t macro_categories_size = get_macro_categories_size();
	for (size_t i = 0; i < macro_categories_size; i++) {
		if (strcmp(var_fbneo_macro_categories[i].system, system) == 0) {
			for (int macro_idx = 0; macro_idx < nbr_macros; macro_idx++) {
				macro_option* option = &var_fbneo_macro_categories[i].options[macro_idx];
				option_defs_us[idx_var].key = option->key;
				option_defs_us[idx_var].desc = option->option_name;
				option_defs_us[idx_var].desc_categorized = option->option_name;
				option_defs_us[idx_var].info = option->info;
				option_defs_us[idx_var].category_key = var_fbneo_macro_categories[i].category_name;
				for (int value_idx = 0; option->values[value_idx].value != NULL; value_idx++) {
					option_defs_us[idx_var].values[value_idx].value = option->values[value_idx].value;
				}
				option_defs_us[idx_var].default_value = option->default_value;
				idx_var++;
			}
		}
	}
	return idx_var;
}

// 把核心选项变量中的按键所设定的组合键宏的值提取出来
static void ProcessMacroContents(const char* system, int nbr_macros, int num_categories, CustomMacroKeys& macrodata) {
	for (int i = 0; i < num_categories; i++) {
		if (strcmp(var_fbneo_macro_categories[i].system, system) == 0) {
			for (int macro_idx = 0; macro_idx < nbr_macros; macro_idx++) {
				macro_option* option = &var_fbneo_macro_categories[i].options[macro_idx];
				struct retro_variable var = {0};
				var.key = option->key;

				if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
					CustomMacroKey macroKey;
					macroKey.system = var_fbneo_macro_categories[i].system;
					macroKey.button = option->button; // 使用 button 成员存储宏选项的描述
					macroKey.macroKey = var.value; // 使用 key 成员存储用户选择的值
					macrodata.macrocontent.push_back(macroKey);
				}
			}
		}
	}
}

// 按系统值运行ProcessMacroContents提取出对应的宏设置
CustomMacroKeys LoadCustomMacroKeys(const char* system) {
	CustomMacroKeys macrodata;

	int num_categories = sizeof(var_fbneo_macro_categories) / sizeof(var_fbneo_macro_categories[0]);

	for (int i = 0; i < num_categories; i++) {
		int nbr_macros = get_macro_count(var_fbneo_macro_categories[i].system);

		if (strcmp(var_fbneo_macro_categories[i].system, system) == 0) {
			ProcessMacroContents(system, nbr_macros, num_categories, macrodata);
		}
	}

	return macrodata;
}

// 把原先的"Buttons 1|2"或者"Buttons Att|Jump"换成Buttons AB来处理
static std::string ReWriteMacro(const std::string& input, const std::string& system) {
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
		while ((pos = result.find("Att")) != std::string::npos) {
			result.replace(pos, 3, "A");
		}
		while ((pos = result.find("Jump")) != std::string::npos) {
			result.replace(pos, 4, "B");
		}
	} else if (system == "streetfighter") {
		size_t pos;
		while ((pos = result.find("wP")) != std::string::npos) {
			result.replace(pos, 2, "A");
		}
		while ((pos = result.find("mP")) != std::string::npos) {
			result.replace(pos, 2, "B");
		}
		while ((pos = result.find("sP")) != std::string::npos) {
			result.replace(pos, 2, "C");
		}
		while ((pos = result.find("wK")) != std::string::npos) {
			result.replace(pos, 2, "D");
		}
		while ((pos = result.find("mK")) != std::string::npos) {
			result.replace(pos, 2, "E");
		}
		while ((pos = result.find("sK")) != std::string::npos) {
			result.replace(pos, 2, "F");
		}
	}
	result.erase(std::remove(result.begin(), result.end(), '|'), result.end());
	return result;
}

// 绑定宏到RA输入设备
INT32 GameInpDigital2RetroInpKey(struct GameInp* pgi, unsigned port, unsigned id, char *szn, unsigned device, unsigned nInput);
void BindCustomMacroKeys(const CustomMacroKeys& macrosdata, char* description, int nPlayer, unsigned int* nDeviceType, struct GameInp * pgi) {
	std::map<std::string, int> keyCount; //储存比如<buttons AB>这样的key的出现的当前次数

	for (int i = 0; i < macrosdata.macrocontent.size(); ++i) {
		const std::string system = macrosdata.macrocontent[i].system;
		std::string key = macrosdata.macrocontent[i].macroKey;
		const char* button = macrosdata.macrocontent[i].button.c_str();

		// 计算key的当前出现次数并添加后缀(比如Buttons AB01)
		if (keyCount.find(key) == keyCount.end()) {
			keyCount[key] = 1;
		} else {
			keyCount[key]++;
		}
		char keyWithSuffix[50];
		sprintf(keyWithSuffix, "%s%02d", key.c_str(), keyCount[key]);

		if (strcmp(keyWithSuffix, description) == 0) {
			if (strcmp("R", button) == 0) {
				// 此全局变量的创立实为无奈之举，RA手柄会一直刷新，description必须常驻，否则字符串丢失
				// 前端显示"Buttons AB01-04"毕竟不正规
				R_button_description = new char[key.size() + 1];
				strcpy(R_button_description, key.c_str());
				GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_R, R_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
			}
			if (strcmp("L", button) == 0) {
				L_button_description = new char[key.size() + 1];
				strcpy(L_button_description, key.c_str());
				GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_L, L_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
			}
			if (strcmp("R2", button) == 0) {
				R2_button_description = new char[key.size() + 1];
				strcpy(R2_button_description, key.c_str());
				GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_R2, R2_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
			}
			if (strcmp("L2", button) == 0) {
				L2_button_description = new char[key.size() + 1];
				strcpy(L2_button_description, key.c_str());
				GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_L2, L2_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
			}
			if (strcmp("R3", button) == 0) {
				R3_button_description = new char[key.size() + 1];
				strcpy(R3_button_description, key.c_str());
				GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_R3, R3_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
			}
			if (strcmp("L3", button) == 0) {
				L3_button_description = new char[key.size() + 1];
				strcpy(L3_button_description, key.c_str());
				GameInpDigital2RetroInpKey(pgi, nPlayer, RETRO_DEVICE_ID_JOYPAD_L3, L3_button_description, RETRO_DEVICE_JOYPAD, GIT_MACRO_AUTO);
			}
		}
	}
}

// 预设按键
// 重载函数-4键
void AssignButtons(const char* system, const char* szName, const char* szInfo, int nPlayer, int i, int nButtons[][4]) {
	if (strcmp(system, "neogeo") == 0) {
		if (_stricmp(" Button A", szName + 2) == 0) {
			nButtons[nPlayer][0] = i;
		}
		if (_stricmp(" Button B", szName + 2) == 0) {
			nButtons[nPlayer][1] = i;
		}
		if (_stricmp(" Button C", szName + 2) == 0) {
			nButtons[nPlayer][2] = i;
		}
		if (_stricmp(" Button D", szName + 2) == 0) {
			nButtons[nPlayer][3] = i;
		}
	} else if (strcmp(system, "pgm") == 0) {
		if (_stricmp(" Button 1", szName + 2) == 0) {
			nButtons[nPlayer][0] = i;
		}
		if (_stricmp(" Button 2", szName + 2) == 0) {
			nButtons[nPlayer][1] = i;
		}
		if (_stricmp(" Button 3", szName + 2) == 0) {
			nButtons[nPlayer][2] = i;
		}
		if (_stricmp(" Button 4", szName + 2) == 0) {
			nButtons[nPlayer][3] = i;
		}
	}
}
// 重载函数-2键
void AssignButtons(const char* system, const char* szName, const char* szInfo, int nPlayer, int i, int nButtons[][2], INT32 nCps1Tradition[]) {
	if (strcmp(system, "cps1") == 0) {
		if (_stricmp(" Attack", szName + 2) == 0) {
			nButtons[nPlayer][0] = i;
			nCps1Tradition[nPlayer] |= 1;// 此处需要位操作，来确保CPS1的游戏是存在Attack和Jump的，不符合的游戏则排除在传统游戏外
		}
		if (_stricmp(" Jump", szName + 2) == 0) {
			nButtons[nPlayer][1] = i;
			nCps1Tradition[nPlayer] |= 2;
		}
	}
}

// 批量设定全部组合键
struct GameInp* AddMacroKeys(struct GameInp* pgi, int nPlayer, int nButtonsTwo[][2], int nButtonsFour[][4], int nPunchInputs[][3], int nKickInputs[][3], const char* system, UINT32& numMacroCount) {
	struct BurnInputInfo bii;
	// 从预设结构体中获得组合键组合
	initial_macro_categories();
	macro_category* category = NULL;
	size_t num_categories = get_macro_categories_size();
	for (size_t i = 0; i < num_categories; i++) {
		if (strcmp(var_fbneo_macro_categories[i].system, system) == 0) {
			category = &var_fbneo_macro_categories[i];
			break;
		}
	}

	if (!category) {
		// 如果没有找到对应的系统，返回
		return pgi;
	}

	int num_prebindkeys = get_macro_count(system);// 实体键的数量，比如L R L2 R2则是4
	int num_combs = 0;
	while (category->options[0].values[num_combs].value != NULL && strncmp(category->options[0].values[num_combs].value, "Buttons ", 8) == 0) {
		num_combs++;
	}

	// 分配数组内存大小
	const char** macros = (const char**)malloc(num_combs * sizeof(const char*));
	int** button_combinations = (int**)malloc(num_combs * sizeof(int*));

	// 填充组合键的数组和按键值的数组
	for (int i = 0; i < num_combs; i++) {
		macros[i] = category->options[0].values[i].value;
		std::string temp_macro = ReWriteMacro(macros[i], system);// 将"Buttons 1|2"这类替换为"Buttons AB"来填充按键值的数组
		button_combinations[i] = (int*)malloc(7 * sizeof(int)); // 最大6键+1=7是为了存储 -1 终止符
		// 此处说明举例：
		// 会构建如下数组macros[]={"Buttons AB","Buttons BCD"}
		// 对应button_combinations[]={(0,1,-1),(1,2,3,-1)}
		// -1占位符用于下方循环的时候直接截断，只处理01或者123也就是AB或者BCD
		int j = 0;
		const char* button_str = temp_macro.c_str() + 8; // 移除开头的 "Buttons " 进行后面按键名的赋值
		if (strstr(button_str, "A") != NULL) button_combinations[i][j++] = 0;
		if (strstr(button_str, "B") != NULL) button_combinations[i][j++] = 1;
		if (strstr(button_str, "C") != NULL) button_combinations[i][j++] = 2;
		if (strstr(button_str, "D") != NULL) button_combinations[i][j++] = 3;
		if (strstr(button_str, "E") != NULL) button_combinations[i][j++] = 4;
		if (strstr(button_str, "F") != NULL) button_combinations[i][j++] = 5;
		button_combinations[i][j] = -1; // 终止符
	}


	for (int i = 0; i < num_combs; i++) {
		// 为了确保L R L2 R2等可以重复绑定，预设和按键数量一模一样的N个组合键
		// 此处曾做过尝试从预设的RA核心选项环境变量中动态预设组合键
		// 但是因为读取序列问题，在执行这个分配的时候，环境变量值还不能获取到，
		// 故而硬编码和按键数量一模一样的N个组合键*n个player,大约100多个组合键，
		// 实属浪费，但是未能找到其他方案，暂时如此！
		for (int k = 1; k <= num_prebindkeys; k++) { // 创建"buttons AB01"-"buttons AB0n"，也就是L和R等等的数量
			pgi->nInput = GIT_MACRO_AUTO;
			pgi->nType = BIT_DIGITAL;
			if (nPunchInputs == NULL && nKickInputs == NULL) { // 注意这里的差异，街霸类6键布局此处不需要设置nmode
				pgi->Macro.nMode = 0;
			}

			sprintf(pgi->Macro.szName, "P%i %s%02d", nPlayer + 1, macros[i], k);

			for (int j = 0; j < 7 && button_combinations[i][j] != -1; j++) { // 最大6个按键循环7次(含-1终止符)或者遇到-1占位符停止
				if (nButtonsTwo != NULL) {
					BurnDrvGetInputInfo(&bii, nButtonsTwo[nPlayer][button_combinations[i][j]]); // 传递进来2键
				} else if (nButtonsFour != NULL) {
					BurnDrvGetInputInfo(&bii, nButtonsFour[nPlayer][button_combinations[i][j]]); // 传递进来4键
				} else if (nPunchInputs != NULL && nKickInputs != NULL) {
					if (button_combinations[i][j] < 3) { // ABC时候从拳取键值
						BurnDrvGetInputInfo(&bii, nPunchInputs[nPlayer][button_combinations[i][j]]); // 传递进来6键
					} else { // DEF时候从踢取键值
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

	//释放内存
	for (int i = 0; i < num_combs; i++) {
		free(button_combinations[i]);
	}
	free(button_combinations);
	free(macros);

	return pgi;
}

/* 自定义组合键功能结束 */

/* 作弊码文件选择功能开始 */

// 移植zipfn.cpp并将函数重命名，以便可以自由的选择读取zip和7z格式，而不改动官方源库
// 全部三个函数zipopen、zipclose、zipgetlist都重命名加入SpecificType后缀
#include "burner.h"
#include "unzip.h"

#ifdef INCLUDE_7Z_SUPPORT
#include "un7z.h"
#endif

#define ZIPFN_FILETYPE_NONE		-1
#define ZIPFN_FILETYPE_ZIP		1
#define ZIPFN_FILETYPE_7ZIP		2

static INT32 nFileType = ZIPFN_FILETYPE_NONE;

static unzFile Zip = NULL;
static INT32 nCurrFile = 0; // The current file we are pointing to

#ifdef INCLUDE_7Z_SUPPORT
static _7z_file* _7ZipFile = NULL;
#endif

INT32 ZipOpenSpecificType(char* szZip) {
	nFileType = ZIPFN_FILETYPE_NONE;

	if (szZip == NULL) return 1;

	// 获取文件后缀名
	const char* ext = strrchr(szZip, '.');
	if (ext == NULL) return 1; // 无效的文件名

	char szFileName[MAX_PATH];
	sprintf(szFileName, "%s", szZip);

	if (strcmp(ext, ".zip") == 0) {
		Zip = unzOpen(szFileName);
		if (Zip != NULL) {
			nFileType = ZIPFN_FILETYPE_ZIP;
			unzGoToFirstFile(Zip);
			nCurrFile = 0;

			return 0;
		}
	}
	#ifdef INCLUDE_7Z_SUPPORT
	else if (strcmp(ext, ".7z") == 0) {
		_7z_error _7zerr = _7z_file_open(szFileName, &_7ZipFile);
		if (_7zerr == _7ZERR_NONE) {
			nFileType = ZIPFN_FILETYPE_7ZIP;
			nCurrFile = 0;

			return 0;
		}
	}
	#endif
	else {
		return 1; // 无效的文件类型
	}

	return 1;
}

INT32 ZipCloseSpecificType()
{
	if (nFileType == ZIPFN_FILETYPE_ZIP) {
		if (Zip != NULL) {
			unzClose(Zip);
			Zip = NULL;
		}
	}

#ifdef INCLUDE_7Z_SUPPORT
	if (nFileType == ZIPFN_FILETYPE_7ZIP) {
		if (_7ZipFile != NULL) {
			_7z_file_close(_7ZipFile);
			_7ZipFile = NULL;
		}
	}
#endif

	nFileType = ZIPFN_FILETYPE_NONE;

	return 0;
}

// Get the contents of a zip file into an array of ZipEntrys
INT32 ZipGetListSpecificType(struct ZipEntry** pList, INT32* pnListCount)
{
	if (nFileType == ZIPFN_FILETYPE_ZIP && Zip == NULL) return 1;
	if (pList == NULL) return 1;

#ifdef INCLUDE_7Z_SUPPORT
	if (nFileType == ZIPFN_FILETYPE_7ZIP && _7ZipFile == NULL) return 1;
#endif

	if (nFileType == ZIPFN_FILETYPE_ZIP) {
		unz_global_info ZipGlobalInfo;
		memset(&ZipGlobalInfo, 0, sizeof(ZipGlobalInfo));

		unzGetGlobalInfo(Zip, &ZipGlobalInfo);
		INT32 nListLen = ZipGlobalInfo.number_entry;

		// Make an array of File Entries
		struct ZipEntry* List = (struct ZipEntry *)malloc(nListLen * sizeof(struct ZipEntry));
		if (List == NULL) { unzClose(Zip); return 1; }
		memset(List, 0, nListLen * sizeof(struct ZipEntry));

		INT32 nRet = unzGoToFirstFile(Zip);
		if (nRet != UNZ_OK) { unzClose(Zip); free(List); List = NULL; return 1; }

		// Step through all of the files, until we get to the end
		INT32 nNextRet = 0;

		for (nCurrFile = 0, nNextRet = UNZ_OK;
			nCurrFile < nListLen && nNextRet == UNZ_OK;
			nCurrFile++, nNextRet = unzGoToNextFile(Zip))
		{
			unz_file_info FileInfo;
			memset(&FileInfo, 0, sizeof(FileInfo));

			nRet = unzGetCurrentFileInfo(Zip, &FileInfo, NULL, 0, NULL, 0, NULL, 0);
			if (nRet != UNZ_OK) continue;

			// Allocate space for the filename
			char* szName = (char *)malloc(FileInfo.size_filename + 1);
			if (szName == NULL) continue;

			nRet = unzGetCurrentFileInfo(Zip, &FileInfo, szName, FileInfo.size_filename + 1, NULL, 0, NULL, 0);
			if (nRet != UNZ_OK) continue;

			List[nCurrFile].szName = szName;
			List[nCurrFile].nLen = FileInfo.uncompressed_size;
			List[nCurrFile].nCrc = FileInfo.crc;
		}

		// return the file list
		*pList = List;
		if (pnListCount != NULL) *pnListCount = nListLen;

		unzGoToFirstFile(Zip);
		nCurrFile = 0;
	}

#ifdef INCLUDE_7Z_SUPPORT
	if (nFileType == ZIPFN_FILETYPE_7ZIP) {
		UInt16 *temp = NULL;
		size_t tempSize = 0;

		INT32 nListLen = _7ZipFile->db.NumFiles;

		// Make an array of File Entries
		struct ZipEntry* List = (struct ZipEntry *)malloc(nListLen * sizeof(struct ZipEntry));
		if (List == NULL) return 1;
		memset(List, 0, nListLen * sizeof(struct ZipEntry));

		for (UINT32 i = 0; i < _7ZipFile->db.NumFiles; i++) {
			size_t len = SzArEx_GetFileNameUtf16(&_7ZipFile->db, i, NULL);

			// if it's a directory entry we don't care about it..
			if (SzArEx_IsDir(&_7ZipFile->db, i)) continue;

			if (len > tempSize) {
				SZipFree(NULL, temp);
				tempSize = len;
				temp = (UInt16 *)SZipAlloc(NULL, tempSize * sizeof(temp[0]));
				if (temp == 0) {
					free(List);
					List = NULL;
					return 1; // memory error
				}
			}

			UINT64 size = SzArEx_GetFileSize(&_7ZipFile->db, i);
			UINT32 crc = _7ZipFile->db.CRCs.Vals[i];

			SzArEx_GetFileNameUtf16(&_7ZipFile->db, i, temp);

			// convert filename to char
			char *szFileName = NULL;
			szFileName = (char*)malloc(len * 2 * sizeof(char*));
			if (szFileName == NULL) continue;

			for (UINT32 j = 0; j < len; j++) {
				szFileName[j + 0] = temp[j] & 0xff;
				szFileName[j + 1] = temp[j] >> 8;
			}

			List[nCurrFile].szName = szFileName;
			List[nCurrFile].nLen = size;
			List[nCurrFile].nCrc = crc;

			nCurrFile++;
		}

		// return the file list
		*pList = List;
		if (pnListCount != NULL) *pnListCount = nListLen;

		nCurrFile = 0;

		SZipFree(NULL, temp);
	}
#endif

	return 0;
}

INT32 ZipLoadFileSpecificType(UINT8* Dest, INT32 nLen, INT32* pnWrote, INT32 nEntry)
{
	if (nFileType == ZIPFN_FILETYPE_ZIP && Zip == NULL) return 1;

#ifdef INCLUDE_7Z_SUPPORT
	if (nFileType == ZIPFN_FILETYPE_7ZIP && _7ZipFile == NULL) return 1;
#endif

	INT32 nRet = 0;

	if (nFileType == ZIPFN_FILETYPE_ZIP) {
		if (nEntry < nCurrFile)
		{
			// We'll have to go through the zip file again to get to our entry
			nRet = unzGoToFirstFile(Zip);
			if (nRet != UNZ_OK) return 1;
			nCurrFile = 0;
		}

		// Now step through to the file we need
		while (nCurrFile < nEntry)
		{
			nRet = unzGoToNextFile(Zip);
			if (nRet != UNZ_OK) return 1;
			nCurrFile++;
		}

		nRet = unzOpenCurrentFile(Zip);
		if (nRet != UNZ_OK) return 1;

		nRet = unzReadCurrentFile(Zip, Dest, nLen);
		// Return how many bytes were copied
		if (nRet >= 0 && pnWrote != NULL) *pnWrote = nRet;

		nRet = unzCloseCurrentFile(Zip);
		if (nRet == UNZ_CRCERROR) return 2;
		if (nRet != UNZ_OK) return 1;
	}

#ifdef INCLUDE_7Z_SUPPORT
	if (nFileType == ZIPFN_FILETYPE_7ZIP) {
		_7ZipFile->curr_file_idx = nEntry;
		UINT32 nWrote = 0;

		UINT32 crc = _7ZipFile->db.CRCs.Vals[nEntry];

		_7z_error _7zerr = _7z_file_decompress(_7ZipFile, Dest, nLen, &nWrote);
		if (_7zerr != _7ZERR_NONE) return 1;

		// Return how many bytes were copied
		if (_7zerr == _7ZERR_NONE && pnWrote != NULL) *pnWrote = (INT32)nWrote;

		// use zlib crc32 module to calc crc of decompressed data, and check against 7z header
		UINT32 nCalcCrc = crc32(0, Dest, nWrote);
		if (nCalcCrc != crc) return 2;
	}
#endif

	return 0;
}
// 移植zipfn.cpp结束

// 作弊码文件选择功能开始
#ifdef INCLUDE_7Z_SUPPORT
const char* cheatfilename[] = { "cheat.dat", "cheat.zip", "cheat.7z" }; // 包含7z支持时的文件名数组
const int cheatfilename_size = sizeof(cheatfilename) / sizeof(cheatfilename[0]);
#else
const char* cheatfilename[] = { "cheat.dat", "cheat.zip" }; // 关闭7z的平台则只能选择zip
const int cheatfilename_size = sizeof(cheatfilename) / sizeof(cheatfilename[0]);
#endif
const char* cheatfilename_check[] = { "cheat.dat", "cheat.zip", "cheat.7z" };
const int cheatfilename_check_size = sizeof(cheatfilename_check) / sizeof(cheatfilename_check[0]);

struct retro_core_option_v2_definition var_fbneo_choose_cheatfile = {
	"fbneo-choose-cheatfile",
	"Choose Cheat Source File",
	NULL,
	#ifdef INCLUDE_7Z_SUPPORT
	"Select Cheat Source File in 'system/fbneo/cheats/'.If setting to <Default>,cheats will be loaded in the order of cheat.dat -> ini file compressed in zip file -> ini file compressed in 7z file-> ini file uncompressed.The cheat loading will stop once found\nRestart or Reload to take effect!",
	#else
	"Select Cheat Source File in 'system/fbneo/cheats/'.If setting to <Default>,cheats will be loaded in the order of cheat.dat -> ini file compressed in zip file -> ini file uncompressed.The cheat loading will stop once found\nRestart or Reload to take effect!",
	#endif
	NULL,
	NULL,
	{
		{ "Default", NULL },
		{ NULL,NULL },
	},
	"Default"
};

static std::vector<std::string> foundCheatFiles;
bool CheckCheatFilesExist() { //这个函数用来确定是否有多个有效cheat文件可供选择，以便创建核心选项，并使得选项值可用
	foundCheatFiles.clear();

	for (int i = 0; i < cheatfilename_size; i++) {
		TCHAR szFilename[MAX_PATH] = _T("");
		sprintf(szFilename, "%s%s", szAppCheatsPath, cheatfilename[i]);
		FILE* file = fopen(szFilename, _T("rb"));
		if (file) {
			foundCheatFiles.push_back(cheatfilename[i]);
			fclose(file);
		}
	}

	if (foundCheatFiles.size() >= 2) {
		return true;
	}

	return false;
}

std::vector<const retro_core_option_v2_definition*> AddCheatfileChooseOption(std::vector<const retro_core_option_v2_definition*> vars_systems) {
	if (CheckCheatFilesExist()) {
		var_fbneo_choose_cheatfile.desc = RETRO_CHOOSE_CHEATFILE_DESC;
		var_fbneo_choose_cheatfile.info = RETRO_CHOOSE_CHEATFILE_INFO;

		var_fbneo_choose_cheatfile.values[0].value = RETRO_CHOOSE_CHEATFILE_DEFAULT; // 默认值

		for (int i = 0; i < foundCheatFiles.size(); i++) {
			var_fbneo_choose_cheatfile.values[i + 1].value = foundCheatFiles[i].c_str();
		}
		var_fbneo_choose_cheatfile.values[foundCheatFiles.size() + 1].value = NULL; // 选项列表结束符

		var_fbneo_choose_cheatfile.default_value = RETRO_CHOOSE_CHEATFILE_DEFAULT;

		vars_systems.push_back(&var_fbneo_choose_cheatfile);
	}
	return vars_systems;
}

//读取用户设置的作弊码源文件的值
static std::string GetChooseCheatFileValue() {
	struct retro_variable var = {0};
	var.key = var_fbneo_choose_cheatfile.key;

	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
		return std::string(var.value);
	} else {
		return "";
	}
}

void apply_ChooseCheatFile_from_variables() {
	TCHAR szFilename[MAX_PATH] = _T("");
	TCHAR szFilenameExist[MAX_PATH] = _T("");

	// 删除旧SettoLoad文件
	if (CheckCheatFilesExist()) {
		if (GetChooseCheatFileValue() != var_fbneo_choose_cheatfile.default_value && !GetChooseCheatFileValue().empty()) {
			sprintf(szFilename, "%s%s.SetToLoad", szAppCheatsPath, GetChooseCheatFileValue().c_str());
		}

		for (int i = 0; i < cheatfilename_check_size; i++) {
			sprintf(szFilenameExist, "%s%s.SetToLoad", szAppCheatsPath, cheatfilename_check[i]);
			FILE* fileExist = fopen(szFilenameExist, _T("rb"));
			if (fileExist != NULL) {
				fclose(fileExist);
				if (strcmp(szFilenameExist, szFilename) != 0) {
					// 使用RA库中的filestream_delete可以识别中文路径
					if (filestream_delete(szFilenameExist) !=0) {
						size_t length = strlen(RETRO_CHOOSE_CHEATFILE_ERR) + 1;
						TCHAR* errCopy = (TCHAR*)malloc(length * sizeof(TCHAR));
						if (errCopy) {
							strncpy(errCopy, RETRO_CHOOSE_CHEATFILE_ERR, length);
							errCopy[length - 1] = '\0';
							HandleMessage(RETRO_LOG_ERROR, errCopy, cheatfilename_check[i]);
							free(errCopy);
						}
						return;
					}
				}
			}
		}

	// 依据选项值创建新SettoLoad文件
		if (szFilename[0] != '\0') {
			FILE* file = fopen(szFilename, _T("rb"));
			if (file != NULL) {
				fclose(file);
				return;
			} else {
			file = fopen(szFilename, _T("w"));
				if (file != NULL) {
					fclose(file);
				}
			}
		}
	}
}

// 根据SettoLoad文件回调environ_cb设置成核心选项的值(用于启动游戏时候同步设置)
// 防止独立的个性化的核心选项扰乱全局设置
void SetCheatFileChooseOptionValue() {
	TCHAR szFilename[MAX_PATH] = _T("");
	bool bisFoundSetToLoad = false;

	if (CheckCheatFilesExist()) {
		for (int i = 0; i < cheatfilename_size; i++) {
			sprintf(szFilename, "%s%s.SetToLoad", szAppCheatsPath, cheatfilename[i]);
			FILE* file = fopen(szFilename, _T("rb"));
			if (file != NULL) {
				fclose(file);

				// 设置核心选项的值
				struct retro_variable var = {0};
				var.key = var_fbneo_choose_cheatfile.key;
				var.value = cheatfilename[i];
				environ_cb(RETRO_ENVIRONMENT_SET_VARIABLE, &var);
				bisFoundSetToLoad = true;
				break; // 找到一个有效的文件后退出循环
			}
		}

		// 如果没有找到有效的文件，设置为默认值
		if (!bisFoundSetToLoad) {
			struct retro_variable var = {0};
			var.key = var_fbneo_choose_cheatfile.key;
			var.value = var_fbneo_choose_cheatfile.default_value;
			environ_cb(RETRO_ENVIRONMENT_SET_VARIABLE, &var);
		}
	} else {
		for (int i = 0; i < cheatfilename_check_size; i++) {
			sprintf(szFilename, "%s%s.SetToLoad", szAppCheatsPath, cheatfilename_check[i]);
			FILE* file = fopen(szFilename, _T("rb"));
			if (file != NULL) {
				fclose(file);
				// 使用RA库中的filestream_delete可以识别中文路径
				if (filestream_delete(szFilename) !=0) {
					size_t length = strlen(RETRO_CHOOSE_CHEATFILE_ERR) + 1;
					TCHAR* errCopy = (TCHAR*)malloc(length * sizeof(TCHAR));
					if (errCopy) {
						strncpy(errCopy, RETRO_CHOOSE_CHEATFILE_ERR, length);
						errCopy[length - 1] = '\0';
						HandleMessage(RETRO_LOG_ERROR, errCopy, cheatfilename_check[i]);
						free(errCopy);
					}
				}
			}
		}
	}

}
/* 作弊码文件选择功能结束 */

/* 读取出招表文件功能开始 */

// TODO：映射表需要参考mame源码:https://github.com/mamedev/mame/blob/master/plugins/data/button_char.lua
// 强行使用unicode表达mame内置图标，无法完美，尽力折中转化
/*👊🦵⚪🛡🐉💥⚡➕➖*/
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

static std::map<std::string, std::string> symbolMap;
void InitializeSymbolMap() {
	for (int i = 0; i < sizeof(SymbolList) / sizeof(SymbolList[0]); ++i) {
		symbolMap[SymbolList[i].key] = SymbolList[i].value;
	}
}

std::string ReplaceSymbols(const std::string& input) {
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

static std::vector<std::string> CommandDataLine; // 存储多个 CommandDataLine

static std::string TrimNewLine(char* line) {

	std::string lineStr(line);
	lineStr.erase(std::remove(lineStr.begin(), lineStr.end(), '\r'), lineStr.end()); // 去除 '\r'
	lineStr.erase(std::remove(lineStr.begin(), lineStr.end(), '\n'), lineStr.end()); // 去除 '\n'

	return lineStr;
}

static bool ReadCommand_Dat() {
	TCHAR line[4096] = {0};
	TCHAR buffer[256] = {0};
	std::string LineStr;
	std::string LastLineStr;
    std::string token;
    std::string drv_name(BurnDrvGetText(DRV_NAME));
	TCHAR szFilename[MAX_PATH] = _T("");
	bool foundInfo = false; // 标记是否找到有效的$info行，即是发现该ROM的出招表条目
	FILE* cmdFile = NULL;

	sprintf(szFilename, "%scommand.dat", szAppCommandPath);
	cmdFile = fopen(szFilename, _T("rt"));
	if (cmdFile == NULL) {
		return false; // 文件打开失败
	}

	InitializeSymbolMap();

	while (_fgetts(line, sizeof(line), cmdFile) != NULL) {
		// 跳过#开头的行
		if (line[0] == '#') {
			continue;
		}

		// 检查行是否以"$info="开头
		if (!foundInfo) {
			if (strncmp(line, "$info=", 6) == 0) {
				std::string info_line(line + 6);
				std::istringstream iss(info_line);
				while (std::getline(iss, token, ',')) {
					strncpy(buffer, token.c_str(), sizeof(buffer) - 1);
					buffer[sizeof(buffer) - 1] = '\0';
					token = TrimNewLine(buffer);
					if (token == drv_name) {
						foundInfo = true; // 标记找到有效的$info行
						break;
					}
				}
				continue;
			}
		}

		// 继续查找$end和空白行
		if (foundInfo) {

			// 处理当前行
			LineStr = TrimNewLine(line);

			// 检查当前行为"$cmd",跳过
			if (strncmp(LineStr.c_str(), "$cmd", 4) == 0) {
				// 如果上一行是"$end",则需要打入空符号，相当于添加空行
				if (strncmp(LastLineStr.c_str(), "$end", 4) == 0) {
					CommandDataLine.push_back("");
				}
				LastLineStr = LineStr;
				continue;
			}
			// 检查当前行为"$end",跳过
			if (strncmp(LineStr.c_str(), "$end", 4) == 0) {
				LastLineStr = LineStr;
				continue;
			}
			// 检查当前行为空，根据情况加空行或者直接break结束提取出招表
			if (LineStr.empty()) {
				// 防止出现连续空行，浪费核心选项空间
				if (LastLineStr.empty()) {
					continue;
				}
				CommandDataLine.push_back("");
				LastLineStr = LineStr;
				continue;
			}
			// 再次遇到"$info="行,直接结束
			if (strncmp(LineStr.c_str(), "$info=", 6) == 0) {
				break;
			}
			CommandDataLine.push_back(ReplaceSymbols(LineStr));
			LastLineStr = LineStr;
		}
	}

	while (!CommandDataLine.empty() && CommandDataLine.back().empty()) {
		CommandDataLine.pop_back(); // 删除最后空元素
	}
	fclose(cmdFile);

	return foundInfo;
}

int get_command_dat_count() {
	if (CommandDataLine.size() == 0) {
		if (!ReadCommand_Dat()) {
			return 0;
		}
	}
	return CommandDataLine.size();
}

static std::vector<std::string> CommandKeys;
int AddCommandDatOptions(int command_idx_var) {
	if (CommandDataLine.size() == 0) {
		if (!ReadCommand_Dat()) {
			return command_idx_var;
		}
	}

	const int maxCommandBlocks = CommandDataLine.size();
	char key[64];

	for (int i = 0; i < maxCommandBlocks; i++) {
		snprintf(key, sizeof(key), "fbneo-commanddat-%d", i);
		CommandKeys.push_back(key);
	}

	for (int j = 0; j < maxCommandBlocks; j++) {
		option_defs_us[command_idx_var].key 				= CommandKeys[j].c_str();
		option_defs_us[command_idx_var].desc 				= " ";
		option_defs_us[command_idx_var].desc_categorized	= CommandDataLine[j].c_str();
		option_defs_us[command_idx_var].info				= NULL;
		option_defs_us[command_idx_var].category_key		= "command_dat";
		option_defs_us[command_idx_var].values[0].value		= " ";
		option_defs_us[command_idx_var].values[1].value		= NULL;
		option_defs_us[command_idx_var].default_value		= " ";
		command_idx_var++;
	}
/*  //调试用打印提取出招表信息
	TCHAR szFilename2[MAX_PATH] = _T("");
	FILE* cmdFile2 = NULL;
	sprintf(szFilename2, "%scommand.log", szAppCommandPath);
	cmdFile2 = fopen(szFilename2, _T("w"));
	if (cmdFile2 != NULL) {
		// 将 CommandDataLine 的内容写入文件
		for (int m = 0; m < CommandDataLine.size(); ++m) {
			fwrite(CommandDataLine[m].c_str(), 1, CommandDataLine[m].size(), cmdFile2);
			fwrite("\n", 1, 1, cmdFile2);
		}
		fclose(cmdFile2);
	}
*/
	return command_idx_var;
}
/* 读取出招表文件功能结束 */
#undef NUM_STRING_NONOFFICIAL