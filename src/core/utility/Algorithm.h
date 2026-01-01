#pragma once
#include <DirectXMath.h>

// ヤコビ法による固有ベクトル取得
void jacobi_eigen_decomposition(DirectX::XMMATRIX& matrix, DirectX::XMMATRIX& eigenVectors);
