# API Specification

### 1. Initialize Frame Streaming
#### Request URI
```
http://{server ip address}:{port}/udp_streaming/init
```

#### Request body
```
{
    "dl_udp_port": 1234
}
```

#### Response body sample
```
{
    ul_udp_port: 2345,
    ul_udp_timeout: 120
}
```

