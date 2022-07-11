#pragma once
#include <malloc.h>
#include <vector>

namespace ext {
    // This is a custom container that stores a POD array.
    template<typename T>
    class data_array {
        static_assert(std::is_trivial_v<T>, "T must be a POD type.");
        T* m_data = nullptr;
        std::size_t m_size = 0;
        public:
            inline data_array() = default;
            inline explicit data_array(std::size_t size) : m_data((T*)malloc(size * sizeof(T))), m_size(size) {}
            inline data_array(data_array&& other) noexcept : m_data(other.m_data), m_size(other.m_size) {
                other.m_data = nullptr;
                other.m_size = 0;
            }

            inline ~data_array() {
                if (m_data != nullptr) {
                    free(m_data);
                }
            }

            data_array(const data_array& other) = delete;
            data_array& operator=(const data_array& other) = delete;

            inline data_array& operator=(data_array&& other) noexcept {
                if (m_data != nullptr) {
                    free(m_data);
                }
                m_data = other.m_data;
                m_size = other.m_size;
                other.m_size = 0;
                other.m_data = nullptr;
                return *this;
            }

            using pointer = T*;
            using const_pointer = const T*;

            using iterator = __gnu_cxx::__normal_iterator<T*, data_array>;
            using const_iterator = __gnu_cxx::__normal_iterator<const T*, data_array>;

            inline void resize_without_copying(std::size_t new_size) {
                if (new_size == m_size) {
                    return;
                }
                if (m_data != nullptr) {
                    free(m_data);
                }
                m_data = (T*)malloc(new_size * sizeof(T));
                m_size = new_size;
            }

            inline T* data() {
                return m_data;
            }

            inline const T* data() const {
                return m_data;
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