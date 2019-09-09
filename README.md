# Xilinx MEC PoC Server

## Build and Run (Run on the port 9999)

```bash
make
./bin/example_http/example_http_server 0.0.0.0 9999 .
```

## Http Server Usage
```
Usage: http_server <address> <port> <doc_root>
  For IPv4, try:
    example_http_server 0.0.0.0 80 .
  For IPv6, try:
    example_http_server 0::0 80 .
```

## Clean *.o and binary
```
make clean
```


## Codes of Boost.ASIO leaning

## References
- [boost.org](http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio.html)


