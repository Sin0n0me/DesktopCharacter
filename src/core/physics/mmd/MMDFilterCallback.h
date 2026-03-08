#pragma once

#include <BulletCollision/BroadphaseCollision/btOverlappingPairCache.h>
#include <unordered_set>

class MMDFilterCallback : public btOverlapFilterCallback {
protected:
    std::unordered_set<btBroadphaseProxy*> always_collide_proxies;

public:
    explicit MMDFilterCallback(void) = default;

    void add_proxy(btBroadphaseProxy* const proxy);

    bool needBroadphaseCollision(
        btBroadphaseProxy* proxy0,
        btBroadphaseProxy* proxy1
    ) const override;
};
