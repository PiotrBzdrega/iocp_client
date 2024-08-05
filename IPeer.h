#pragma once

#include "IManager.h"

class IPeer
{
private:
    IManager* _manager;
public:
    IPeer(/* args */);
    virtual ~IPeer() = default;
};

