CPP=g++
GLM_INC1=../External/glm
GLM_INC2=../../External/glm
GLM_INC3=../../../External/glm
LUA_INC=/usr/include/lua5.2 
INCDIRS=-I${LUA_INC} -I${GLM_INC1} -I${GLM_INC2} -I${GLM_INC3}
CPPFLAGS=-DGLM_SWIZZLE -DOPENWAR_USE_GLEW -DOPENWAR_USE_SDL -Dnullptr=0 -O0 -g3 -Wall -fmessage-length=0 -std=c++0x ${INCDIRS} 
LDFLAGS=-lGL -lGLEW -lopenal -lSDL2 -lSDL2_image -llua5.2
SOURCES=$(shell for file in `find . -name \*.cpp`;do echo $$file; done)
OBJECTS=$(SOURCES:.cpp=.o)
EXEC=main

all: $(OBJECTS)
	@$(CPP) -o $(EXEC) $(CPPFLAGS) $(OBJECTS) $(LDFLAGS)

compile: $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(OBJECTS) 

%.o: %.cpp
	@$(CPP) $(CPPFLAGS) $< -o $@

clean: 
	find . -name \*.o -exec rm {} \;
	$(RM) $(EXEC)
