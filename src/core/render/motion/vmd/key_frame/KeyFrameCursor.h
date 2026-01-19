#pragma once

#include "FrameManager.h"
#include "IKeyframeLifecycleListener.h"
#include <functional>
#include <memory>
#include <optional>
#include <vector>

template <typename T>
class KeyFrameCursor : public IKeyframeLifecycleListener {
protected:
    const std::vector<T> key_frame_list;
    const std::shared_ptr<FrameManager> frame_manager;

private:
    const std::function<KeyFrame(const T&)> get_key_frame; // データが多いとvtable分余分にデータが増えるので継承の代わり
    KeyFrame key_frame_index;

protected:
    static std::vector<T> sort(
        std::vector<T>&& key_frame_list
    );

    void on_update_key_frame(void) override;

public:
    explicit KeyFrameCursor(
        const std::shared_ptr<FrameManager>& frame_manager,
        const std::vector<T>& key_frame_list,
        const std::function<KeyFrame(const T&)> function
    );

    virtual ~KeyFrameCursor(void);

    std::optional<T> get_previous_key_frame(void) const;
    std::optional<T> get_next_key_frame(void) const;
    std::optional<T> get_current_key_frame(void) const;
    std::optional<T> get_first_key_frame(void) const;
    std::optional<T> get_last_key_frame(void) const;
};

template<typename T>
inline std::vector<T> KeyFrameCursor<T>::sort(
    std::vector<T>&& key_frame_list
) {
    // これなんとかしたい(constフィールド対策でこうなっている)
    std::list<T> list(key_frame_list.begin(), key_frame_list.end());
    list.sort();
    return std::vector<T>(list.begin(), list.end());
}

template<typename T>
inline KeyFrameCursor<T>::KeyFrameCursor(
    const std::shared_ptr<FrameManager>& frame_manager,
    const std::vector<T>& key_frame_list,
    const std::function<KeyFrame(const T&)> function
) :
    key_frame_list(key_frame_list),
    frame_manager(frame_manager),
    get_key_frame(function),
    key_frame_index(0) {
    this->frame_manager->join(this);
}

template<typename T>
inline KeyFrameCursor<T>::~KeyFrameCursor(void) {
    this->frame_manager->leave(this);
}

template<typename T>
inline void KeyFrameCursor<T>::on_update_key_frame(void) {
    if(this->key_frame_list.empty()) {
        return;
    };

    const auto& current_frame = this->frame_manager->get_current_frame();
    const auto& current_key_frame = this->key_frame_list.at(this->key_frame_index);
    const auto key_frame = this->get_key_frame(current_key_frame);
    if(key_frame == current_frame) {
        return;
    }

    // 次のフレームへ
    if(key_frame < current_frame) {
        if(this->get_next_key_frame().has_value()) {
            this->key_frame_index += 1;
        }
        return;
    }

    // 指定したキーフレームになるまで巻き戻す
    for(;;) {
        const auto previous = this->get_previous_key_frame();
        if(!previous.has_value()) {
            return;
        }

        if(current_frame < this->get_key_frame(previous.value())) {
            this->key_frame_index -= 1;
        } else {
            return;
        }
    }
}

template<typename T>
inline std::optional<T> KeyFrameCursor<T>::get_previous_key_frame(void) const {
    if(this->key_frame_list.empty()) {
        return std::nullopt;
    }

    if(this->key_frame_index == 0) {
        return std::nullopt;
    }

    return std::optional<T>{
        this->key_frame_list.at(
            static_cast<size_t>(this->key_frame_index - 1)
        )
    };
}

template<typename T>
inline std::optional<T> KeyFrameCursor<T>::get_next_key_frame(void) const {
    if(this->key_frame_list.empty()) {
        return std::nullopt;
    }

    // 末端と同じ場合は次はない扱い
    const auto& opt_current = this->get_current_key_frame();
    const auto& opt_last = this->get_last_key_frame();
    if(!opt_current.has_value() || !opt_last.has_value()) {
        return std::nullopt;
    }
    const auto& current = opt_current.value();
    const auto& last = opt_last.value();
    if(this->get_key_frame(current) == this->get_key_frame(last)) {
        return std::nullopt;
    }

    return std::optional<T>{
        this->key_frame_list.at(
            static_cast<size_t>(this->key_frame_index + 1)
        )
    };
}

template<typename T>
inline std::optional<T> KeyFrameCursor<T>::get_current_key_frame(void) const {
    if(this->key_frame_list.empty()) {
        return std::nullopt;
    }
    return std::optional<T>{
        this->key_frame_list.at(
            static_cast<size_t>(this->key_frame_index)
        )
    };
}

template<typename T>
inline std::optional<T> KeyFrameCursor<T>::get_first_key_frame(void) const {
    if(this->key_frame_list.empty()) {
        return std::nullopt;
    }
    return std::optional<T>{
        this->key_frame_list.at(0)
    };
}

template<typename T>
inline std::optional<T> KeyFrameCursor<T>::get_last_key_frame(void) const {
    if(this->key_frame_list.empty()) {
        return std::nullopt;
    }
    return std::optional<T>{
        this->key_frame_list.at(this->key_frame_list.size() - 1)
    };
}
