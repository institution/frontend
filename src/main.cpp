#include <fstream>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <cmath>
#include <array>
#include <cstdint>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>


#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>

//#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#include "aga3.hpp"
#include "aga2.hpp"
#include "gl.h"


std::string get_file_contents(std::string const& filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}


template <class T>
struct Color {
	T r, g, b, a;
	Color() {}
	Color(T r, T g, T b, T a = 0): r(r), g(g), b(b), a(a) {}

	template<class A>
	void serialize(A & ar, uint const& version) {
		ar & r;
		ar & g;
		ar & b;
		ar & a;
	}
};

using Real = float;
using v3f = aga3::Mv1<Real>;
using v2f = aga2::Mv1<Real>;
using c4f = Color<Real>;
using r3f = aga3::Mv02<Real>;

using v3r = aga3::Mv1<Real>;
using v2r = aga2::Mv1<Real>;
using c4r = Color<Real>;
using r3r = aga3::Mv02<Real>;


/*
([(-1, -1, -1),
  (1, -1, -1),
  (-1, 1, -1),
  (1, 1, -1),
  (-1, -1, 1),
  (1, -1, 1),
  (-1, 1, 1),
  (1, 1, 1)],
 [(0, 1),
  (2, 3),
  (0, 2),
  (1, 3),
  (4, 5),
  (6, 7),
  (4, 6),
  (5, 7),
  (0, 4),
  (1, 5),
  (2, 6),
  (3, 7)])
*/
	 
	
template <class T>
auto rotate_matrix4(aga3::Mv02<T> const& r) -> std::array<T, 16> {
	// Convert rotor3 to glMatrix4.
	// rotor -- multivector
	// return -- opengl (loadable to matrix) list
	// [dorst] chapter 7.10.4
	
	auto w = r[0];
	auto z = r[1];
	auto x = r[2];
	auto y = r[3];

	return std::array<T, 16>({
		T(1.-2.*y*y-2.*z*z), T(2.*x*y-2.*w*z),    T(2.*x*z+2.*w*y),    T(0.),
		T(2.*y*x+2.*w*z),    T(1.-2.*z*z-2.*x*x), T(2.*y*z-2.*w*x),    T(0.),
		T(2.*z*x-2.*w*y),    T(2.*z*y+2.*w*x),    T(1.-2.*x*x-2.*y*y), T(0.),
		T(0.),               T(0.),               T(0.),               T(1.),
	});
}

template <class T>
auto translate_matrix4(aga3::Mv1<T> const& r) -> std::array<T, 16> {
	auto x = r[0];
	auto y = r[1];
	auto z = r[2];
	
	return std::array<T, 16>({
		T(1), T(0), T(0), T(0),
		T(0), T(1), T(0), T(0),
		T(0), T(0), T(1), T(0),
		T(x), T(y), T(z), T(1),
	});
}


template <class T>
auto imul(T t0, T t1, T t2, T t3, T y0, T y1, T y2, T y3) -> T {
	return t0*y0 + t1*y1 + t2+y2 + t3*y3;
}


template <class T>
auto mul(std::array<T, 16> const& a, std::array<T, 16> const& b) -> std::array<T, 16> {
	
	//0  1  2  3
	//4  5  6  7
	//8  9  10 11
	//12 13 14 15
		
	std::array<T, 16> r;
	
	r[0] = imul(a[0], a[4], a[8], a[12], b[0], b[1], b[2], b[3]);
	r[4] = imul(a[0], a[4], a[8], a[12], b[4], b[5], b[6], b[7]);
	r[8] = imul(a[0], a[4], a[8], a[12], b[8], b[9], b[10], b[11]);
	r[12] = imul(a[0], a[4], a[8], a[12], b[12], b[13], b[14], b[15]);
	
	r[1] = imul(a[1], a[5], a[9], a[13], b[0], b[1], b[2], b[3]);
	r[5] = imul(a[1], a[5], a[9], a[13], b[4], b[5], b[6], b[7]);
	r[9] = imul(a[1], a[5], a[9], a[13], b[8], b[9], b[10], b[11]);
	r[13] = imul(a[1], a[5], a[9], a[13], b[12], b[13], b[14], b[15]);
		 
	r[2] = imul(a[2], a[6], a[10], a[14], b[0], b[1], b[2], b[3]);
	r[6] = imul(a[2], a[6], a[10], a[14], b[4], b[5], b[6], b[7]);
	r[10] = imul(a[2], a[6], a[10], a[14], b[8], b[9], b[10], b[11]);
	r[14] = imul(a[2], a[6], a[10], a[14], b[12], b[13], b[14], b[15]);
	
	r[3] = imul(a[3], a[7], a[11], a[15], b[0], b[1], b[2], b[3]);
	r[7] = imul(a[3], a[7], a[11], a[15], b[4], b[5], b[6], b[7]);
	r[11] = imul(a[3], a[7], a[11], a[15], b[8], b[9], b[10], b[11]);
	r[15] = imul(a[3], a[7], a[11], a[15], b[12], b[13], b[14], b[15]);
		 
	return r;
}








