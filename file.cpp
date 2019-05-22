
#include <string>
#include <fstream>
#include <streambuf>
 
std::ifstream t("file.txt");
std::string str((std::istreambuf_iterator<char>(t)),std::istreambuf_iterator<char>());



std::ifstream t("file.txt");
std::stringstream buffer;
buffer << t.rdbuf();
std::string contents(buffer.str());
