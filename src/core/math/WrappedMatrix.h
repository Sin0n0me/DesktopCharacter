#pragma once
#include "Angle.h"
#include <array>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

#ifdef _WIN32
#include <DirectXMath.h>

using Matrix4x4 = DirectX::XMMATRIX;
using Vector4 = DirectX::XMVECTOR;

#else
#define USE_GLM
#include <glm.hpp>

using Matrix4x4 = glm::mat4;
using Vector4 = glm::vec4;

#endif // _WIN32

template <bool, bool>
class WrappedMatrix;

namespace {
    template <typename T>
    struct CheckWrappedMatrix : std::false_type {};

    template <bool InputHand, bool InputMajor>
    struct CheckWrappedMatrix<WrappedMatrix<InputHand, InputMajor>> : std::true_type {};

    template <typename T>
    concept IsWrappedMatrix = CheckWrappedMatrix<T>::value;
}

// 右手系, 左手系, 行優先, 列優先が混じり分からなくなって辛かったので行列のラッパークラス作成
template <bool IsLeftHand = true, bool IsRowMajor = true>
class WrappedMatrix {
private:
    template <bool, bool>
    friend class WrappedMatrix;

public:
    using WrappedDirectXMatrix = WrappedMatrix<true, true>;  // 左手系 行優先
    using WrappedOpenGLMatrix = WrappedMatrix<false, false>; // 右手系 列優先
    using WrappedBulletMatrix = WrappedMatrix<false, false>; // 右手系 列優先
    using WrappedMMDMatrix = WrappedMatrix<true, true>;      // 左手系 行優先

public:
    static constexpr bool IS_LEFT_HAND = IsLeftHand;
    static constexpr bool IS_ROW_MAJOR = IsRowMajor;

public:
    //static constexpr WrappedMatrix Zero = WrappedMatrix();
    //static constexpr WrappedMatrix Identity = WrappedMatrix();

private:
    Matrix4x4 matrix;

public:
    constexpr WrappedMatrix(void) noexcept :
        matrix(
#ifdef USE_GLM
            // TODO:
            static_assert(false);
#else
            Matrix4x4(
                DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)
            )
#endif // USE_GLM
        ) {
    }
    constexpr WrappedMatrix(const WrappedMatrix& matrix) :
        matrix(matrix.matrix) {
    }
    constexpr explicit WrappedMatrix(const Matrix4x4& matrix) noexcept : matrix(matrix) {}
    constexpr explicit WrappedMatrix(
        const float m11, const float m12, const float m13, const float m14,
        const float m21, const float m22, const float m23, const float m24,
        const float m31, const float m32, const float m33, const float m34,
        const float m41, const float m42, const float m43, const float m44
    ) noexcept :
        matrix(
            m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44
        ) {
    }

    constexpr explicit WrappedMatrix(const std::array<float, 16>& array) :
        matrix(
#ifdef USE_GLM
            // TODO:
            static_assert(false);
#else
            IsRowMajor ?
            Matrix4x4(
                array[0], array[1], array[2], array[3],
                array[4], array[5], array[6], array[7],
                array[8], array[9], array[10], array[11],
                array[12], array[13], array[14], array[15]
            ) :
            Matrix4x4(
                array[0], array[4], array[8], array[12],
                array[1], array[5], array[9], array[13],
                array[2], array[6], array[10], array[14],
                array[3], array[7], array[11], array[15]
            )
#endif // USE_GLM
        ) {
    }
    template <bool InputHand, bool InputMajor>
    constexpr explicit WrappedMatrix(const WrappedMatrix<InputHand, InputMajor>& matrix) :
        matrix(matrix.to<WrappedMatrix>().matrix) {
    }

public:

    /**
     * @brief unwrapされた行列を取得する
     * @return unwrapした行列の参照
     */
    [[nodiscard]] constexpr const Matrix4x4& get(void) const {
        return this->matrix;
    }

    /**
     * @brief 行列内の平行移動成分を取得する
     * @return 行列内の平行移動成分
     */
    [[nodiscard]] constexpr Vector4 get_translation(void) const {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return this->matrix.r[3];
        } else {
            return DirectX::XMVectorSet(
                DirectX::XMVectorGetW(
                    this->matrix.r[0]
                ),
                DirectX::XMVectorGetW(
                    this->matrix.r[1]
                ),
                DirectX::XMVectorGetW(
                    this->matrix.r[2]
                ),
                DirectX::XMVectorGetW(
                    this->matrix.r[3]
                )
            );
        }
