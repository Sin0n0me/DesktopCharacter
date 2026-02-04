#pragma once
#include "../../../model/pmd/bone/Bone.h"
#include "../VMDFileStruct.h"
#include "IKKeyFrameCursor.h"
#include <map>
#include <memory>
#include <vector>

class IKSolver;
class FrameManager;
class IBoneAccessor;

class IKKeyFrameManager {
private:
    const std::shared_ptr<const IBoneAccessor> bone_accessor;
    const std::shared_ptr<const IKSolver> ik_solver;
    const std::shared_ptr<const FrameManager> frame_manager;
    std::map<BoneIndex, std::unique_ptr<IKKeyFrameCursor>> ik_frame_map; // first: frame

public:
    explicit IKKeyFrameManager(
        const std::shared_ptr<IBoneAccessor>& bone_accessor,
        const std::shared_ptr<FrameManager>& frame_manager,
        const std::vector<VMDIK>& iks
    );

    void apply_ik(void);
};
