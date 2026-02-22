#pragma once

#include <BulletCollision/BroadphaseCollision/btOverlappingPairCache.h>
#include <vector>

class MMDFilterCallback : public btOverlapFilterCallback {
protected:
    std::vector<btBroadphaseProxy*> non_filter_proxy;

public:
    explicit MMDFilterCallback(void) = default;

    void add_proxy(btBroadphaseProxy* const proxy);

    bool needBroadphaseCollision(
        btBroadphaseProxy* proxy0,
        btBroadphaseProxy* proxy1
    ) const override;
};
