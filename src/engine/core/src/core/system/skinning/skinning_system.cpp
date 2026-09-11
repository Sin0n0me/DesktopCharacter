#include "skinning_system.h"
#include "bone_view_factory.h"
#include <component/animation_component.h>
#include <component/ik_component.h>
#include <component/physics_component.h>
#include <component/skinning_component.h>
#include <ik_system/ik_solver.h>

namespace enishi::core {
    SkinningSystem::SkinningSystem(ecs::Registory& registory) noexcept
        : registory(&registory) {
    }

    bool SkinningSystem::should_close(void) {
        return false;
    }

    void SkinningSystem::pre_update(void) {
    }

    void SkinningSystem::update(const types::DeltaTime& delta_time) {
        for (auto [entity, animation, model, ik, physics, skinning] :
            this->registory->view<component::AnimationComponent,
                component::ModelComponent,
                component::IKComponent,
                component::PhysicsComponent,
                component::SkinningComponent>()) {
            auto& bones = this->get_or_build(entity, animation, model, ik, physics);

            bones.animation_updater->update_global_form_roots();

            this->solve_ik(bones, ik);

            (void)bones.physics_updater;

            this->write_skinning_matrices(animation, model, skinning);
        }
    }

    void SkinningSystem::post_update(void) {
    }

    void SkinningSystem::render(void) const {
    }

    ModelBones& SkinningSystem::get_or_build(const types::HandleId entity,
        component::AnimationComponent& animation,
        const component::ModelComponent& model,
        component::IKComponent& ik,
        component::PhysicsComponent& physics) noexcept {
        const auto iter = this->model_bones.find(entity);
        if (iter != this->model_bones.end()) {
            return *iter->second;
        }

        auto bones = std::make_unique<ModelBones>();
        const auto bone_count = model.bone_node.size();

        bones->animation_cache = std::make_unique<skinning_system::AnimationBonesCache>(
            model.bone_node, BoneViewFactory::make_animation_view(animation));
        bones->physics_cache = std::make_unique<skinning_system::PhysicsBonesCache>(
            model.bone_node, BoneViewFactory::make_physics_view(physics));
        bones->ik_cache = std::make_unique<skinning_system::IKBoneCache>(
            model.bone_node, BoneViewFactory::make_ik_view(ik));
        bones->bind_cache = std::make_unique<skinning_system::BindBonesCache>(
            BoneViewFactory::make_bind_view(model));

        auto& ref = *bones;
        this->model_bones.emplace(entity, std::move(bones));
        return ref;
    }

    void SkinningSystem::solve_ik(
        ModelBones& bones, const component::IKComponent& ik) const noexcept {
        for (const auto& [bone_index, ik_index] : ik.ik_map) {
            ik::IKSolver::apply_ik(
                ik.iks[ik_index], bones.ik_cache.get(), bones.ik_updater.get(), bone_index);
        }
    }

    void SkinningSystem::write_skinning_matrices(const component::AnimationComponent& animation,
        const component::ModelComponent& model,
        component::SkinningComponent& skinning) const noexcept {
        const auto bone_count = model.bone_node.size();
        if (skinning.skinning_matrices.size() != bone_count) {
            skinning.skinning_matrices.resize(bone_count);
        }

        for (std::size_t i = 0; i < bone_count; ++i) {
            skinning.skinning_matrices[i] = animation.global[i] * model.bind_bone[i].global_inverse;
        }
    }
} // namespace enishi::core