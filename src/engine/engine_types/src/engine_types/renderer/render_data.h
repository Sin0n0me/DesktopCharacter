#pragma once
#include <memory>
#include <vector>

namespace enishi::types {
    enum class AccessFlag : std::uint8_t {
        None,
        ReadOnly,
        WriteOnly,
        ReadAndWrite,
    };

    enum class ResourceAccessFlag : std::uint8_t {
        CPURead,
        CPUWrite,
        GPURead,
        GPUWrite,
    };

    struct RenderData {
        void* data;               // データのポインタ
        std::uint32_t stride;     // 1つのデータ間隔
        std::uint32_t byte_width; // 全データのサイズ

        explicit RenderData(
            void* const data, const std::uint32_t stride, const std::uint32_t byte_width)
            : data(data)
            , stride(stride)
            , byte_width(byte_width) {
        }
    };

    template <typename T> class OwnedRenderData {
      private:
        std::vector<T> buffer;
        std::shared_ptr<RenderData> render_data;

      public:
        explicit OwnedRenderData(std::vector<T>&& buffer)
            : buffer(buffer)
            , render_data(std::make_shared<RenderData>(buffer.data(),
                  static_cast<std::uint32_t>(sizeof(T)),
                  static_cast<std::uint32_t>(sizeof(T) * buffer.size()))) {
        }

        T& operator[](const std::uint32_t index) {
            this->buffer[index];
        }

        [[nodiscard]] std::weak_ptr<const RenderData> get_render_data(void) const {
            return this->render_data;
        }
    };
} // namespace enishi::types