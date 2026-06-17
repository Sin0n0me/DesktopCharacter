#include "vmd_converter.h"
#include "../../clip_data/interpolation/interpolation.h"
#include <foundation/log/logger.h>
#include <foundation/str/to_utf8.h>

namespace enishi::animation {
    AnimationClipData FrameConverter::make_clip_data(
        const assets_system::IBoneResolver* resolver, const assets_system::VMDData& data) {
        AnimationClipData clip_data{};

        if (FrameConverter::write_bone_track(clip_data.bone_tracks, data.bone_key_frames, resolver)
                .is_err()) {
            // return;
        }

        return clip_data;
    }

    foundation::VoidResult<AnimationError> FrameConverter::write_bone_track(
        std::vector<BoneTrack>& bone_tracks,
        const std::vector<assets_system::VMDBoneKeyFrame>& bone_key_frames,
        const assets_system::IBoneResolver* resolver) {
        std::unordered_map<std::uint32_t, std::uint32_t> tmep;

        for (const auto& bone_key_frame : bone_key_frames) {
            const auto utf8 = foundation::sjis_to_utf8(bone_key_frame.bone_name);
            if (utf8.is_err()) {
                foundation::Logger::warning("UTF8に変換できない文字が含まれています");
                continue;
            }

            const auto opt_index = resolver->resolve_index(utf8.value());
            if (opt_index.is_none()) {
                foundation::Logger::warning("モデルに存在しないボーンが含まれています");
                continue;
            };
            const auto bone_index = opt_index.value();

            if (!tmep.contains(bone_index)) {
                const auto track_index = bone_tracks.size() - 1;
                tmep[bone_index] = track_index;

                // 初回追加時のみ
                if (track_index == 0) {
                    BoneTrack& bone_track = bone_tracks[0];
                    bone_track.bone_index = bone_index;
                    bone_track.positions.interpolation_type = InterpolationType::VmdBezier;
                    bone_track.rotations.interpolation_type = InterpolationType::VmdBezier;
                }
            }

            BoneTrack& bone_track = bone_tracks[tmep[bone_index]];

            // フレーム単位なので時間に変換
            float time = static_cast<float>(bone_key_frame.frame) / assets_system::VMD_FPS;
            bone_track.positions.times.emplace_back(time);
            bone_track.rotations.times.emplace_back(time);

            // アニメーションの補完用
            bone_track.scales;
            const auto bezier =
                VMDAnimationBezier::make(std::to_array(bone_key_frame.interpolation));

            const auto translate = glm::vec3(bone_key_frame.translation[0],
                bone_key_frame.translation[1],
                bone_key_frame.translation[2]);

            // VMDはxyzwの順序でデータが格納されている
            const auto rotate = glm::quat(bone_key_frame.rotation[3],
                bone_key_frame.rotation[0],
                bone_key_frame.rotation[1],
                bone_key_frame.rotation[2]);

            bone_track.positions.values.emplace_back(translate);
            bone_track.rotations.values.emplace_back(rotate);
        }

        return {};
    }
} // namespace enishi::animation