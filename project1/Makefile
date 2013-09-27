CPP = g++
INC = -I../glslutil -I../mvcutil -I./CSVReader -I.
C_FLAGS = -fPIC -g -c -DGL_GLEXT_PROTOTYPES $(INC)

LINK = g++ -fPIC -g
LOCAL_UTIL_LIBRARIES = -L../lib -lglsl
OGL_LIBRARIES = -lglut -lGLU -lGL

OBJS = project1.o ModelView.o Controller.o CSVReader.o

project1: $(OBJS) ../lib/libglsl.so
	$(LINK) -o project1 $(OBJS) $(LOCAL_UTIL_LIBRARIES) $(OGL_LIBRARIES)

../lib/libglsl.so: ../glslutil/ShaderIF.h ../glslutil/ShaderIF.c++
	(cd ../glslutil; make)

project1.o: project1.c++
	$(CPP) $(C_FLAGS) project1.c++
ModelView.o: ModelView.h ModelView.c++
	$(CPP) $(C_FLAGS) ModelView.c++
Controller.o: ../mvcutil/Controller.h ../mvcutil/Controller.c++
	$(CPP) $(C_FLAGS) ../mvcutil/Controller.c++
CSVReader.o: ./CSVReader/CSVReader.h ./CSVReader/CSVReader.c++
	$(CPP) $(C_FLAGS) ./CSVReader/CSVReader.c++
clean:
	rm -rf *~ *.o project1 \#*
