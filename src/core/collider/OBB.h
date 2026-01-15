#pragma once
#include <DirectXMath.h>
#include <vector>

constexpr float EXTENT_MARGIN = 1.1f;

struct OBB {
    DirectX::XMFLOAT3 center;       // ローカル空間
    DirectX::XMFLOAT3 axis[3];      // 正規化された軸（ローカル）
    DirectX::XMFLOAT3 half_extent;  // 半径

    //
    static OBB make_by_covariance_matrix(const std::vector<DirectX::XMFLOAT3>& positions);

    static OBB make(
        const std::vector<DirectX::XMFLOAT3>& positions,
        const DirectX::XMVECTOR& mean,
        const DirectX::XMMATRIX& eigen_vectors
    );
};
