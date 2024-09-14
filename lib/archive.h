#include <bitset>
#include <vector>

struct File_Header{
    std::string file_name;
    int file_name_size = 0;
    int file_size = 0;
};
const uint8_t kReNewSize = 12;
const uint8_t ByteSize = 8;
struct Archive{
    void IncludeFile(const std::string& file_name);
    void GetFile(const std::string& file_name);
    void DeleteFile(const std::string& file_name);
    void Show_List();
    std::vector <File_Header> files;
    std::string archive_path;
    void Make_One_Archive(const Archive& file_name1,const Archive& file_name2 );
};

std::bitset<ByteSize> DeCoderHam(std::bitset<kReNewSize>& bits);
std::bitset<kReNewSize>  CoderHam(char digit);
std::bitset<ByteSize> Eight_Bites(char byte);
std::bitset<ByteSize> Eight_Bites(int byte);