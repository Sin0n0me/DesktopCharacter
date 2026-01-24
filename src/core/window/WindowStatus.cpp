#include "WindowStatus.h"

POINT WindowStatus::to_client_position(const POINT& pt) const {
    return POINT{
        .x = pt.x - this->screen_position.x,
        .y = pt.y - this->screen_position.y,
    };
}

POINT WindowStatus::to_screen_position(const POINT& pt) const {
    return POINT{
        .x = pt.x + this->screen_position.x,
        .y = pt.y + this->screen_position.y,
    };
}