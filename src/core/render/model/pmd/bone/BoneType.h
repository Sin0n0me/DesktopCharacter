#pragma once

// 0:回転  1:回転と移動  2:IK  3:不明  4:IK影響下  5:回転影響下(NumberIK ボーン と同じ回転をする  左右の目の向きの一致)
// 6:IK接続先  7:非表示  8:捻り  9:回転追従(付与)
enum class BoneType {
    Rotate = 0,
    RotateAndTranslate = 1,
    IK = 2,
};
