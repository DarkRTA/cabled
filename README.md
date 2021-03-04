# cabled - a no bullshit connnection manager for jack

This is a really simple connection manager for jack designed to automatically
connect/disconnect ports specified in a config file.


# Usage
Command line usage:
```
./cabled <config file>
```
Config file example:

```
[connect]
# the output port comes first, followed by the input port
clienta:port_0 = client_b:port_0
[disconnect]
clienta:port_1 = client_b:port_1
```

# TODO

* Avoid trying to reconnect every single port every time a new port is registered.
* Avoid disconnect/connect loops by not disconnecting anything we've already connected.
