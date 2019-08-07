#pragma once
#include <unordered_map>
#include <stack>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <mutex> 
#include <vector>
#include <map>


class HciManager{
public:
    std::map<int,std::string> listConnections();


private:
    std::vector<std::string> m_devices;

};