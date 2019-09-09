# Xilinx MEC PoC Server

## Server Streaming URI

```
http://{your ip address}:{port}/udp_streaming/init
```

## Request Body Example
```
{
    "dl_udp_port": 1234
}
```

## Response Body Example
```
{
    ul_udp_port: 2345,
    ul_udp_timeout: 120
}
```

