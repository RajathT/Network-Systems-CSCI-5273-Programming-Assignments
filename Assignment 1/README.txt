**********README*********************
>>get filename
-->Here the command will request the server to send the files. Here I have used the system of ACK, only when the packet is ACK'd by the client or server the next file is sent. If any packet loss the packets are re transmitted
>>put filename
-->The client will put this in the server, here the basic logic used is that the packets are given a sequence number and the the packets are sent, at the server side the packets are then reordered.
>>delete filename
-->This commad will delete the mentioned filename at the server's end
>>ls
-->This command will list all the files at the server directory. This will request the server to send the files to the client
>>exit
-->Exits the server using exit(0)

Commands to enter:
The functionality of the server and client works perfectly fine, but the commands to be enetered are as follows

>>get(space)filename
>>put(space)filename
>>delete(space)filename
>>ls(space)files
>>exit(space)server

If server exits then please run it again, all functions work perfectly fine when the commands to the server are sent as soon as it is complied and run.


