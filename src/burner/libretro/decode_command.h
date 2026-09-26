#ifndef DECODE_COMMAND_H
#define DECODE_COMMAND_H

#include <string>
#include <vector>

namespace CommandDat {

struct Record {
    std::vector<std::string> sets;
    std::string              data;
};

struct Result {
    std::vector<Record> records;
};

bool Load(const char* command_dat_path, const char* rom_name);
void Unload(void);
const Result& Get(void);
bool IsLoaded(void);

} // namespace CommandDat

#endif // DECODE_COMMAND_H