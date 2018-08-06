#include <string>
#include <vector>

extern bool endsWith(const std::string &text,const std::string &suffix);
extern bool startsWith(const std::string &text,const std::string &prefix);
extern bool startsWith(const std::string &text,char);
extern bool contains(const std::string &text,const std::string &contents);
extern std::string withoutRight(const std::string &text,size_t n);
extern std::vector<std::string> split(const std::string &text);
