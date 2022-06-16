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

            display_list(display_list&& other);

            void resize(std::size_t size);

            template<typename F>
            inline void write_into(F func) {
                GX_BeginDispList(m_data, m_size);
                func();
                m_size = GX_EndDispList();
            }

            inline void call() {
                GX_CallDispList(m_data, m_size);
            }
    };
};