#pragma once
#include <platform/animation/interface_animation_bone_view.h>

namespace enishi::skinning_system {
    /**
     * この型の目的
     *
     * platform::IAnimationBoneViewの非所有(non-owning)な実装
     * 値そのものは持たず、外部(ECS等)が所有する各データへのポインタを保持するだけの薄いラッパー
     *
     * componentやecsを一切知らないため、このクラス自体はどんなデータソースにも紐付けられる
     * 実データへの結び付けは、component/ecsを知っている呼び出し側(例: model_control_system)が
     * コンストラクタに参照を渡すことで行う
     *
     * 前提条件: このビューが指す先の変数(vectorの要素など)は、
     * ビューの生存期間中にアドレスが変わらない(resize等でreallocされない)こと
     */
    class AnimationBoneView final : public platform::IAnimationBoneView {
      private:
        glm::vec3* const translation;
        glm::quat* const rotation;
        glm::vec3* const scale;
        glm::mat4* const global; // UpdateGlobal等で書き込まれた計算済みのグローバル行列

      public:
        AnimationBoneView(glm::vec3& translation,
            glm::quat& rotation,
            glm::vec3& scale,
            glm::mat4& global) noexcept
            : translation(&translation)
            , rotation(&rotation)
            , scale(&scale)
            , global(&global) {
        }

        ~AnimationBoneView(void) noexcept override = default;

        AnimationBoneView(const AnimationBoneView&) = delete;
        AnimationBoneView& operator=(const AnimationBoneView&) = delete;
        AnimationBoneView(AnimationBoneView&&) = delete;
        AnimationBoneView& operator=(AnimationBoneView&&) = delete;

        void set_animation_translation(glm::vec3&& translation) noexcept override;
        void set_animation_translation(const glm::mat4& translation) noexcept override;
        void set_animation_rotation(glm::quat&& rotation) noexcept override;
        void set_animation_rotation(const glm::quat& rotation) noexcept override;
        void set_animation_scale(glm::vec3&& scale) noexcept override;
        void set_animation_scale(const glm::vec3& scale) noexcept override;

        glm::vec3& get_animation_translation(void) noexcept override;
        const glm::vec3& get_animation_translation(void) const noexcept override;
        glm::quat& get_animation_rotation(void) noexcept override;
        const glm::quat& get_animation_rotation(void) const noexcept override;
        glm::vec3& get_animation_scale(void) noexcept override;
        const glm::vec3& get_animation_scale(void) const noexcept override;

        glm::mat4 get_animation_local_transform(void) const noexcept override;
        glm::mat4 get_animation_global_transform(void) const noexcept override;
        void set_animation_global_transform(const glm::mat4& mat) noexcept override;
        void set_animation_global_transform(glm::mat4&& mat) noexcept override;
    };
} // namespace enishi::skinning_system