#endif // USE_GLM
    }

    /**
     * @brief 行列をfloatの配列(行優先)として取得する
     * @return 行優先と同じメモリ配置の16個のfloatの要素を持つ配列
     */
    [[nodiscard]] constexpr std::array<float, 16> to_row_major_array(void) const {
        std::array<float, 16> result{};

#ifdef USE_GLM
        // TODO: glmは転置する必要がある
        static_assert(false);
#else
        DirectX::XMStoreFloat4x4(
            reinterpret_cast<DirectX::XMFLOAT4X4*>(result.data()),
            this->matrix
        );

#endif // USE_GLM

        return result;
    }

    /**
     * @brief 行列をfloatの配列(列優先)として取得する
     * @return 列優先と同じメモリ配置の16個のfloatの要素を持つ配列
     */
    [[nodiscard]] constexpr std::array<float, 16> to_column_major_array(void) const {
        std::array<float, 16> result{};

#ifdef USE_GLM
        // TODO: glmは転置するはない
        static_assert(false);
#else
        DirectX::XMStoreFloat4x4(
            reinterpret_cast<DirectX::XMFLOAT4X4*>(result.data()),
            DirectX::XMMatrixTranspose(this->matrix)
        );

#endif // USE_GLM

        return result;
    }

    void set_translation(
        const float x,
        const float y,
        const float z
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        this->set_translation(
            DirectX::XMVectorSet(x, y, z, 1.0f)
        );
#endif // USE_GLM
    }

    void set_translation(
        const float x,
        const float y,
        const float z,
        const float w
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        this->set_translation(
            DirectX::XMVectorSet(x, y, z, w)
        );
#endif // USE_GLM
    }

    void set_translation(const Vector4& vec4) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            this->matrix.r[3] = vec4;
        } else {
            this->matrix.r[0] = DirectX::XMVectorSetW(
                this->matrix.r[0],
                DirectX::XMVectorGetX(vec4)
            );
            this->matrix.r[1] = DirectX::XMVectorSetW(
                this->matrix.r[1],
                DirectX::XMVectorGetY(vec4)
            );
            this->matrix.r[2] = DirectX::XMVectorSetW(
                this->matrix.r[2],
                DirectX::XMVectorGetZ(vec4)
            );
            this->matrix.r[3] = DirectX::XMVectorSetW(
                this->matrix.r[3],
                DirectX::XMVectorGetW(vec4)
            );
        }
#endif // USE_GLM
    }

public:

    /**
     * @brief 平行移動成分だけの行列を作成する
     * @param x x成分
     * @param y y成分
     * @param z z成分
     * @return 引数に指定された成分を含む行列
     */
    [[nodiscard]] static constexpr WrappedMatrix make_translation(
        const float x,
        const float y,
        const float z
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        const Matrix4x4 translate = DirectX::XMMatrixTranslation(
            x,
            y,
            z
        );
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(translate);
        } else {
            return WrappedMatrix(
                DirectX::XMMatrixTranspose(translate)
            );
        }

#endif // USE_GLM
    }

    /**
     * @brief 平行移動成分だけの行列を作成する
     * @param vec 平行移動成分となるベクトル
     * @return 引数に指定された成分を含む行列
     */
    [[nodiscard]] static constexpr WrappedMatrix make_translation_from_vector(
        const Vector4& vec
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        const Matrix4x4 translate = DirectX::XMMatrixTranslationFromVector(
            vec
        );
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(translate);
        } else {
            return WrappedMatrix(
                DirectX::XMMatrixTranspose(translate)
            );
        }

#endif // USE_GLM
    }

    /**
     * @brief 回転軸と回転量から回転行列を作成する
     * @param axis 回転軸
     * @param angle 回転量
     * @return 回転行列
     */
    [[nodiscard]] static constexpr WrappedMatrix make_rotate_from_axis_angle(
        const Vector4& axis,
        const Radian<float>& angle
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        const auto& rotate = DirectX::XMMatrixRotationAxis(
            axis,
            angle.get()
        );
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(rotate);
        } else {
            return WrappedMatrix(
                DirectX::XMMatrixTranspose(rotate)
            );
        }

#endif // USE_GLM
    }

    /**
     * @brief クォータニオンから回転行列を作成する
     * @param quaternion クォータニオン
     * @return 回転行列
     */
    [[nodiscard]] static constexpr WrappedMatrix make_rotation_from_quaternion(
        const Vector4& quaternion
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        const Matrix4x4 rotate = DirectX::XMMatrixRotationQuaternion(
            quaternion
        );
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(rotate);
        } else {
            return WrappedMatrix(
                DirectX::XMMatrixTranspose(rotate)
            );
        }

