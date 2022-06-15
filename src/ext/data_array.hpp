#pragma once
#include <vector>

namespace ext {
    // This is a custom container that stores a POD array.
    template<typename T, typename A = std::allocator<T>>
    class data_array {
        static_assert(std::is_pod_v<T>, "T must be a POD type.");
        std::size_t m_size;
        T* m_data;
        A alloc;
        public:
            inline data_array() : m_size(0), m_data(nullptr) {}
            inline data_array(std::size_t m_size) : m_size(m_size), m_data(alloc.allocate(m_size)) {}
            inline data_array(data_array&& other) : m_size(other.m_size), m_data(other.m_data) {
                other.m_size = 0;
                other.m_data = nullptr;
            }

            inline ~data_array() {
                if (m_data != nullptr) {
                    alloc.deallocate(m_data, m_size);
                }
            }

            data_array(const data_array& other) = delete;
            data_array& operator=(const data_array& other) = delete;

            inline data_array& operator=(data_array&& other) {
                if (m_data != nullptr) {
                    alloc.deallocate(m_data, m_size);
                }
                m_size = other.m_size;
                m_data = other.m_data;
                other.m_size = 0;
                other.m_data = nullptr;
                return *this;
            }

            using pointer = T*;
            using const_pointer = const T*;

            using iterator = __gnu_cxx::__normal_iterator<T*, data_array>;
            using const_iterator = __gnu_cxx::__normal_iterator<const T*, data_array>;

            inline void resize_without_copying(std::size_t new_size) {
                alloc.deallocate(m_data, m_size);
                m_size = new_size;
                m_data = alloc.allocate(m_size);
            }

            inline T& operator[](std::size_t index) { return m_data[index]; }
            inline const T& operator[](std::size_t index) const { return m_data[index]; }

            inline std::size_t size() const { return m_size; }

            inline iterator begin() { return iterator(m_data); }
            inline iterator end() { return iterator(m_data + m_size); }
            inline const_iterator begin() const { return const_iterator(m_data); }
            inline const_iterator end() const { return const_iterator(m_data + m_size); }
    };
}