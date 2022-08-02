#pragma once
#include "block_mesh_generation.hpp"

namespace game {

    static constexpr std::size_t safe_buffer_overflow_size = 0x100;

    template<typename T, std::size_t MAX_QUAD_COUNT>
    struct block_quad_array {
        using iterator = ext::data_array<T>::iterator;
        ext::data_array<T> quads;

        inline block_quad_array() : quads(MAX_QUAD_COUNT + safe_buffer_overflow_size) {}

        inline auto begin() { return quads.begin(); }
        inline auto end() { return quads.end(); }
        inline auto begin() const { return quads.begin(); }
        inline auto end() const { return quads.end(); }

        inline T* data() { return quads.data(); }
        inline const T* data() const { return quads.data(); }
    };

    namespace block_mesh_layer {
        struct standard {
            static constexpr const char* name = "standard";

            using chunk_quad = standard_quad;
            static constexpr std::size_t max_quad_count = 0x2000;
            using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

            static std::size_t get_chunk_display_list_size(std::size_t vert_count);
            static void write_chunk_vertex(const chunk_quad::vertex& vert);

            static void init_chunk_rendering();
        };

        struct tinted {
            static constexpr const char* name = "tinted";

            using chunk_quad = tinted_quad;
            static constexpr std::size_t max_quad_count = 0x1000;
            using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

            static std::size_t get_chunk_display_list_size(std::size_t vert_count);
            static void write_chunk_vertex(const chunk_quad::vertex& vert);

            static void init_chunk_rendering();
        };

        struct tinted_alpha {
            static constexpr const char* name = "tinted alpha block";

            using chunk_quad = tinted_quad;
            static constexpr std::size_t max_quad_count = 0x500;
            using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

            static std::size_t get_chunk_display_list_size(std::size_t vert_count);
            static void write_chunk_vertex(const chunk_quad::vertex& vert);

            static void init_chunk_rendering();
        };

        struct tinted_double_side_alpha {
            static constexpr const char* name = "tinted double side alpha";

            using chunk_quad = tinted_quad;
            static constexpr std::size_t max_quad_count = 0x1000;
            using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

            static std::size_t get_chunk_display_list_size(std::size_t vert_count);
            static void write_chunk_vertex(const chunk_quad::vertex& vert);

            static void init_chunk_rendering();
        };

        struct tinted_decal {
            static constexpr const char* name = "tinted decal";

            using chunk_quad = tinted_decal_quad;
            static constexpr std::size_t max_quad_count = 0x1000;
            using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

            static std::size_t get_chunk_display_list_size(std::size_t vert_count);
            static void write_chunk_vertex(const chunk_quad::vertex& vert);

            static void init_chunk_rendering();
        };
    }

    #define EVAL_MACRO_ON_FIRST_BLOCK_MESH_LAYERS(macro) \
    macro(standard) \
    macro(tinted) \
    macro(tinted_alpha) \
    macro(tinted_double_side_alpha)

    #define EVAL_MACRO_ON_LAST_BLOCK_MESH_LAYER(macro) \
    macro(tinted_decal)

    #define EVAL_MACRO_ON_BLOCK_MESH_LAYERS(macro) \
    EVAL_MACRO_ON_FIRST_BLOCK_MESH_LAYERS(macro) \
    EVAL_MACRO_ON_LAST_BLOCK_MESH_LAYER(macro)

    template<typename F>
    constexpr void for_each_block_mesh_layer(const F& func) {
        #define EVAL(layer) func.template operator()<block_mesh_layer::layer>();
        EVAL_MACRO_ON_BLOCK_MESH_LAYERS(EVAL)
        #undef EVAL
    }

    template<typename C>
    struct block_mesh_layers {
        #define EVAL(layer) C::template type<block_mesh_layer::layer> layer;
        EVAL_MACRO_ON_BLOCK_MESH_LAYERS(EVAL)
        #undef EVAL

        block_mesh_layers() = default;

        template<typename F>
        constexpr block_mesh_layers(const F& func) :
            #define EVAL(layer) layer(func.template operator()<block_mesh_layer::layer>()),
            EVAL_MACRO_ON_FIRST_BLOCK_MESH_LAYERS(EVAL)
            #undef EVAL
            #define EVAL(layer) layer(func.template operator()<block_mesh_layer::layer>())
            EVAL_MACRO_ON_LAST_BLOCK_MESH_LAYER(EVAL)
            #undef EVAL
        {}

        template<typename L>
        constexpr const auto& get_layer() const {
            #define EVAL(layer) if constexpr (std::is_same_v<L, block_mesh_layer::layer>) return layer;
            EVAL_MACRO_ON_BLOCK_MESH_LAYERS(EVAL)
            #undef EVAL
        }

        template<typename L>
        constexpr auto& get_layer() {
            #define EVAL(layer) if constexpr (std::is_same_v<L, block_mesh_layer::layer>) return layer;
            EVAL_MACRO_ON_BLOCK_MESH_LAYERS(EVAL)
            #undef EVAL
        }
    };

    #undef EVAL_MACRO_ON_BLOCK_MESH_LAYERS
    
    template<typename T>
    struct single_type_container {
        template<typename T1>
        using type = T;
    };

    struct chunk_quad_array_container {
        template<typename T>
        using type = T::chunk_quad_array;
    };

    struct chunk_quad_array_iterator_container {
        template<typename T>
        using type = T::chunk_quad_array::iterator;
    };
};