#include <stdexcept>
#include <SDL2/SDL.h>
#include "ext/ext.hpp"

#include <GL/glew.h>


void init_glew() {
	// GL_INVALUD_ENUM bug
	glewExperimental=GL_TRUE;
	
	auto x = glewInit();
	if (x != GLEW_OK) {
		print(std::cerr, "ERROR: GLEW: %||\n", glewGetErrorString(x));
		std::exit(-1);
	}

	// GL_INVALUD_ENUM bug
	auto err = glGetError();
	switch (err) {
		case GL_INVALID_ENUM:
			//print(std::cerr, "INFO: glewInit: GL_INVALID_ENUM\n");
			break;
		case GL_NO_ERROR:
			break;
		default:
			ext::fail("unexpected error\n");
	}
	
}


char const* get_gl_err_msg(GLuint x) {
	#define CASE_ERR(X) case X: return #X
	switch(x) {
		CASE_ERR(GL_NO_ERROR);
		CASE_ERR(GL_INVALID_ENUM);
		CASE_ERR(GL_INVALID_VALUE);
		CASE_ERR(GL_INVALID_OPERATION);
		CASE_ERR(GL_INVALID_FRAMEBUFFER_OPERATION);
		CASE_ERR(GL_OUT_OF_MEMORY);
		CASE_ERR(GL_STACK_UNDERFLOW);
		CASE_ERR(GL_STACK_OVERFLOW);
	}
	#undef CASE_ERR
	return "unknown opengl error code";
}

#define CHECK_GL() \
	check_gl(__FILE__, __LINE__) 

void check_gl(char const* fname, int line=0) {
	auto x = glGetError();
	if (x != GL_NO_ERROR) {
		do {
			print(std::cerr, "Error at [%||:%||]: %|| (code %||)\n", fname, line, get_gl_err_msg(x), x);
			x = glGetError();
		} while (x != GL_NO_ERROR);		
		assert(0);
		std::exit(-1);
	}
}


void myLinkProgram(GLuint program) {
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		char log[len];

		glGetProgramInfoLog(program, len, nullptr, log);
		ext::fail("Error while linking program: %||\n", log);
	}
}

void myLoadShader(GLuint shader, char const* shader_src)
{
	glShaderSource(shader, 1, &shader_src, nullptr);
	CHECK_GL();	
	glCompileShader(shader);
	CHECK_GL();	
	
	GLint compiled;	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	CHECK_GL();

	if (!compiled)
	{
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		CHECK_GL();

		char log[len];
		
		glGetShaderInfoLog(shader, len, nullptr, log);
		CHECK_GL();	

		print(std::cerr, shader_src);
		ext::fail("Error while compiling shader: %||\n", log);
	}
	
}


void myShowShader(GLuint shader) {
	GLint len = 0;
	glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &len);
	char * buf = new char[len];

	glGetShaderSource(shader, len, nullptr, buf);

	print(buf);

	delete buf;
	
}


void check_sdl() {
	if (SDL_GetError()[0]) {
		print(std::cerr, "Error: SDL: %||\n", SDL_GetError());
		SDL_Quit();
		std::exit(1);		
	}
}



char const* src_vert_shader = R"QWERTY(
	#version 300 es                          

	precision mediump float;

	layout(location = 0) in vec4 vPosition;
	
	void main()                              
	{                                        
	   gl_Position = vPosition;              
	}                                        
)QWERTY";

char const* src_frag_shader = R"QWERTY(
	#version 300 es
	precision mediump float;
	out vec4 fragColor;                          
	void main()                                  
	{                                            
	   fragColor = vec4 ( 1.0, 0.0, 0.0, 0.8 );  
	}                                            
)QWERTY";


void myShowGLInfo() {
	GLint v;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &v);
	print("GL_MAX_VERTEX_UNIFORM_VECTORS = %||\n", v);
	
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &v);
	print("GL_MAX_FRAGMENT_UNIFORM_VECTORS = %||\n", v);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &v);
	print("GL_MAX_VERTEX_ATTRIBS = %||\n", v);
	
	glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &v);
	print("GL_MAX_VERTEX_OUTPUT_COMPONENTS = %||\n", v);
	
	CHECK_GL();
}


void myAttachShaders(GLuint prog, char const* src_vert_shader, char const* src_frag_shader)
{
	auto vs = glCreateShader(GL_VERTEX_SHADER);
	CHECK_GL();
	
	auto fs = glCreateShader(GL_FRAGMENT_SHADER);
	CHECK_GL();	

	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	myLoadShader(vs, src_vert_shader);
	myLoadShader(fs, src_frag_shader);

	// will remain attached to program
	glDeleteShader(vs);
	glDeleteShader(fs);
}



int main() {
	
	SDL_Init(SDL_INIT_VIDEO);
	check_sdl();
	
		
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	check_sdl();
	
		
	SDL_Window * win = SDL_CreateWindow("", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600, 
		SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
	);
	check_sdl();
	
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	check_sdl();
	

	// OpenGL
	{
		init_glew();
		CHECK_GL();

		auto prog = glCreateProgram();
		CHECK_GL();	

		myAttachShaders(prog, src_vert_shader, src_frag_shader);				
		myLinkProgram(prog);

		glUseProgram(prog);
		CHECK_GL();

		// geometry
		{
			// vertex array data
			GLfloat verts[] = {
				 0.1f,  0.6f, 0.0f,
				-0.5f, -0.5f, 0.5f,
				 0.3f, -0.4f, 0.2f
			};

			glViewport (0, 0, 800, 600);

			glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
			glClear(GL_COLOR_BUFFER_BIT);
			CHECK_GL();	

			// vertex array meta-info
			GLuint vao[1];

			glGenVertexArrays(1, vao);
			CHECK_GL();
			
			// create va with buffer
			{
				
				// buffer for vertex array
				GLuint vbo[1];
				glGenBuffers(1, vbo);		
				glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
				CHECK_GL();

				
				glBindVertexArray(vao[0]);
				CHECK_GL();

				// match arguments
				glEnableVertexAttribArray(0);
				CHECK_GL();
				
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				CHECK_GL();

				glBindVertexArray(0);
				
				glDeleteBuffers(1, vbo);
			
			}
			
			glBindVertexArray(vao[0]);
			CHECK_GL();			
			
			bool run = 1;
			while (run) {
				SDL_Event e;
				while (SDL_PollEvent(&e)) {
					if (e.type == SDL_QUIT) {
						run = 0;
					}
				}

				// drawing
				glDrawArrays(GL_TRIANGLES, 0, 3);
				//glDrawArrays(GL_POINTS, 0, 3);
				CHECK_GL();			
				
				SDL_GL_SwapWindow(win);
			}
			
			glDeleteVertexArrays(1, vao);
		}
		
		glDeleteProgram(prog);
		CHECK_GL();		   
	}
		
	SDL_GL_DeleteContext(ctx);
	SDL_DestroyWindow(win);
	SDL_Quit();
	
	return 0;
}

