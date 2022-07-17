#pragma once
#include <cstdint>
#include <gccore.h>

namespace gfx {
    class display_list {
        void* m_data = nullptr;
        std::size_t m_size = 0;
        public:
            display_list() = default;
            ~display_list();

            display_list(const display_list&) = delete;
            display_list& operator=(const display_list&) = delete;

            display_list(display_list&& other) noexcept;

            void resize(std::size_t size);

            template<typename F>
            inline void write_into(F func) {
                DCInvalidateRange(m_data, m_size);
                GX_BeginDispList(m_data, m_size);
                func();
                std::size_t new_size = GX_EndDispList();
                DCFlushRange(m_data, m_size);
                m_size = new_size;
            }

            inline void call() const {
                GX_CallDispList(m_data, m_size);
            }

            inline void checked_call() const {
                if (m_size == 0) {
                    return;
                }
                call();
            }

            inline std::size_t size() const {
                return m_size;
            }
    };
};