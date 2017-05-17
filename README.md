### CPSC 526 - Assignment 2 - Port Forwarding and Logging Proxy Server

##Authors:

Tyrone Lagore
James MacIsaac 	

##Desc:

This is the readme document for the proxy program.
The program is an implementation of a proxy server that listens for client
connections on a given port and forwards traffic to a target destination.
The program the returns all output from the target back to the client, unaltered.
It serves as a base to perform a MITM (Man-In-The-Middle) attack.

##Running the program:

Once the program is compiled and linked (using the makefile), the startup command is as follows...
from within the project folder...

     ./bin/proxy [log opt] <listening port> <target host> <target port>

where <> is required and [] is optional as arguments to start the program.
      -The program resolves target hostnames to IP addresses.
      -Options for the [log opt] proxy logging argument are:
      
      	       `<no log option> : no logging will be done.
	       	       	       	 Data still gets forwarded between the client and target.
				 
      	       -raw : 		 Data is logged as is.
	       
	       -strip : 	 Like raw but only printable characters are printed;
	       	      		 Everything else is replaced with '.'

	       -hex : 		 Logging similar to the 'hexdump -C' command.

               -autoN : 	 Data segments are divided into 'N'-byte long chunks,
	       	        	 where each chunk will be displayed seperately on its own line.
				 It also prints the escaped version of '\\' '\t' '\n' and '\r'
				 Respectively. Each byte not in the printable range of ascii
				 will be displayed in the form '\XX' where 'XX' is the
				 2-digit hex value of the byte.`

##Testing The Program:

The program works for connections using TCP including http, ssh, netcat,
and the backdoor program from asg1.

Concurrency testing can use the simple program we included in the submission
called the multi chat server. usage is as follows:

       ./bin/multi_chat_server <port>

This will open a connection which can interface with the proxy, and will send
and receive messages. We tested with 3 of these connecting to the proxy and
all would receive and send messages properly.

##Data Flow indicators:

Incoming Data (Client -> Proxy -> Target) lines use prefix: '--->'
Outgoing Data (Target -> Proxy -> Client) lines use prefix: '<---'

##Sample output for various formats:

All connections are running a (netcat <-> proxy <-> asg1 backdoor) configuration
All components running on localhost

#<LOGGING OFF>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| => ./bin/proxy 8888 localhost 9999
!! 'localhost' is '127.0.0.1'
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Proxy Server Active
!! ~~~~~~~~~~~~~~~~~~~
!! config:
!! listening on port: 	8888
!! dst ip: 		127.0.0.1
!! dst port: 		9999
!! logs: 		off
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Accepting client connections...
!! src ip: 127.0.0.1
!! [20272] ~ Client Connected - Sun Feb 12 20:57:41 2017

!! [20273] ~ Client listener exitting, sending interrupt to sister threads
!! Worker thread exited successfully

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#RAW

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| => ./bin/proxy -raw 8888 localhost 9999
!! 'localhost' is '127.0.0.1'
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Proxy Server Active
!! ~~~~~~~~~~~~~~~~~~~
!! config:
!! listening on port: 	8888
!! dst ip: 		127.0.0.1
!! dst port: 		9999
!! logs: 		raw
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Accepting client connections...
!! src ip: 127.0.0.1
!! [16316] ~ Client Connected - Sun Feb 12 20:47:29 2017

<--- 
<--- -----------------------------------------------
<--- !! Server Connected
<--- 
<--- -----------------------------------------------
<--- 
<--- @[/home/ugc/james.macisaac/Desktop/cpsc_526_assignment1]
<--- >> 
---> ls
<--- -----------------------------------------------
<--- bin
<--- makefile
<--- README.md
<--- src
<--- 
<--- -----------------------------------------------
<--- 
<--- @[/home/ugc/james.macisaac/Desktop/cpsc_526_assignment1]
<--- >> 
!! [16317] ~ Client listener exitting, sending interrupt to sister threads
!! Worker thread exited successfully

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#STRIP

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| => ./bin/proxy -strip 8888 localhost 9999
!! 'localhost' is '127.0.0.1'
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Proxy Server Active
!! ~~~~~~~~~~~~~~~~~~~
!! config:
!! listening on port: 	8888
!! dst ip: 		127.0.0.1
!! dst port: 		9999
!! logs: 		strip
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Accepting client connections...
!! src ip: 127.0.0.1
!! [18462] ~ Client Connected - Sun Feb 12 20:53:01 2017

<--- 
<--- -----------------------------------------------
<--- !! Server Connected
<--- 
<--- -----------------------------------------------
<--- 
<--- @[/home/ugc/james.macisaac/Desktop/cpsc_526_assignment1]
<--- >> 
---> ls
<--- -----------------------------------------------
<--- bin
<--- makefile
<--- README.md
<--- src
<--- 
<--- -----------------------------------------------
<--- 
<--- @[/home/ugc/james.macisaac/Desktop/cpsc_526_assignment1]
<--- >> 
!! [18463] ~ Client listener exitting, sending interrupt to sister threads
!! Worker thread exited successfully

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#HEX

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| => ./bin/proxy -hex 8888 localhost 9999
!! 'localhost' is '127.0.0.1'
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Proxy Server Active
!! ~~~~~~~~~~~~~~~~~~~
!! config:
!! listening on port: 	8888
!! dst ip: 		127.0.0.1
!! dst port: 		9999
!! logs: 		hex
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Accepting client connections...
!! src ip: 127.0.0.1
!! [19188] ~ Client Connected - Sun Feb 12 20:54:53 2017


