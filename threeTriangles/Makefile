CPP = g++
INC = -I../glslutil -I../mvcutil -I.
C_FLAGS = -fPIC -g -c -DGL_GLEXT_PROTOTYPES $(INC)

LINK = g++ -fPIC -g
LOCAL_UTIL_LIBRARIES = -L../lib -lglsl
OGL_LIBRARIES = -lglut -lGLU -lGL

OBJS = threeTriangles.o ModelView.o Controller.o

threeTriangles: $(OBJS) ../lib/libglsl.so
	$(LINK) -o threeTriangles $(OBJS) $(LOCAL_UTIL_LIBRARIES) $(OGL_LIBRARIES)

../lib/libglsl.so : ../glslutil/ShaderIF.h ../glslutil/ShaderIF.c++
	(cd ../glslutil; make)

threeTriangles.o: threeTriangles.c++
	$(CPP) $(C_FLAGS) threeTriangles.c++
ModelView.o: ModelView.h ModelView.c++
	$(CPP) $(C_FLAGS) ModelView.c++
Controller.o: ../mvcutil/Controller.h ../mvcutil/Controller.c++
	$(CPP) $(C_FLAGS) ../mvcutil/Controller.c++
