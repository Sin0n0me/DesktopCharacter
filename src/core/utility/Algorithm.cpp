#include "Algorithm.h"

// ŒvŽZ•û–@‚ÍChatGPT‚©‚ç
void jacobi_eigen_decomposition(DirectX::XMMATRIX& matrix, DirectX::XMMATRIX& eigenvectors) {
	eigenvectors = DirectX::XMMatrixIdentity();
	for(int i = 0; i < 24; ++i) {
		int p = 0;
		int q = 1;
		const float max_value = [&matrix, &p, &q]() {
			float max_value = fabsf(matrix.r[0].m128_f32[1]);
			for(int j = 0; j < 3; ++j) {
				for(int k = j + 1; k < 3; ++k) {
					const float v = fabsf(matrix.r[j].m128_f32[k]);
					if(v > max_value) {
						max_value = v;
						p = j;
						q = k;
					}
				}
			}
			return max_value;
			}();

		if(max_value < 1e-6f) {
			break;
		}

		const float app = matrix.r[p].m128_f32[p];
		const float aqq = matrix.r[q].m128_f32[q];
		const float apq = matrix.r[p].m128_f32[q];
		const float phi = 0.5f * atanf(2.0f * apq / (aqq - app));
		const float c = cosf(phi);
		const float s = sinf(phi);

		DirectX::XMMATRIX r = DirectX::XMMatrixIdentity();
		r.r[p].m128_f32[p] = c;
		r.r[q].m128_f32[q] = c;
		r.r[p].m128_f32[q] = s;
		r.r[q].m128_f32[p] = -s;

		matrix = DirectX::XMMatrixTranspose(r) * matrix * r;
		eigenvectors = eigenvectors * r;
	}
}