#endif // USE_GLM
    }

    [[nodiscard]] static constexpr WrappedMatrix make_rotation_from_roll_pitch_yaw(
        const Radian<float>& roll,
        const Radian<float>& pitch,
        const Radian<float>& yaw
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        const Matrix4x4 rotate = DirectX::XMMatrixRotationRollPitchYaw(
            pitch.get(),
            yaw.get(),
            roll.get()
        );
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(rotate);
        } else {
            return WrappedMatrix(
                DirectX::XMMatrixTranspose(rotate)
            );
        }

#endif // USE_GLM
    }

    [[nodiscard]] static constexpr WrappedMatrix make_scaling(
        const float x,
        const float y,
        const float z
    ) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        const Matrix4x4 scale = DirectX::XMMatrixScaling(
            x,
            y,
            z
        );
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(scale);
        } else {
            return WrappedMatrix(
                DirectX::XMMatrixTranspose(scale)
            );
        }

#endif // USE_GLM
    }

public:
    constexpr void operator=(const WrappedMatrix& input) {
        this->matrix = input.matrix;
    }
    constexpr void operator*=(const WrappedMatrix& input) {
        this->matrix *= input.matrix;
    }
    constexpr WrappedMatrix operator*(const WrappedMatrix& input) const {
        return WrappedMatrix(this->matrix * input.matrix);
    }

public:
    // 悩み中なのでいったんコメントアウト
    /*
    constexpr Vector4& operator[](const unsigned int index) const {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return this->matrix.r[index];
        } else {
            return this->transpose().matrix.r[index];
        }

#endif // USE_GLM
    }
     */

    template <bool CastHand, bool CastMajor>
    constexpr explicit operator WrappedMatrix<
        CastHand,
        CastMajor
    >(void) const {
        return WrappedMatrix::cast<
            CastHand,
            CastMajor
        >(*this);
    }

public:

    /**
     * @brief 引数に渡した行列を任意の座標系, 優先に合わせた行列変換する
     * @tparam CastHand 変換する座標系
     * @tparam CastMajor 変換する行列の優先
     * @param matrix 変換対象のラップされた行列
     * @return 任意の座標系と優先に変換されたラップされた行列
     */
    template <bool CastHand, bool CastMajor>
    [[nodiscard]] static constexpr WrappedMatrix<CastHand, CastMajor> cast(const WrappedMatrix& matrix) {
        using Output = WrappedMatrix<CastHand, CastMajor>;
        constexpr bool is_same_hand = Output::IS_LEFT_HAND == WrappedMatrix::IS_LEFT_HAND;
        constexpr bool is_same_major = Output::IS_ROW_MAJOR == WrappedMatrix::IS_ROW_MAJOR;

        if constexpr(is_same_hand) {
            if constexpr(is_same_major) {
                return Output(matrix);
            } else {
                return matrix.transpose();
            }
        } else {
            if constexpr(is_same_major) {
                return matrix.inverse_z();
            } else {
                return matrix.transpose().inverse_z();
            }
        }
    }

    /**
     * @brief 引数に渡した行列を任意の座標系, 優先に合わせた行列変換する
     * @tparam T 変換するラップされた行列の型
     * @param matrix 変換対象のラップされた行列
     * @return 任意の座標系と優先に変換されたラップされた行列
     */
    template <IsWrappedMatrix T>
    [[nodiscard]] static constexpr WrappedMatrix<
        T::IS_LEFT_HAND,
        T::IS_ROW_MAJOR
    > cast(const WrappedMatrix& matrix) {
        return WrappedMatrix::cast<T::IS_LEFT_HAND, T::IS_ROW_MAJOR>(matrix);
    }

    /**
     * @brief 引数に渡した行列を任意の座標系, 優先に合わせた行列変換する
     * @tparam T 変換するラップされた行列の型
     * @param matrix 変換対象のラップされた行列
     * @return 任意の座標系と優先に変換されたラップされた行列
     */
    template <IsWrappedMatrix T>
    [[nodiscard]] constexpr WrappedMatrix<
        T::IS_LEFT_HAND,
        T::IS_ROW_MAJOR
    > to(void) const {
        return WrappedMatrix::cast<
            T::IS_LEFT_HAND,
            T::IS_ROW_MAJOR
        >(*this);
    }

public:
    [[nodiscard]] constexpr WrappedMatrix::WrappedDirectXMatrix to_directx(void) const {
        return this->to<WrappedMatrix::WrappedDirectXMatrix>();
    }
    [[nodiscard]] constexpr WrappedMatrix::WrappedOpenGLMatrix to_opengl(void) const {
        return this->to<WrappedMatrix::WrappedOpenGLMatrix>();
    }
    [[nodiscard]] constexpr WrappedMatrix::WrappedBulletMatrix to_bullet(void) const {
        return this->to<WrappedMatrix::WrappedBulletMatrix>();
    }
    [[nodiscard]] constexpr WrappedMatrix::WrappedMMDMatrix to_mmd(void) const {
        return this->to<WrappedMatrix::WrappedMMDMatrix>();
    }

