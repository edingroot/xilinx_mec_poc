# API Specification

### 1. Initialize Frame Streaming
#### Request URI for init UDP socket
```
http://{server ip address}:{port}/udp_streaming/init
```

#### Request body sample
```json
{
    "dl_udp_port": 1234
}
```

#### Response body sample
```json
{
    "ul_udp_port": 2345,
    "ul_udp_timeout": 120
}
```

#### UDP streeaming response body
```json
{
    "rawdata" : "OK No Problem\n"
}
```

#### Request URI for terminating UDP socket (TODO)
```
http://{server ip address}:{port}/udp_streaming/terminate
```

