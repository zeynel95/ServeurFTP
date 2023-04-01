On this TP we have worked on implementing a server/client infrastructure.

The server offers NB_PROCS processes open to communicate with clients. 
Each client is then linked to one process(serveur) and starts the communication.

## Retrieving files
Suppose the cache is clean, and we initiate first contact.
Clients can send the name of the file to the serveur to retrieve it.
This works with a protocol we have put in place which works like this.
// For the visual representation, look at Comms_noProblem.jpg

First of all we check if the file we request exists.
At this point I need to specify that the files are saved on a directory named filesServeur hardcoded.
If the file requested isn't on ./filesServeur/ than nothing is done, we wait for another communication 
from the part of the client.
The client can send multiple requests to a server. The client can communicate as long as it doesn't send 
a bye message. This is considered a stop communication and the client is stopped.

So the client sends a filename, and receives a response from the server.
The response is either 0(doesn't exist) or 1(exists). If we receive 0 we restart our principle while loop.

If we got a 1(file exists) than we pass to the next steps.
The server then send messages back to the client 2 by 2. The first one indicates the number of bytes that 
the client should expect to write, then the bytes. This is done so that we send the file by chunks or packages.
Every 2 messages is a chunk. 
The client in this case receives the first message, receives the second message and then downloads this package.
At the same time its the client that creates a .cache file in case something wrong happens from the part of the 
server or the client. If we stop the client or the server at any point, then .cache helps us keep track.

The server once sent all the packages of the file, it sends a last -1 signal to indicate a 'success'. The client then 
knows that the file is over and to restart at the top of our while() loop.

## Error handling
Error handling is done at several places.
(Server)Once to check if the file requested from the client exists on the server, if it doesn't we send 0.
(Client)Can't request a file which it already has on its own directory
(Client)If whenever we receive a 0 message from the server, we know the server is dead. The only appropriate messages 
from the server should be a number bigger then 0, the bytes or -1 for 'success'. At every message we receive, if its a 0 then 
we stop and close the client.

## Caching system
Every time we receive a package from the server, we keep track of it in .cache file. We save the name and the number of the 
current package. If we have a semi-downloaded file, it is either because the client or the server stopped. At relaunch we look 
at the cache and check if do need to resume what we were download before. Have a look at Comms_withProblem.jpg.

If the client has no cache, or the cache sais sucess, than we send 0 to the server, which then does nothing and waits for 
the name of the file as usual. In case were we receive something different from 0, than it must be a number X. The server then 
receives the name of the file semi-downloaded. It starts then to send 2 by 2 with the same protocol from before, the packages 
only this time skipping X packages downloaded. 