#ifndef GL92847255573_H
#define GL92847255573_H


namespace gl {

	class Critical: public std::runtime_error {
		public:
			explicit Critical (const std::string& what_arg): std::runtime_error(what_arg) {}
			explicit Critical (const char* what_arg): std::runtime_error(what_arg) {}
	};

	class Error: public std::runtime_error {
		public:
			explicit Error (const std::string& what_arg): std::runtime_error(what_arg) {}
			explicit Error (const char* what_arg): std::runtime_error(what_arg) {}
	};

}

namespace gl {

	using Float = GLfloat;
	using Int = GLint;
	using Uint = GLuint;
	using Enum = GLenum;
	using Boolean = GLboolean;
	using Sizei = GLsizei;
	using Void = GLvoid;
	using Bitfield = GLbitfield;

	struct Shader{
		GLuint id{0};

		~Shader() {
			if (id > 0) {
				assert(glIsShader(id) == GL_TRUE);
				glDeleteShader(id);
				id = 0;
			}
		}
	};

	struct Program{
		GLuint id{0};

		~Program() {
			if (id > 0) {
				assert(glIsProgram(id) == GL_TRUE);
				glDeleteProgram(id);
				id = 0;
			}
		}
	};

	struct Buffer{
		GLuint id{0};

		~Buffer() {
			if (id > 0) {
				//assert(glIsProgram(id) == GL_TRUE);
				glDeleteBuffers(1, &id);
				id = 0;
			}
		}

	};

	void bind_buffer(Enum type, Buffer const& b) {
		glBindBuffer(type, b.id);
	}

	void bind_buffer(Enum type) {
		glBindBuffer(type, 0);
	}

	void buffer_data(Enum target, Sizei size, void const* data, Enum usage) {
		glBufferData(target, size, data, usage);
	}

	Buffer create_buffer() {
		Buffer b;
		glGenBuffers(1, &b.id);
		return b;
	}

	template <class T>
	Buffer create_buffer(Enum type, std::vector<T> const& ts, Enum usage) {
		auto b = create_buffer();
		bind_buffer(type, b);
		buffer_data(type, ts.size() * sizeof(T), &ts[0], usage);
		bind_buffer(type);
		return b;
	}


	struct Texture {
		Uint id{0};

		~Texture() {
			if (id) {
				glDeleteTextures(1, &id);
				id = 0;
			}
		}
	};

	void bind_texture(Enum target, Texture const& t) {
		glBindTexture(target, t.id);
	}

	void bind_texture(Enum target) {
		glBindTexture(target, 0);
	}

	void tex_parameter(Enum target, Enum pname, Int param) {
		glTexParameteri(target, pname, param);
	}

	Texture create_texture() {
		Texture t;
		glGenTextures(1, &t.id);
		return t;
	}

	void active_texture(Enum texture) {
		 glActiveTexture(texture);
	}


	void tex_image_2D(
		Enum target, Int level, Int internalFormat,
		Sizei width, Sizei height, Int border,
		Enum format, Enum type, void const* data
	) {
		glTexImage2D(
			target, level, internalFormat,
			width, height, border,
			format, type, data
		);
	}



	//void buffer_data(Buffer b, Sizei size, void const* data, Enum usage) {
	//	glNamedBufferData(b.id, size, data, usage);
	//}



	GLint get_shader_iv(Shader const& s, GLenum param_id) {
		GLint tmp;
		glGetShaderiv(s.id, param_id, &tmp);
		return tmp;
	}

	Int get_buffer_parameter_iv(Enum type, Enum param_id) {
		Int tmp;
		glGetBufferParameteriv(type, param_id, &tmp);
		return tmp;
	}



	GLint get_program_iv(Program const& s, GLenum param_id) {
		GLint tmp;
		glGetProgramiv(s.id, param_id, &tmp);
		return tmp;
	}


	std::string get_program_infolog(Program const& t)
	{
		assert(glIsProgram(t.id));

		Int len = get_program_iv(t, GL_INFO_LOG_LENGTH);

		std::vector<char> buffer;
		buffer.resize(len);

		glGetProgramInfoLog(t.id, len, nullptr, &buffer[0]);

		return std::string(&buffer[0]);
	}

