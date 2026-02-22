#include "MMDFilterCallback.h"
#include <algorithm>

void MMDFilterCallback::add_proxy(btBroadphaseProxy* const proxy) {
    this->non_filter_proxy.push_back(proxy);
}

bool MMDFilterCallback::needBroadphaseCollision(
    btBroadphaseProxy* proxy0,
    btBroadphaseProxy* proxy1
) const {
    const auto find_iter = std::find_if(
        this->non_filter_proxy.begin(),
        this->non_filter_proxy.end(),
        [proxy0, proxy1](const auto& x) {return x == proxy0 || x == proxy1; }
    );
    if(find_iter != this->non_filter_proxy.end()) {
        return true;
    }

    if((proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) == 0) {
        return false;
    }

    if((proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask) == 0) {
        return false;
    }

    return true;
}