PROGS=mec_http_server 
    #   asyncudptest

# all:${PROGS}

dpu: mec_http_server_dpu

caff: mec_http_server_caff

SRC=${shell pwd}/src
OUTPUT=${shell pwd}/bin

MAKE_BIN_DIR := ${shell mkdir -p $(OUTPUT) }
MAKE_EXAMPLE_HTTP_BIN_DIR := ${shell mkdir -p $(OUTPUT)/mec_http_server }

CXXFLAGS+=-g -std=c++11
LDFLAGS+=-lboost_system -lboost_thread -lpthread `pkg-config --cflags opencv` `pkg-config --libs opencv` 

# mec_http_server:${SRC}/mec_http_server/main.o\
#     ${SRC}/mec_http_server/http_server.o\
#     ${SRC}/mec_http_server/connection.o\
#     ${SRC}/mec_http_server/request_handler.o\
#     ${SRC}/mec_http_server/request_parser.o\
#     ${SRC}/mec_http_server/reply.o\
#     ${SRC}/mec_http_server/connection_manager.o\
#     ${SRC}/mec_http_server/mime_types.o
# 	@${CXX} ${CXXFLAGS}  -o ${OUTPUT}/mec_http_server/$@   $^  ${LDFLAGS}

image_classifier_dpu.o: ${SRC}/mec_http_server/image_classifier_dpu.cpp ${SRC}/mec_http_server/image_classifier.hpp\
    ${CXX} ${CXXFLAGS}  -o ${OUTPUT}/mec_http_server/$@   $^  ${LDFLAGS}

image_classifier_caff.o: ${SRC}/mec_http_server/image_classifier_caff.cpp ${SRC}/mec_http_server/image_classifier.hpp\
    ${CXX} ${CXXFLAGS}  -o ${OUTPUT}/mec_http_server/$@   $^  ${LDFLAGS}

mec_http_server_dpu:${SRC}/mec_http_server/main.o\
    ${SRC}/mec_http_server/http_server.o\
    ${SRC}/mec_http_server/connection.o\
    ${SRC}/mec_http_server/request_handler.o\
    ${SRC}/mec_http_server/request_parser.o\
    ${SRC}/mec_http_server/reply.o\
    ${SRC}/mec_http_server/connection_manager.o\
    ${SRC}/mec_http_server/mime_types.o\
    ${SRC}/mec_http_server/image_classifier_dpu.o
	@${CXX} ${CXXFLAGS}  -o ${OUTPUT}/mec_http_server/$@   $^  ${LDFLAGS}

mec_http_server_caff:${SRC}/mec_http_server/main.o\
    ${SRC}/mec_http_server/http_server.o\
    ${SRC}/mec_http_server/connection.o\
    ${SRC}/mec_http_server/request_handler.o\
    ${SRC}/mec_http_server/request_parser.o\
    ${SRC}/mec_http_server/reply.o\
    ${SRC}/mec_http_server/connection_manager.o\
    ${SRC}/mec_http_server/mime_types.o\
    ${SRC}/mec_http_server/image_classifier_caff.o
	@${CXX} ${CXXFLAGS}  -o ${OUTPUT}/mec_http_server/$@   $^  ${LDFLAGS}

clean:
	@rm -rf ${OUTPUT} ${SRC}/*.o\
	    ${SRC}/mec_http_server/*.o

.PHONY: all clean dpu caff
