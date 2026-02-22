#pragma once
#include "../../../model/pmd/bone/Bone.h"
#include "../VMDFileStruct.h"
#include <map>
#include <memory>
#include <vector>

class IKSolver;
class KeyFrameTimer;
class IBoneAccessor;
class IKKeyFrameCursor;

class IKKeyFrameManager {
private:
    const std::shared_ptr<const IBoneAccessor> bone_accessor;
    const std::shared_ptr<const KeyFrameTimer> frame_manager;
    const std::shared_ptr<IKSolver> ik_solver;
    std::map<BoneIndex, std::unique_ptr<IKKeyFrameCursor>> ik_frame_map; // first: frame

public:
    explicit IKKeyFrameManager(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<KeyFrameTimer>& frame_manager,
        const std::shared_ptr<IKSolver>& ik_solver,
        const std::vector<VMDIK>& iks
    );

    void apply_ik(void);
};