public:
    [[nodiscard]] constexpr WrappedMatrix<!IsLeftHand, IsRowMajor> inverse_z(void) const {
        using Output = WrappedMatrix<!IsLeftHand, IsRowMajor>;

#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        const auto scale = DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
        return Output(
            scale * this->matrix * scale
        );
#endif // USE_GLM
    }

    [[nodiscard]] constexpr WrappedMatrix<IsLeftHand, !IsRowMajor> transpose(void) const {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        return WrappedMatrix<IsLeftHand, !IsRowMajor>(
            DirectX::XMMatrixTranspose(this->matrix)
        );
#endif // USE_GLM
    }

    [[nodiscard]] constexpr WrappedMatrix inverse(void) const {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        return WrappedMatrix(
            DirectX::XMMatrixInverse(nullptr, this->matrix)
        );
#endif // USE_GLM
    }

public:

    [[nodiscard]] static constexpr WrappedMatrix make_zero_matrix(void) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        return WrappedMatrix(Matrix4x4{});
#endif // USE_GLM
    }

    [[nodiscard]] static constexpr WrappedMatrix make_identity_matrix(void) {
#ifdef USE_GLM
        // TODO:
        static_assert(false);
#else
        return WrappedMatrix(DirectX::XMMatrixIdentity());
#endif // USE_GLM
    }

    /**
     * @brief
     * @param model
     * @param view
     * @param projection
     * @return
     */
    [[nodiscard]] static constexpr WrappedMatrix make_camera_matrix(
        const WrappedMatrix& model,
        const WrappedMatrix& view,
        const WrappedMatrix& projection
    ) {
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(
                model.matrix * view.matrix * projection.matrix
            );
        } else {
            return WrappedMatrix(
                projection.matrix * view.matrix * model.matrix
            );
        }
    }

    /**
     * @brief
     * @param translate
     * @param rotate
     * @param scale
     * @return
     */
    [[nodiscard]] static constexpr WrappedMatrix make_transform_matrix(
        const WrappedMatrix& translate,
        const WrappedMatrix& rotate,
        const WrappedMatrix& scale
    ) {
        if constexpr(WrappedMatrix::IS_ROW_MAJOR) {
            return WrappedMatrix(
                scale.matrix * rotate.matrix * translate.matrix
            );
        } else {
            return WrappedMatrix(
                translate.matrix * rotate.matrix * scale.matrix
            );
        }
    }

public:

    /**
     * @brief 引数に渡した順番に行列を掛ける
     *        基準の世界の優先とと引数に渡した行列の優先が違う場合逆順に掛けられる
     * @tparam T WrappedMatrix型
     * @tparam ...Args Tと同一のWrappedMatrix型
     * @param base ラップされた行列
     * @param ...args 掛けるラップされた行列
     * @return 引数に渡した行列を全て掛け合わせた結果が返る
     */
    template <IsWrappedMatrix T, IsWrappedMatrix... Args>
        requires (std::same_as<T, Args> && ...)
    [[nodiscard]] static constexpr WrappedMatrix multiply(
        const T& base,
        const Args&... args
    ) {
        if constexpr(WrappedMatrix::IS_ROW_MAJOR == T::IS_ROW_MAJOR) {
            // 左から右に掛け続けるのであれば
            // 結合法則があるので先に展開先から計算しても問題ない
            return (base * (args * ...)).to<WrappedMatrix>();
        } else {
            // 基準世界の優先と引数の行列の優先が違えば逆順に掛けるようにする
            // 一度tupleに変換し, tupleに逆順アクセスして掛けていく
            constexpr std::size_t N = sizeof...(Args) + 1;
            const auto& args_tuple = std::forward_as_tuple(base, args...);
            const auto& reverse = [&]<std::size_t... I>(const std::index_sequence<I...>&_) -> T {
                return (std::get<N - 1 - I>(args_tuple) * ...);
            };
            return reverse(std::make_index_sequence<N>{}).to<WrappedMatrix>();
        }
    }
};

using DirectXMatrix = WrappedMatrix<>::WrappedDirectXMatrix;
using OpenGLMatrix = WrappedMatrix<>::WrappedOpenGLMatrix;
using BulletMatrix = WrappedMatrix<>::WrappedBulletMatrix;
using MMDMatrix = WrappedMatrix<>::WrappedMMDMatrix;
