
#include <string>
#include <fstream>
#include <streambuf>

//读取文件中的全部内容到内存,c++11
string ReadAllFromFile(const string & filename)
{
    ifstream f(filename,ifstream::in|ifstream::binary);
    return string((istreambuf_iterator<char>(f)),istreambuf_iterator<char>());
}

//读取文件中的全部的内容.
string ReadAllFromFile(const string & filename){
    ifstream f(filename);
    ostringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

#include "zlib.h"
//将文件进行压缩.使用了zlib库,如果只是进行字符串的压缩,那么可以使用,zlib库中的compress,和uncompress
int CompressToFile(const string &content,const string& outfilename){
    auto file = gzopen(outfilename.c_str(),"wb");
    if(file == nullptr)
    {
        return -1;
    }
    gzwrite(file,(const voidp )content.c_str(),content.size());
    gzclose(file);
    return 0;
}

//从windows的文件路径中获取文件名.
string RemainFileName(const string &filepath){
    if(filepath.length() <= 0)
    {
        return "";
    }

    string::size_type pos = filepath.rfind('\\');
    if(pos == string::npos)
    {
        return filepath;
    }
    return filepath.substr(pos + 1);
}
