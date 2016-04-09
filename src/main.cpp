#include "frontend/front.hpp"




/*struct Texture{
	GLuint id{0};
	uint16_t width{0};
	uint16_t height{0};
	GLenum format;

	Texture() {}

};*/






// GL_REPEAT

// render rectangular image



//void render_subtexture



/*void Front::render_texture(Texture id, v2i pos, v2i end) {
// drawing				             
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		//glDrawArrays(GL_POINTS, 0, 3);
		CHECK_GL();		
}*/

using frontend::Front;
using frontend::Texture;
using frontend::Color;
using frontend::Image;

int main() {

	Front front;
	front.init("Ala ma Kota", {800,600});
	
			

	auto t = front.make_texture("res/zecora.png");
	
	auto f = front.make_font("res/testfont.png", -1);
	
	bool run = 1;
	while (run) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				run = 0;
			}
		}

		front.render_fill({{0,0},{700,500}}, Color(200,100,100,255));

		front.render_texture(t, {20,20});

		f.render_glyph(
			front,
			{200,20},
			f.get_glyph(uint32_t('A')),
			Color(255,0,0)
		);

		
		
		front.flip();
	}

	
	return 0;
}





