#pragma once
#include <memory>

// 毎回template引数書くのが面倒なので省略用のラッパー
// Rustの型推論ってすごいね

class Maker {
public:

    template <typename T, typename... Args>
    static inline void make_shared(std::shared_ptr<T>& ptr, const Args... args) {
        auto made_ptr = std::make_shared<T>(args...);
        ptr.swap(made_ptr);
    }

    template <typename T, typename... Args>
    static inline void make_unique(std::unique_ptr<T>& ptr, const Args... args) {
        auto made_ptr = std::make_unique<T>(args...);
        ptr.swap(made_ptr);
    }
};
