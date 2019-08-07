#include "helper.h"
#pragma once

enum DeviceState{
    NIL = 0,
    WEAKSIG = 1,
    WITHINRANGE = 2
};

class AgentInterface
{
public:
    AgentInterface(): 
        m_fileDescriptor(0),
        m_state(DeviceState::NIL)
    {



    };
    virtual ~AgentInterface(){};
    virtual DeviceState getAgentState() = 0;
    virtual void init(int) = 0;
    
    virtual void SetAgentHandler(std::map<DeviceState,std::function<void()>> handlers )
    {
        m_Handlers = handlers;
    };
    
    virtual void updateState(const DeviceState& state)
    {
        if( m_state != state )
        {
            m_state = state;
            EventHandler();
        }
    };


private:
    virtual void EventHandler()
    {
        switch(m_state){
            case DeviceState::NIL:
            {
                auto iter = m_Handlers.find(DeviceState::NIL);
                if( iter != m_Handlers.end())
                {
                    iter->second();
                }
                break;
            }
            case DeviceState::WEAKSIG:
            {
                auto iter = m_Handlers.find(DeviceState::WEAKSIG);
                if( iter != m_Handlers.end())
                {
                    iter->second();
                }
                break;
            }
            case DeviceState::WITHINRANGE:
            {
                auto iter = m_Handlers.find(DeviceState::WITHINRANGE);
                if( iter != m_Handlers.end())
                {
                    iter->second();
                }
                break;
            }
        }
    }

    std::map<DeviceState,std::function<void()>> m_Handlers;
    DeviceState m_state;
    int m_fileDescriptor;
};