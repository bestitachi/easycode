/*
时间相关的操作
*/

#include <iomanip>
#include <ctime>
string FormateUnixtime(const string & timet)
{
    //ostringstream s;
    time_t t = stol(timet);
    //s << put_time(std::localtime(&t),"%F %T");
    char mbstr[30];
    strftime(mbstr,sizeof(mbstr),"%F %T",localtime(&t));
    return string(mbstr);
}
