#include "chunk_rendering.hpp"
#include "gfx.hpp"

using namespace game;

static void set_alpha(u8 alpha) {
	GX_SetTevColor(GX_TEVREG1, { 0xff, 0xff, 0xff, alpha });
}

void game::draw_chunks(const math::matrix view, const camera& cam, chunk::map& chunks) {
	for_each_block_mesh_layer([view, &cam, &chunks]<typename L>() {
		L::init_chunk_rendering();

		if constexpr (std::is_same_v<L, block_mesh_layer::standard>) {
			if (cam.update_view) {
				for (auto& [ pos, chunk ] : chunks) {
					set_alpha(chunk.alpha);

					chunk.tf.update_model_view(view);
					chunk.tf.load(chunk::mat);

					chunk.core_disp_list_layers.standard.call();
					chunk.shell_disp_list_layers.standard.call();
				}
			} else {
				for (auto& [ pos, chunk ] : chunks) {
					chunk.core_disp_list_layers.standard.call();
					chunk.shell_disp_list_layers.standard.call();
				}
			}
		}

		for (auto& [ pos, chunk ] : chunks) {
			set_alpha(chunk.alpha);

			chunk.tf.load(chunk::mat);
			chunk.core_disp_list_layers.template get_layer<L>().call();
			chunk.shell_disp_list_layers.template get_layer<L>().call();
		}
	});
}