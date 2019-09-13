# Xilinx MEC PoC Server

## Build MEC HTTP Server and Run (Run on the port 9999)
### For General Device (Ubuntu on PC or Labtop)
```bash
make -j3 #j3 means using 3 threads to compile
./bin/mec_http_server/mec_http_server_general 0.0.0.0 9999 .
```
### For Ultra 96 FPGA Board
`Please replace the demo.hpp in the right place in U96_data folder`
```bash
make DPU -j3 
./bin/mec_http_server/mec_http_server_general 0.0.0.0 9999 .
```
### For Caffe
```bash
make CAFF -j3 
./bin/mec_http_server/mec_http_server_general 0.0.0.0 9999 .
```

## MEC HTTP Server Usage
```
Usage: http_server <address> <port> <doc_root>
  For IPv4, try:
    mec_http_server 0.0.0.0 80 .
  For IPv6, try:
    mec_http_server 0::0 80 .
```

## Clean MEC HTTP Server *.o and Binary
```bash
make clean
```
---
## Build MEC Test Client and Run (OpenCV Required)
```bash
cd mec_test_client
cmake .
make
./client 0.0.0.0 10000
```

## MEC Test Client Usage
```
Usage: client <address> <port>
```

## Clean MEC Test Client
```bash
make clean
rm -rf Makefile CMakeCache.txt cmake_install.cmake CMakeFiles
```
---
## Codes of Boost.ASIO leaning

## References
- [boost.org](http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio.html)


