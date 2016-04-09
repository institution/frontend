#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "../ext/ext.hpp"
#include "glm.hpp"
#include "color.hpp"

namespace frontend {

	using v2s = ext::v2<int16_t>;
	using v2i = ext::v2<int32_t>;
	using v2f = ext::v2<float_t>;
	using b2s = ext::b2<int16_t>;

	using Image = darray2::darray2<Color, int16_t>;


	struct Texture {
		GLuint id{0};
		v2s dim;
		GLenum format{GL_RGBA};

		void create();
		void destroy();

		Texture() = default;
		Texture(Texture const& o) = delete;		
		Texture(Texture && o):
			id(o.id),
			dim(o.dim),
			format(o.format)
		{			
			o.id = 0;
		}		
		void operator=(Texture && o) {
			id = o.id;
			dim = o.dim;
			format = o.format;
			o.id = 0;
		}
		~Texture() {
			if (id != 0) {
				destroy();
			}
		}
	};

	struct PixFont;


		

		


	struct Front {
		
		// window & context
		SDL_Window * win{nullptr};
		SDL_GLContext ctx;

		v2s win_dim{0,0};
		v2s ctx_dim{0,0};

		// opengl stuff
		GLuint vao[1];
		GLuint vbo[1];
		GLuint prog[1];

		glm::mat4 proj;
		Texture white1x1;
		
		// misc
		bool verbose{false};
		bool done{false};

		// public interface
		Front() = default;
		~Front();
		void init(std::string const& title, v2s dim);			
		
		void flip() { SDL_GL_SwapWindow(win); }

		Texture make_texture(filesys::Path const& path);
		Texture make_texture(Image const& img);
		Texture make_texture(uint8_t const* rgba, v2s dim);
		
		PixFont make_font(filesys::Path const& path, int adv);


		void render_texture(Texture const& t, v2s pos_);
		void render_texture(Texture const& t, v2s trg, b2s src);
		void render_texture(Texture const& t, v2s trg, b2s src, Color fg);

		//void render_texture(Texture const& t, b2s trg, b2s src, Color fg);

		void render_fill(b2s box, Color c);

		

		void stop() { done = true; }

		void clear();

	private:
		void set_blend_font(Color c);
		void set_blend_norm();
		void render_subtexture(Texture const& t, v2s trg, b2s src);
		void _render_call_GL(GLenum tex_id, GLfloat * data, size_t size);

		void create_SDL(std::string const& title, v2s dim);
		void destroy_SDL();

		void create_GL();
		void destroy_GL();
	};

	inline Front::~Front() {
		destroy_GL();
		destroy_SDL();
	}

	inline void Front::init(std::string const& title, v2s dim) {
		create_SDL(title, dim);
		create_GL();
	}


	Image load_png(filesys::Path const& path);

}

#include "pixfont.hpp"

