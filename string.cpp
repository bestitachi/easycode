/*字符串的相关操作*/
#include <string>
#include <vector>

//字符串的分割，返回一个vector
vector<string> Sepstr(const string & str,const string & sep){
    vector<string> res;
    if(str.empty()){
        return res;
    }
    if(sep.empty())
    {
        res.push_back(str);
        return res;
    }
    int start = 0;
    auto pos = str.find(sep,start);
    while(pos != string::npos)
    {
        res.push_back(str.substr(start,pos-start));
        start = pos + sep.size();
        pos = str.find(sep,start);
    }
    res.push_back(str.substr(start));
    return res;
}
