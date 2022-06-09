#pragma once
#include <vector>

namespace ext {
    // This is a custom container that stores an array with a fixed size but the initial size is not known at compile time
    template<typename T, typename A = std::allocator<T>>
    class fixed_array {
        std::size_t m_size;
        T* m_data;
        A alloc;
        public:
            inline fixed_array(std::size_t m_size) {
                this->m_size = m_size;
                this->m_data = alloc.allocate(m_size);
            }
            inline fixed_array(fixed_array&& other) {
                this->m_size = other.m_size;
                this->m_data = other.m_data;
                other.m_size = 0;
                other.m_data = nullptr;
            }

            inline ~fixed_array() {
                if (m_data != nullptr) {
                    alloc.deallocate(m_data, m_size);
                }
            }

            fixed_array(const fixed_array& other) = delete;
            fixed_array& operator=(const fixed_array& other) = delete;

            using pointer = T*;
            using const_pointer = const T*;

            using iterator = __gnu_cxx::__normal_iterator<T*, fixed_array>;
            using const_iterator = __gnu_cxx::__normal_iterator<const T*, fixed_array>;

            inline T& operator[](std::size_t index) { return m_data[index]; }
            inline const T& operator[](std::size_t index) const { return m_data[index]; }

            inline std::size_t size() const { return m_size; }

            inline iterator begin() { return iterator(m_data); }
            inline iterator end() { return iterator(m_data + m_size); }
            inline const_iterator begin() const { return const_iterator(m_data); }
            inline const_iterator end() const { return const_iterator(m_data + m_size); }
    };
}