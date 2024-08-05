#pragma once

class IManager
{
private:
    /* data */
public:
    virtual bool appendNotificationNode()=0;
    virtual ~IManager() = default;
};