<--- d299e700   0A                                               |                   |
<--- d299e700   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D |  ---------------  |
<--- d299e710   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | ----------------  |
<--- d299e720   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 0A | ----------------  |
<--- d299e730   21 21 20 53 65 72 76 65  72 20 43 6F 6E 6E 65 63 |  !! Server Conne  |
<--- d299e740   74 65 64 0A 0A 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | cted  ----------  |
<--- d299e750   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | ----------------  |
<--- d299e760   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | ----------------  |
<--- d299e770   2D 2D 2D 2D 0A 00 00 40  5B 2F 68 6F 6D 65 2F 75 | -----   @[/home/  |
<--- d299e780   67 63 2F 6A 61 6D 65 73  2E 6D 61 63 69 73 61 61 | ugc/james.macisa  |
<--- d299e790   63 2F 44 65 73 6B 74 6F  70 2F 63 70 73 63 5F 35 | ac/Desktop/cpsc_  |
<--- d299e7a0   32 36 5F 61 73 73 69 67  6E 6D 65 6E 74 31 5D 0A | 526_assignment1]  |
<--- d299e7b0   00 3E 3E 20 00                                   |                   |

---> d319f700   6C                                               |                   |
---> d319f700   73 0A                                            |                   |

<--- d299e700   2D                                               |                   |
<--- d299e700   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | ----------------  |
<--- d299e710   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | ----------------  |
<--- d299e720   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 0A 00 | ---------------   |
<--- d299e730                                                    |                   |

<--- d299e700   62                                               |                   |
<--- d299e700   69 6E 0A 00 6D 61 6B 65  66 69 6C 65 0A 00 52 45 | bin  makefile  R  |
<--- d299e710   41 44 4D 45 2E 6D 64 0A  00 73 72 63 0A          |                   |

<--- d299e700   0A                                               |                   |
<--- d299e700   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D |  ---------------  |
<--- d299e710   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 2D | ----------------  |
<--- d299e720   2D 2D 2D 2D 2D 2D 2D 2D  2D 2D 2D 2D 2D 2D 2D 0A | ----------------  |
<--- d299e730   00 00                                            |                   |

<--- d299e700   40                                               |                   |
<--- d299e700   5B 2F 68 6F 6D 65 2F 75  67 63 2F 6A 61 6D 65 73 | @[/home/ugc/jame  |
<--- d299e710   2E 6D 61 63 69 73 61 61  63 2F 44 65 73 6B 74 6F | s.macisaac/Deskt  |
<--- d299e720   70 2F 63 70 73 63 5F 35  32 36 5F 61 73 73 69 67 | op/cpsc_526_assi  |
<--- d299e730   6E 6D 65 6E 74 31 5D 0A  00 3E 3E 20 00          |                   |
!! [19189] ~ Client listener exitting, sending interrupt to sister threads
!! Worker thread exited successfully

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#AUTON (N = 16 in this case)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| => ./bin/proxy -auto16 8888 localhost 9999
!! 'localhost' is '127.0.0.1'
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Proxy Server Active
!! ~~~~~~~~~~~~~~~~~~~
!! config:
!! listening on port: 	8888
!! dst ip: 		127.0.0.1
!! dst port: 		9999
!! logs: 		auto16
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! Accepting client connections...
!! src ip: 127.0.0.1
!! [19805] ~ Client Connected - Sun Feb 12 20:56:29 2017

<--- Bytes 0-16       \n  -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 16-32      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 32-48      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 48-64      \n  !   !       S   e   r   v   e   r       C   o   n   n   e   
<--- Bytes 64-69      c   t   e   d   \n  
<--- Bytes 0-16       \n  -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 16-32      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 32-48      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 48-64      \n  \00 \00 @   [   /   h   o   m   e   /   u   g   c   /   j   
<--- Bytes 64-80      a   m   e   s   .   m   a   c   i   s   a   a   c   /   D   e   
<--- Bytes 80-96      s   k   t   o   p   /   c   p   s   c   _   5   2   6   _   a   
<--- Bytes 96-112     s   s   i   g   n   m   e   n   t   1   ]   \n  \00 >   >       
<--- Bytes 112-113    \00 
---> Bytes 0-3        l   s   \n  
<--- Bytes 0-16       -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 16-32      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 32-48      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   \n  
<--- Bytes 48-49      \00 
<--- Bytes 0-16       b   i   n   \n  \00 m   a   k   e   f   i   l   e   \n  \00 R   
<--- Bytes 16-30      E   A   D   M   E   .   m   d   \n  \00 s   r   c   \n  
<--- Bytes 0-16       \n  -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 16-32      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 32-48      -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
<--- Bytes 48-51      \n  \00 \00 
<--- Bytes 0-16       @   [   /   h   o   m   e   /   u   g   c   /   j   a   m   e   
<--- Bytes 16-32      s   .   m   a   c   i   s   a   a   c   /   D   e   s   k   t   
<--- Bytes 32-48      o   p   /   c   p   s   c   _   5   2   6   _   a   s   s   i   
<--- Bytes 48-62      g   n   m   e   n   t   1   ]   \n  \00 >   >       \00 
!! [19806] ~ Client listener exitting, sending interrupt to sister threads
!! Worker thread exited successfully

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
