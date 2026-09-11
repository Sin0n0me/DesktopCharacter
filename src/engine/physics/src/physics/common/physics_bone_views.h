#pragma once
#include <memory>
#include <platform/physics/bone/interface_physics_bone_view.h>
#include <platform/physics/bone/interface_physics_bone_view_list.h>
#include <platform/physics/rigid_body/interface_rigid_body.h>

namespace enishi::physics {
    struct PhysicsBoneViews {
        std::shared_ptr<platform::IPhysicsBoneViewList> views;
        std::shared_ptr<platform::IBoneUpdater> updater;
        std::shared_ptr<platform::IPhysicsBoneView> physics_bone_view;
    };
} // namespace enishi::physics