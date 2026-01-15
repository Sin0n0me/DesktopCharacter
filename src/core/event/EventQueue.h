#pragma once
#include "IEventHandler.h"
#include <memory>
#include <queue>

class EventQueue {
private:
    std::queue<std::unique_ptr<IEventHandler>> event_queue;

public:
};
