#pragma once
#include <queue>
#include <memory>
#include "IEventHandler.h"

class EventQueue {
private:
	std::queue<std::unique_ptr<IEventHandler>> event_queue;

public:
};
