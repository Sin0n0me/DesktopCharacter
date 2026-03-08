#pragma once
class IConfigChangeListener {
public:

    virtual void on_changed() = 0;
};
