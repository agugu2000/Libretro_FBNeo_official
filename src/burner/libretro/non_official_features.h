#ifndef NON_OFFICIAL_FEATURES_H
#define NON_OFFICIAL_FEATURES_H

#include "retro_common.h"
#include <vector>
#include <string>

/* 字符串资源相关*/
#define NUM_STRING_NONOFFICIAL	22
/* 非官方增加部分*/
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
#define pgm_macro_desc						pSelLangStrNonOfficial[ 10]
#define cps1_macro_desc						pSelLangStrNonOfficial[ 11]
#define streetfighter_macro_desc			pSelLangStrNonOfficial[ 12]
#define macro_info_general					pSelLangStrNonOfficial[ 13]
#define RETRO_IGNORE_CRC_DESC				pSelLangStrNonOfficial[ 14]
#define RETRO_IGNORE_CRC_INFO				pSelLangStrNonOfficial[ 15]
#define RETRO_CHOOSE_CHEATFILE_DESC			pSelLangStrNonOfficial[ 16]
#define RETRO_CHOOSE_CHEATFILE_INFO			pSelLangStrNonOfficial[ 17]
#define RETRO_CHOOSE_CHEATFILE_DEFAULT		pSelLangStrNonOfficial[ 18]
#define RETRO_COMMAND_DAT_CAT_DESC		    pSelLangStrNonOfficial[ 19]
#define RETRO_COMMAND_DAT_CAT_INFO		    pSelLangStrNonOfficial[ 20]
#define RETRO_CHOOSE_CHEATFILE_ERR		    pSelLangStrNonOfficial[ 21]

/* 添加组合键选项所需的变量等*/
void initial_macro_categories();
// 定义 macro_option_value 结构体
struct macro_option_value {
	const char* value;
	const char* label;
};

// 定义 macro_option 结构体
struct macro_option {
	const char* key;
	const char* button;
	const char* option_name;
	const char* info;
	const char* default_value;
	macro_option_value values[20]; // 目前最大可能出現15個按鍵組合，加上"取消"和"NULL"
};

// 定义 macro_category 结构体
struct macro_category {
	const char* system;
	const char* category_name;
	macro_option options[4]; // 每个类别下预设有4个键位对应的宏选项
};

// 自定义宏键结构体
struct CustomMacroKey {
	std::string system;
	std::string button;
	std::string macroKey;
};

// 自定义宏键集合结构体
struct CustomMacroKeys {
	std::vector<CustomMacroKey> macrocontent;
};

// 函数声明
size_t get_macro_categories_size();
int get_macro_count(const char* system);
CustomMacroKeys LoadCustomMacroKeys(const char* system);
int AddMacroOptions(const char* system, int nbr_macros, int idx_var);
void BindCustomMacroKeys(const CustomMacroKeys& macrosdata, char* description, int nPlayer, unsigned int* nDeviceType, struct GameInp * pgi);

// 外部变量声明
//extern CustomMacroKeys macrodata;
extern macro_category var_fbneo_macro_categories[];

struct GameInp* AddMacroKeys(struct GameInp* pgi, int nPlayer, int nButtonsTwo[][2], int nButtonsFour[][4], int nPunchInputs[][3], int nKickInputs[][3], const char* system, UINT32& numMacroCount);
void AssignButtons(const char* system, const char* szName, const char* szInfo, int nPlayer, int i, int nButtons[][4]);
void AssignButtons(const char* system, const char* szName, const char* szInfo, int nPlayer, int i, int nButtons[][2], INT32 nCps1Tradition[]);

/* 添加选择作弊文件的选项所需的变量等*/
// 移植zipfn.cpp并将函数重命名，以便可以自由的选择读取zip和7z格式，而不改动官方源库
INT32 ZipOpenSpecificType(char* szZip);
INT32 ZipCloseSpecificType();
INT32 ZipGetListSpecificType(struct ZipEntry** pList, INT32* pnListCount);
INT32 ZipLoadFileSpecificType(UINT8* Dest, INT32 nLen, INT32* pnWrote, INT32 nEntry);

#ifdef INCLUDE_7Z_SUPPORT
extern const char* cheatfilename[];
extern const int cheatfilename_size;
#else
extern const char* cheatfilename[];
extern const int cheatfilename_size;
#endif
bool CheckCheatFilesExist();
void apply_ChooseCheatFile_from_variables();
void SetCheatFileChooseOptionValue();
std::vector<const retro_core_option_v2_definition*> AddCheatfileChooseOption(std::vector<const retro_core_option_v2_definition*> vars_systems);

/* 添加读取出招表文件的选项所需的变量等*/
struct SymbolMapping {
	const char* key;
	const char* value;
};
int get_command_dat_count();
int AddCommandDatOptions(int command_idx_var);

#endif // NON_OFFICIAL_FEATURES_H