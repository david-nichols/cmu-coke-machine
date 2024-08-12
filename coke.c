/*
 *	coke.c - Find out if any coke is in the terminal room
 *		 vending machine.
 *
 *	Link with -lpup -lqueue -lenet -ljobs
 *
 ****************************************************************
 * HISTORY:
 * 15-Jun-85  David Nichols (nichols) at Carnegie-Mellon University
 *	Fixed column headings for lastest flavor changes.
 *
 * 12-May-83  David Nichols (nichols) at Carnegie-Mellon University
 *	Changed "Tab" heading for first slot to "Diet Coke."
 *
 * 21-Apr-83  David Nichols (nichols) at Carnegie-Mellon University
 *	Decided to go for it and nuke Pausch.
 *
 * 31-Jan-83  David Nichols (nichols) at Carnegie-Mellon University
 *	Changed to give coldness of bottom coke instead of next-to-
 *	bottom.
 *
 * 26-Jan-83  David Nichols (nichols) at Carnegie-Mellon University
 *	Changed the formatting this time.  We now print a more pictoral
 *	display of the light status.
 *
 * 25-Jan-83  David Nichols (nichols) at Carnegie-Mellon University
 *	Fixed up formatting some more.
 *
 * 20-Jan-83  David Nichols (nichols) at Carnegie-Mellon University
 *	Added -v (verbose) option, fixed up formatting of messages a
 *	little.
 *
 * 17-Jan-83  David Nichols (nichols) at Carnegie-Mellon University
 *	Created.
 */

#include <stdio.h>		/* for def of NULL */
#include <sys/types.h>
#include <pup/pup.h>
#include <pup/pupconstants.h>

#define min(a,b)		((a) < (b) ? (a) : (b))

#define CokeServicesSocket	0771
/* Pup types for Coke services */
#define CokeRequest		0200	/* request Coke status */
#define CokeReply		0201	/* return Coke status */
#define NoCokePepsi		0202	/* Coke status unavailable */
/* Coke machine types and machine numbers */
#define SixSlotMachine		1
#define TerminalRoomMachine	1

struct QPup *GetCokePup();
char *ColdTime();

int	verbose = 0;
char   *FlavorTab[] = {
    "bad flavor",
    "Diet Coke",
    "Diet Coke",
    "\"Coke\"",
    "\"Coke\"",
    "\"Coke\"",
    "\"Coke\"",
    NULL
};
maxFlavorLen = 0;

main (argc, argv)
    int argc;
    char *argv[];
{
    struct QPup *cp;
    char    buf[400];
    int     i;

    if (argc >= 2 && strcmp (argv[1], "-v") == 0) {
	++verbose;
	--argc;
	++argv;
    }
    if (argc >= 2)
	quit (1, "usage: coke [-v]\n");
    for (i = 1; FlavorTab[i] != NULL; ++i)
	if (strlen (FlavorTab[i]) > maxFlavorLen)
	    maxFlavorLen = strlen (FlavorTab[i]);
    cp = GetCokePup (TerminalRoomMachine);
    if (cp == NULL)
	printf ("Coke server not responding.\n");
    else if (cp -> type == CokeReply)
	PrintCokePup (cp);
    else if (cp -> type == NoCokePepsi) {
	cp -> length -= PUPHEAD;
	if (cp -> length <= 2)
	    printf ("Coke machine hardwire is down.\n");
	else {
	    GetPupBytes (&cp -> data[1], buf,
		    min (cp -> length - 2, sizeof (buf)));
	    buf[cp -> length - 2] = 0;
	    printf ("Coke status not available: %s\n", buf);
	}
    }
}

