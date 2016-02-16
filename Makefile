# Simple Makefile
# $@ -- The file name of the target of the rule.
# $* -- The stem with which an implicit rule matches
# $< -- The name of the first prerequisite.
# $^ -- The names of all the prerequisites
# $(VARS:%.cpp=%.o) -- pattern replace

CC:=clang++
#CC:=emcc

# output files
OUTS:=main test

# em opts
EMOPTS:=
EMOPTS+=-s USE_SDL=2
EMOPTS+=-s DISABLE_EXCEPTION_CATCHING=0
#EMOPTS+=-s SAFE_HEAP=1
EMOPTS+=-s ASSERTIONS=1
#EMOPTS+=-s DEMANGLE_SUPPORT=1
#EMOPTS+=-s EXCEPTION_DEBUG=1

# compiler options
CCOPTS:=
CCOPTS+=-std=c++14
CCOPTS+=-I./inc 
#CCOPTS+=$(shell freetype-config --cflags)
CCOPTS+=-Wsign-compare -Wreturn-type -Wparentheses -Wpedantic -Wconversion-null
CCOPTS+=-ferror-limit=3
CCOPTS+=-Wno-vla-extension

# linker options
LLOPTS:=
#LLOPTS+=$(shell freetype-config --libs)
LLOPTS+=-lGL -lGLEW 
LLOPTS+=-L./lib -lSDL2

# common options
CCOPTS+=-O0 -g
LLOPTS+=-O0 -g 

# environment specific
ifeq (${CC}, emcc)
	# emcc
	CCOPTS+=${EMOPTS}
	LLOPTS+=${EMOPTS}
	OUT_EXT:=.html
	#LLOPTS+=--preload-file res
else
	# clang
	OUT_EXT:=
endif


default: ${OUTS}


# assert dirs
$(shell mkdir -p build)
$(shell find src/ -type d | cut -c 5- | xargs -I{} mkdir -p build/{})

# list of compiled source build/obj/fname.cpp.obj
OBJS:=$(shell find src -name '*.cpp')
OBJS:=$(OBJS:src/%.cpp=build/%.cpp.obj)

-include $(OBJS:%.obj=%.d)


# compiler
${OBJS}: build/%.obj: src/%
	${CC} -c -MMD -MP -o $@ $< ${CCOPTS}

# linker
${OUTS}: $(OBJS)
	${CC} -o build/$@${OUT_EXT} build/$@.cpp.obj  $(filter-out $(OUTS:%=build/%.cpp.obj),$(OBJS)) ${LLOPTS}

clean:
	rm -rf build/*