	std::string get_shader_infolog(Shader const& t)
	{
		assert(glIsShader(t.id));

		Int len = get_shader_iv(t, GL_INFO_LOG_LENGTH);

		std::vector<char> buffer;
		buffer.resize(len);

		glGetShaderInfoLog(t.id, len, nullptr, &buffer[0]);

		return std::string(&buffer[0]);
	}

	Shader create_shader(GLenum type, std::string const& src) {
		Shader s;
		s.id = glCreateShader(type);

		char const* c_src = src.c_str();
		glShaderSource(s.id, 1, &c_src, nullptr);
		glCompileShader(s.id);

		GLint compile_ok = GL_FALSE,
		link_ok = GL_FALSE;

		if (get_shader_iv(s, GL_COMPILE_STATUS) == 0) {
			std::cerr << get_shader_infolog(s);
			throw Critical("compile shader error");
		}

		return s;
	}



	Program create_program() {
		Program p;
		p.id = glCreateProgram();
		return p;
	}



	void attach_shader(Program const& p, Shader const& s) {
		glAttachShader(p.id, s.id);
	}

	void link_program(Program const& p) {
		glLinkProgram(p.id);
	}



	Program create_program(Shader const& vert, Shader const& frag) {
		auto p = create_program();
		attach_shader(p, vert);
		attach_shader(p, frag);
		link_program(p);
		if (!get_program_iv(p, GL_LINK_STATUS)) {
			std::cerr << get_program_infolog(p);
			throw Critical("link program error");
		}
		return p;
	}

	struct Attrib {
		GLint id;
	};

	Attrib get_attrib_location(Program const& p, std::string name) {
		Attrib al;
		al.id = glGetAttribLocation(p.id, name.c_str());
		if (al.id == -1) {
			throw Critical("cannot get attribute: " + name);
		}
		return al;
	}

	struct Uniform {
		GLint id;
	};

	Uniform get_uniform_location(Program const& p, std::string name) {
		Uniform y;
		y.id = glGetUniformLocation(p.id, name.c_str());
		if (y.id == -1) {
			throw Critical("cannot get attribute: " + name);
		}
		return y;
	}

	void use_program(Program const& p) {
		glUseProgram(p.id);
	}

	void enable_vertex_attrib_array(Attrib const& al) {
		glEnableVertexAttribArray(al.id);
	}


	void disable_vertex_attrib_array(Attrib const& al) {
		glDisableVertexAttribArray(al.id);
	}


	void vertex_attrib_pointer(Attrib al, Int size, Enum type, Boolean normalized, Sizei stride=0, Void const* pointer=0)
	{
		glVertexAttribPointer(
			al.id,
			size,
			type,
			normalized,
			stride,
			pointer
		);
	}


	void depth_func(Enum what) {
		glDepthFunc(what);
	}

	void draw_arrays(Enum mode, Int first, Sizei count) {
		glDrawArrays(mode, first, count);
	}

	void draw_elements(Enum mode, Sizei count, Enum type, void const* indices) {
		glDrawElements(mode, count, type, indices);
	}


	void clear(Bitfield mask) {
		glClear(mask);
	}

	void enable(Enum cap) {
		glEnable(cap);
	}

	void blend_func(Enum sfactor, Enum dfactor) {
		glBlendFunc(sfactor, dfactor);
	}

	void uniform(Uniform u, Int v1) {
		glUniform1i(u.id, v1);
	}

	void uniform(Uniform u, Float v1) {
		glUniform1f(u.id, v1);
	}

	void uniform(Uniform u, Float v1, Float v2) {
		glUniform2f(u.id, v1, v2);
	}

	void uniform(Uniform u, Float v1, Float v2, Float v3) {
		glUniform3f(u.id, v1, v2, v3);
	}

	void uniform(Uniform u, Float v1, Float v2, Float v3, Float v4) {
		glUniform4f(u.id, v1, v2, v3, v4);
	}


	void uniform_matrix4f(Uniform u, Sizei count, Boolean transpose, Float const* value) {
		glUniformMatrix4fv(u.id, count, transpose, value);
	}

	void uniform_matrix4f(Uniform u, Float const* value, Boolean transpose=GL_FALSE) {
		uniform_matrix4f(u, 1, transpose, value);
	}

	void viewport(Int x, Int y, Sizei width, Sizei height) {
		glViewport(x, y, width, height);
	}


} // namespace gl

#endif
