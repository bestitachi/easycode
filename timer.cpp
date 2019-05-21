/*
时间相关的操作
*/

#include <iomanip>
#include <ctime>

string tm2str(const struct tm &st, const string &format="%Y%m%d")
{
        char time_tmp[255] = "\0";

        strftime(time_tmp, sizeof(time_tmp), format.c_str(), &st);

        return string(sTimeString);
}
int str2tm(const string &str, const string &format, struct tm &st)
{
        char *p = strptime(str.c_str(), format.c_str(), &st);
        return (p != NULL) ? 0 : -1;
}
time_t str2timet(const string &str, const string &fomat){
        struct tm st;
        memset(&st,0,sizeof(st));
        char *p = strptime(str.c_str(), fomat.c_str(), &st);
        int ret=  (p != NULL) ? 0 : -1;
        if(ret == 0)
        {
            return mktime(&st);
        }
        return ret;
}
string tm2str(const time_t &t, const string &format)
{
    struct tm tt;
    localtime_r(&t, &tt);
    return tm2str(tt, format);
}


string FormateUnixtime(const string & timet)
{
    //ostringstream s;
    time_t t = stol(timet);
    //s << put_time(std::localtime(&t),"%F %T");//c++11
    return tm2str(t,"%F %T");
}

string DaysAgo(const string & timet,const string & format="%Y%m%d",int n=1)
{
    auto unix_t = str2timet(timet,format);
    unix_t -= 86400*n;
    return tm2str(unix_t,format);
}
