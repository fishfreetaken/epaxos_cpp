CXX=g++ 
FLAG=-I./

CFLAGS = -g -Wall -std=c++17 -I./src -I./include \
                -I./ \

TARGET=test
#DIRS := $(shell find . -maxdepth 3 -type d)
DIRS= src remote .

FILES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))

OBJS = $(patsubst %.cpp,%.o, $(FILES))

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $@

.c.o:
	$(CXX) -c  $(CFLAGS)  $< -o $@

LIBS = -lpthread -lm
$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(LIBS) $(OBJS)  -o $@ 
#SRC=$(wildcard *.cpp)

#include remote/submakefile.mk 
#include src/submakefile.mk

#OBJ=$(SRC:.cpp=.o)
#app.executable: $(OBJ)
#	CXX $^ $(FLAG) -o $@

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)