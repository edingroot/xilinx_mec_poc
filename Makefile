PROGS=mec_http_server 
    #   asyncudptest

all:${PROGS}

SRC=${shell pwd}/src
OUTPUT=${shell pwd}/bin

MAKE_BIN_DIR := ${shell mkdir -p $(OUTPUT) }
MAKE_EXAMPLE_HTTP_BIN_DIR := ${shell mkdir -p $(OUTPUT)/mec_http_server }

CXXFLAGS+=-g -std=c++11
LDFLAGS+=-lboost_system -lboost_thread -lpthread

mec_http_server:${SRC}/mec_http_server/main.o\
    ${SRC}/mec_http_server/http_server.o\
    ${SRC}/mec_http_server/connection.o\
    ${SRC}/mec_http_server/request_handler.o\
    ${SRC}/mec_http_server/request_parser.o\
    ${SRC}/mec_http_server/reply.o\
    ${SRC}/mec_http_server/connection_manager.o\
    ${SRC}/mec_http_server/mime_types.o
	@${CXX} ${CXXFLAGS}  -o ${OUTPUT}/mec_http_server/$@   $^  ${LDFLAGS}

clean:
	@rm -rf ${OUTPUT} ${SRC}/*.o\
	    ${SRC}/mec_http_server/*.o


.PHONY: all clean
