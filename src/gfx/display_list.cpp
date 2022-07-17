#include "display_list.hpp"
#include <cstring>
#include <malloc.h>

using namespace gfx;

display_list::~display_list() {
    if (m_data != nullptr) {
        free(m_data);
    }
}

display_list::display_list(display_list&& other) noexcept : m_data(other.m_data), m_size(other.m_size) {
    other.m_data = nullptr;
    other.m_size = 0;    
}

void display_list::resize(std::size_t size) {
    if (m_data != nullptr) {
        free(m_data);
    }

    // Align to 32 bytes
    m_size = (((size + 63) >> 5) + 1) << 5;

    // Allocate memory
    m_data = memalign(32, m_size);
    std::memset(m_data, 0, m_size);
}