CPP = g++
INC = -I../glslutil -I../mvcutil -I.
C_FLAGS = -fPIC -g -c -DGL_GLEXT_PROTOTYPES $(INC)

LINK = g++ -fPIC -g
LOCAL_UTIL_LIBRARIES = -L../lib -lglsl
OGL_LIBRARIES = -lglut -lGLU -lGL

OBJS = twoTriangles.o ModelView.o Controller.o

twoTriangles: $(OBJS) ../lib/libglsl.so
	$(LINK) -o twoTriangles $(OBJS) $(LOCAL_UTIL_LIBRARIES) $(OGL_LIBRARIES)

../lib/libglsl.so: ../glslutil/ShaderIF.h ../glslutil/ShaderIF.c++
	(cd ../glslutil; make)

twoTriangles.o: twoTriangles.c++
	$(CPP) $(C_FLAGS) twoTriangles.c++
ModelView.o: ModelView.h ModelView.c++
	$(CPP) $(C_FLAGS) ModelView.c++
Controller.o: ../mvcutil/Controller.h ../mvcutil/Controller.c++
	$(CPP) $(C_FLAGS) ../mvcutil/Controller.c++
