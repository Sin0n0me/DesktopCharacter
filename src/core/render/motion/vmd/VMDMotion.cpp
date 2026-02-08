#include "../../../physics/mmd/MMDPhysics.h"
#include "../../../timer/DeltaTime.h"
#include "../../model/pmd/bone/IBoneAccessor.h"
#include "../../model/pmd/ik/IKSolver.h"
#include "../../model/pmd/morph/IMorphAccessor.h"
#include "../../motion/vmd/ik_key_frame/IKKeyFrameCursor.h"
#include "bone_key_frame/BoneKeyFrameManager.h"
#include "ik_key_frame/IKKeyFrameManager.h"
#include "key_frame/KeyFrameTimer.h"
#include "morphkey_frame/MorphKeyFrameManager.h"
#include "VMDLoader.h"
#include "VMDMotion.h"
#include <algorithm>

constexpr float VMD_FPS = 30.0f;

VMDMotion::VMDMotion(
    const std::shared_ptr<IBoneAccessor>& bone_accessor,
    const std::shared_ptr<IMorphAccessor>& morph_accessor,
    const std::shared_ptr<IKSolver>& ik_solver,
    const std::shared_ptr<MMDPhysics>& physics
) :
    bone_accessor(bone_accessor),
    morph_accessor(morph_accessor),
    frame_manager(
        new KeyFrameTimer(
            VMD_FPS,
            0xFFFFFFFF
        )
    ),
    ik_solver(ik_solver),
    physics(physics) {
}

bool VMDMotion::init_motion(void) {
    return true;
}

bool VMDMotion::load_motion_file(const std::filesystem::path& path) {
    VMDLoader loader = VMDLoader(path);
    if(!loader.load_vmd()) {
        return false;
    }

    this->bone_key_frame_manager = std::make_unique<BoneKeyFrameManager>(
        this->bone_accessor,
        this->frame_manager,
        loader.get_bone_key_frames()
    );

    this->morph_key_frame_manager = std::make_unique<MorphKeyFrameManager>(
        this->morph_accessor,
        this->frame_manager,
        loader.get_morph_key_frames()
    );

    this->ik = std::make_unique<IKKeyFrameManager>(
        this->bone_accessor,
        this->frame_manager,
        this->ik_solver,
        loader.get_iks()
    );

    // 最終フレームを求める
    const std::vector<KeyFrame> last_frames = {
        this->bone_key_frame_manager->get_last_frame(),
        this->morph_key_frame_manager->get_last_frame(),
    };
    this->frame_manager->set_max_frame(
        *std::max_element(last_frames.begin(), last_frames.end())
    );

    return true;
}

void VMDMotion::update_motion(const DeltaTime& delta_time) {
    // フレームの更新
    this->frame_manager->update(delta_time);

    // モーフの適用
    this->morph_key_frame_manager->apply_morph();

    // ローカル行列作成
    this->bone_key_frame_manager->update_local_matricies();

    // グローバル行列作成
    this->bone_key_frame_manager->update_global_matricies();

    // IK
    this->ik->apply_ik();

    // スキニング用の定数バッファ結果を格納
    this->bone_key_frame_manager->apply_skinning();

    // 物理演算適用
    this->physics->update(delta_time);
}