CXX=g++ 
FLAG=-I./

CFLAGS = -g -Wall -std=c++17 -I./src -I./include \
                -I./ \

TARGET=test
#DIRS := $(shell find . -maxdepth 3 -type d)
DIRS= src remote .

FILES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
HFILES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.h))

OBJS = $(patsubst %.cpp,%.o, $(FILES))

#all_o_depds = $(patsubst %.o, %.o.d, $(FILES))

LOG = loc

#-include $(all_o_depds)

LIBS = -lpthread -lm
$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(LIBS) $(OBJS)  -o $@ 

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $@

.c.o:
	$(CXX) -c  $(CFLAGS)  $< -o $@



#SRC=$(wildcard *.cpp)

#include remote/submakefile.mk 
#include src/submakefile.mk

#OBJ=$(SRC:.cpp=.o)
#app.executable: $(OBJ)
#	CXX $^ $(FLAG) -o $@

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)
	$(RM) $(LOG)