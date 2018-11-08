#include "agent.h"

blueAgent::blueAgent
{
    m_bContinue = true;
}

void blueAgent::addDevice(int fd, devInfo dev)
{
    if (fd <= 0 && dev == nullptr)
    {
        //error handling
    }
    else
    {
        devNew.push(fd);
        devList.insert(fd, dev);
    }
}

void blueAgent::removeDevice(int fd)
{
    if (fd <= 0)
    {
        //error handling
    }
    else
    {
        devList.erase(fd);
    }
}

void blueAgent::run()
{
    while(m_bContinue)
    {

        while( devNew.size()!= 0 )
        {
            //add dev to listen queue
            int fd = devNew.top();
            //bluetooth address of the device
            string devAddr = devList[fd];
            
            
            //remove
            devNew.pop();
        }
        
        
    }
}

void sblueAgent::stop()
{
    m_bContinue = false;
}