struct QPup *GetCokePup (machine)
    int machine;
{
    int     length;
    long    lastProbe,
            start;
    struct QPup *request,
               *reply;
    struct Socket  *soc;
    struct PupAddress   namepa;

    namepa.netID = 0;
    namepa.hostID = 0;
    namepa.socketID = CokeServicesSocket;
    soc = MakeSocket (0, namepa, 1, 1 * TICKSPERSECOND, 0, NULL, NULL);
    if (soc == NULL)
	quit (1, "coke: Can't open Ethernet.\n");
    request = AllocQPup ();
    request -> type = CokeRequest;
    request -> data[0] = machine;
    PutPup (soc, request, 2);
    lastProbe = start = time (0);
    reply = GetPup (soc);
    while (start + 8 > time (0) &&
	    (reply == NULL ||
		(reply -> type != CokeReply &&
		    reply -> type != NoCokePepsi))) {
	if (reply != NULL)
	    FreeQPup (reply);
	if (lastProbe + 2 <= time (0)) {
	    lastProbe = time (0);
	    PutPup (soc, request, 2);
	}
	reply = GetPup (soc);
    }
    DestroySocket (soc);
    return reply;
}

PrintCokePup (cp)
    struct QPup *cp;
{
    int     i;
    char   *s;

    switch (cp -> data[0]) {
	case SixSlotMachine: 
	    if (verbose)
		for (i = 0; i < 6; ++i)
		    printf ("Column %d (%s): %s Bottle 1: %d min, Bottle 2: %d min, Rest: %d min\n",
			    i + 1, FlavorTab[i + 1],
			    cp -> data[i * 4 + 1] ? "not empty, " : "empty, ",
			    cp -> data[i * 4 + 2], cp -> data[i * 4 + 3],
			    cp -> data[i * 4 + 4]);
	    else {
		printf ("         flavor:   ");
		for (i = 0; i < 6; i += 2)
		    printf (i < 4 ? "%-16s" : "%s\n", FlavorTab[i + 1]);
		printf ("h:mm in machine:   ");
		for (i = 0; i < 6; i += 2) {
		    if (cp -> data[i * 4 + 1] == 0)
			s = "EMPTY";
		    else
			s = ColdTime (cp -> data[i * 4 + 2]);
		    printf (i < 4 ? "%-16s" : "%s\n", s);
		}
		printf ("\n");
		printf ("         flavor:   ");
		for (i = 1; i < 6; i += 2)
		    printf (i < 4 ? "%-16s" : "%s\n", FlavorTab[i + 1]);
		printf ("h:mm in machine:   ");
		for (i = 1; i < 6; i += 2) {
		    if (cp -> data[i * 4 + 1] == 0)
			s = "EMPTY";
		    else
			s = ColdTime (cp -> data[i * 4 + 2]);
		    printf (i < 4 ? "%-16s" : "%s\n", s);
		}
	    }
	    break;
	default: 
	    printf ("Unknown Coke machine type.  Can't report status.\n");
	    break;
    }
}

/* Convert time in minutes to printable string.  Returns static (yuk). */
char *ColdTime (t)
    int t;
{
    static long seed = 0;
    static char buf[20];

    if (t >= 255)
	if (getuid () == 352) {	/* Confuse Randy. */
	    if (seed == 0) {
		seed = time (0);
		srand (seed);
	    }
	    sprintf (buf, "4:%02d", rand () % 20);
	}
	else
	    strcpy (buf, "COLD");
    else
	sprintf (buf, "%d:%02d", t / 60, t % 60);
    return buf;
}

ColumnStatus(column, full, bottle1, bottle2, rest)
    int column, full, bottle1, bottle2, rest;
{
    int     extras;
    char    buf[200];

    sprintf (buf, "Column %d (%s): ", column, FlavorTab[column]);
    extras = maxFlavorLen - strlen (FlavorTab[column]);
    while (extras-- > 0)
	strcat (buf, " ");
    printf ("%s", buf);
    if (verbose)
	printf ("%s Bottle 1: %d min, Bottle 2: %d min, Rest: %d min\n",
		full ? "full, " : "empty,", bottle1, bottle2, rest);
    else {
	int     coldness = bottle1;

	if (full)
	    if (coldness >= 255)
		printf ("not empty, (%ss are cold)\n", FlavorTab[column]);
	    else
		printf ("not empty, (cooled for %d:%02d hours)\n",
			coldness / 60, coldness % 60);
	else
	    printf ("** EMPTY **\n");
    }
}
