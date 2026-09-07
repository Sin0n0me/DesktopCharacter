#pragma once
#include <platform/physics/bone/interface_physics_bone_view.h>

namespace enishi::skinning_system {
    /**
     * この型の目的
     *
     * platform::IPhysicsBoneViewの非所有(non-owning)な実装
     * AnimationBoneViewと同じく、外部が所有するlocal/global行列へのポインタを保持するだけ
     *
     * 物理演算専用のバッファは別に持たず、アニメーション側と同じlocal/globalを指す想定
     * (AnimationComponent::AnimationCommand::WriteBackPhysicsSimulateが
     *  「現在のglobalを物理エンジンに書き戻す」という意味を持っていることに合わせている)
     */
    class PhysicsBoneView final : public platform::IPhysicsBoneView {
      private:
        glm::mat4* const local;
        glm::mat4* const global;

      public:
        PhysicsBoneView(glm::mat4& local, glm::mat4& global) noexcept
            : local(&local)
            , global(&global) {
        }

        ~PhysicsBoneView(void) noexcept override = default;

        PhysicsBoneView(const PhysicsBoneView&) = delete;
        PhysicsBoneView& operator=(const PhysicsBoneView&) = delete;
        PhysicsBoneView(PhysicsBoneView&&) = delete;
        PhysicsBoneView& operator=(PhysicsBoneView&&) = delete;

        void set_physics_local(glm::mat4&& local) noexcept override;
        void set_physics_local(const glm::mat4& local) noexcept override;
        void set_physics_global(glm::mat4&& global) noexcept override;
        void set_physics_global(const glm::mat4& global) noexcept override;

        [[nodiscard]] glm::mat4& get_physics_local(void) noexcept override;
        [[nodiscard]] const glm::mat4& get_physics_local(void) const noexcept override;
        [[nodiscard]] glm::mat4& get_physics_global(void) noexcept override;
        [[nodiscard]] const glm::mat4& get_physics_global(void) const noexcept override;
    };
} // namespace enishi::skinning_system
