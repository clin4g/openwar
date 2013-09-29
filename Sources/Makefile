CXX=g++
GLM_INC1=../External/glm
GLM_INC2=../../External/glm
GLM_INC3=../../../External/glm
LUA_INC=/usr/include/lua5.2 
INCDIRS=-I${LUA_INC} -I${GLM_INC1} -I${GLM_INC2} -I${GLM_INC3}
CPPFLAGS=-DGLM_SWIZZLE -DOPENWAR_USE_GLEW -DOPENWAR_USE_SDL -Dnullptr=0 -O0 -g3 -Wall -fmessage-length=0 -std=c++0x ${INCDIRS} -c
SOURCES=$(shell for file in `find . -name \*.cpp`;do echo $$file; done)
OBJECTS=$(SOURCES:.cpp=.o)

all: $(OBJECTS)
	@$(CXX) $(CPPFLAGS) $(OBJECTS) 

%.o: %.cpp
	@$(CXX) $(CPPFLAGS) $< -o $@

clean: 
	find . -name \*.o -exec rm {} \;