void init_glew() {
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		//glewGetErrorString(err)
		throw std::runtime_error("aaa");
	}
	
	if (!GLEW_VERSION_4_4) {
		exit(1); // or handle the error in a nicer way
	}
}
	


int main() {
	int screen_width = 640;
	int screen_height = 480;
	
    sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 4;
	sf::Window window(sf::VideoMode(screen_width, screen_height), "OpenGL", sf::Style::Default, settings);

    window.setVerticalSyncEnabled(true);
	
	init_glew();
	
	
	Sys sys;
	
	sys.create_part(v3r(-1,0,0));
	sys.create_part(v3r(+1,0,0));
	sys.create_part(v3r(0,+1,0));
	
	sys.create_link(0,1);
	sys.create_link(1,2);
	sys.create_link(0,2);
	
	
	
	
	auto vert_shad = gl::create_shader(GL_VERTEX_SHADER, get_file_contents("src/vert_shad.glsl"));
	auto frag_shad = gl::create_shader(GL_FRAGMENT_SHADER, get_file_contents("src/frag_shad.glsl"));
	auto prog = gl::create_program(vert_shad, frag_shad);
		
	gl::enable(GL_BLEND);
	gl::blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	gl::enable(GL_DEPTH_TEST);
	gl::depth_func(GL_LESS);

	gl::clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	sf::Clock clock;
	
	
	
	
	auto part_buff = gl::create_buffer(GL_ARRAY_BUFFER, parts, GL_STATIC_DRAW);	
	auto link_buff = gl::create_buffer(GL_ELEMENT_ARRAY_BUFFER, sys.s_ls, GL_STATIC_DRAW);
	
	
	auto a_mass = gl::get_attrib_location(prog, "mass");
	auto a_pos = gl::get_attrib_location(prog, "pos");
	auto a_transform = gl::get_uniform_location(prog, "m_transform");
	auto u_mvp = gl::get_uniform_location(prog, "mvp");
	
	
	
	
	// run the main loop
	bool running = true;
	while (running)
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				// end the program
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				screen_width = event.size.width;
				screen_height = event.size.height;
					
				gl::viewport(0, 0, screen_width, screen_height);
				
			}
		}
		
		
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
		glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 10.0f);
		glm::mat4 mvp = projection * view * model;
		
		auto sec = clock.getElapsedTime().asSeconds();
		
		auto r = aga3::rotor(
			aga3::e0 ^ aga3::e1, 
			float(((2.0 * M_PI) / 30.0) * sec)
		);
		
		auto rotat_mat = rotate_matrix4(r);
		auto trans_mat = translate_matrix4(v3f(float(0.1 * sec), 0, 0));
		auto mat = glm::mat4(1.0f);
		
		//gl::uniform_matrix4f(a_transform, glm::value_ptr(mat));		
		gl::uniform_matrix4f(a_transform, &rotat_mat[0]);
		gl::uniform_matrix4f(u_mvp, glm::value_ptr(mvp));
		
		
		// draw...
		/*
		vector<int> es;
		es.push_back(0);
		es.push_back(1);
		es.push_back(2);*/

		// clear the buffers
		gl::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		gl::use_program(prog);

		/*
		gl::enable_vertex_attrib_array(a_mass);
		gl::bind_buffer(GL_ARRAY_BUFFER, part_buff);
		gl::vertex_attrib_pointer(
			a_mass,   // attribute
			1,         // number of elements per vertex, here (x,y)
			GL_FLOAT,  // the type of each element
			GL_FALSE,  // take our values as-is
			6,         // no extra data between each position
			0          // offset of first element
		);
		*/
		
		gl::enable_vertex_attrib_array(a_pos);
		gl::bind_buffer(GL_ARRAY_BUFFER, part_buff);
		gl::vertex_attrib_pointer(
			a_pos,             // attribute
			3,                 // number of elements per vertex
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // extra data between each position
			(void*)0           // offset of first element
		);
		
		gl::bind_buffer(GL_ARRAY_BUFFER);

		gl::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, link_buff);
		auto size = gl::get_buffer_parameter_iv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE);
		gl::draw_elements(GL_LINES, size/sizeof(std::uint32_t), GL_UNSIGNED_INT, 0);
		gl::bind_buffer(GL_ELEMENT_ARRAY_BUFFER);
		
		//gl::bind_buffer(GL_ARRAY_BUFFER, part_buff);
		//gl::draw_arrays(GL_TRIANGLES, 0, 10);
		//gl::bind_buffer(GL_ARRAY_BUFFER);
		
		
		
		
		gl::disable_vertex_attrib_array(a_pos);
		gl::disable_vertex_attrib_array(a_mass);

		// end the current frame (internally swaps the front and back buffers)
		window.display();
	}

	
	return 0;
}