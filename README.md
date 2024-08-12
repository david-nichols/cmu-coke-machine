# cmu-coke-machine
Here's the original Unix client code for the [CMU SCS Coke machine](https://www.cs.cmu.edu/~coke/).

The files:
* `coke.c` -- The client code itself, as of 1985. 
* `coke.1` -- The Unix `man` page.
* `coke.mss` -- A [Scribe](https://en.wikipedia.org/wiki/Scribe_(markup_language)) document describing the network protocol it used.

`coke` was a command line program running on our Vaxes running BSD 4.2 Unix. A sample 
invocation would look like 
```
$ coke
         flavor:   Diet Coke       "Coke"          "Coke"
h:mm in machine:   3:20            COLD            EMPTY

         flavor:   Diet Coke       "Coke"          "Coke"
h:mm in machine:   EMPTY           0:20            3:00
```
I don't know why "Coke" was quoted in the output. 

The server was a microcomputer in the ceiling 
above the Coke machine. Our network at the time was the original 3 Megabit Ethernet running 
the [Pup](https://en.wikipedia.org/wiki/PARC_Universal_Packet) protocols. TCP/IP was just
beginning to show up.

The "h:mm in the machine" value told you how long the bottles
had been the machine to help you snag the cold one.
It was reported by the server in minutes with a max of 255; the client would 
print "COLD" for the max value. One day, Randy Pausch joked to me that the program
never showed "COLD" to him, that there must be something personal in the code to keep him
from seeing it. I knew a feature request when I heard it, so I added the code at lines 199-205.
I never got around to telling him.