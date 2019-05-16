/*ip 的相关操作*/

string HostIPToString(long host_ip)
{
    struct in_addr  addr;
    addr.s_addr = htonl(host_ip);
    
    char ip_char[20]={0};
    inet_ntop(AF_INET,(const void *)&addr,ip_char,20);
    return string(ip_char);
}
