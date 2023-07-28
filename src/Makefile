CXX=g++
PROTOS_PATH=/data/home/vapourchen/workspace/epaxos_cpp/src
INC_DIR=/data/home/vapourchen/workspace/epaxos_cpp/
LDFLAGS=-I$(INC_DIR) -L$(PROTOS_PATH) -g -m64 -lprotobuf -lpthread -lgtest -std=c++11
#LDFLAGS += -L/usr/local/lib `pkg-config --cflags protobuf`\
           -pthread\
           -ldl

PUBF_LIB = conf.pb.o instance.pb.o 

#$(info $(LDFLAGS))
vpath %.proto $(PROTOS_PATH)
PROTOC = protoc
#pkg-config --cflags --libs protobuf
all: test

pb: *.proto
	protoc --cpp_out=. *.proto

test: instance_test.o  $(PUBF_LIB) 
		$(CXX) $^ $(LDFLAGS) -o $@

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
		$(PROTOC) -I$(PROTOS_PATH) --cpp_out=. $<

#@touch protoc_middleman

clean:
	rm -rf test *.o