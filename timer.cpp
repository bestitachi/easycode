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
