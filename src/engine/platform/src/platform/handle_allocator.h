#include <engine_types/handler_types.h>
#include <vector>

namespace enishi::platform {
    class HandleAllocator {
      private:
        types::HandleId next_id;
        std::vector<bool> alive;
        std::vector<types::HandleId> free_list;

      public:
        types::HandleId create(void);

        void destroy(const types::HandleId id);

        bool is_alive(const types::HandleId id) const noexcept;
    };
} // namespace enishi::platform