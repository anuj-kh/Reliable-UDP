# Reliable-UDP
## Designed and implementated a reliable UDP protocol in C++
The problem statement can be found [here](./assignment-2.pdf)<br/>
The protocol specification can be found [here](./Computer%20Networks%20A2%20Phase1.pdf)<br/>
The performance plots for packet loss, packet delay, packet reorder and packet corruption tested through netem can be found [here](./Computer%20Networks%20Project%20(Performance%20Plots).pdf)

The [protocol.h](./protocol.h) file contains the protocol code.<br/>
The [server.cpp](./server.cpp) file contains the server code which is built upon this protocol.<br/>
The [client.cpp](./client.cpp) file contains the client code which is built upon this protocol.