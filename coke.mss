@Make[Report]
@Style[Indent 0, Spacing 1.2]
@Font[TimesRoman10]
@MajorHeading[Coke Machine Status Protocol]
@Begin[center]
David A. Nichols
@Value(date)

@End[center]

This document describes the Pup version of the Coke Machine Status Protocol
(CMSP).  It provides a means for polling the status of a Coke machine to
determine if any Cokes are available from it.

Note: All numbers in this document are expressed in octal notation.

The Coke Status server runs on socket 0771.  It responds to CokeRequest Pups
(type 0200) with either a CokeReply Pup (type 0201) or a NoCokePepsi Pup
(type 0202).  The response is sent to the sender Pup address in the
CokeRequest Pup with the same Pup ID.

The CokeRequest Pup has a single word of data giving the number of the Coke
machine whose status is being requested.  Currently, the only machine with
an assigned number is the 16oz bottle machine in the foyer to the Computer
Science terminal room at Carnegie-Mellon University in Pittsburgh, PA.

The CokeReply Pup has the machine type in word 0, followed by machine type
specific data.  For machine type 1 (6-slot, 2 bottles in reserve when the
slot is empty), the format is as follows:

@Begin[Description]
Word 0:@\Machine type (= 1).

Word 1:@\Column full (1 if at least one Coke is available for sale,
0 otherwise).

Word 2:@\First bottle temperature: number of minutes the first bottle in
the rack has been in the machine.  Does not overflow, but instead stops at
32767.

Word 3:@\Second bottle temperature: same as Word 1.

Word 4:@\Temperature of the rest of the bottles in the column.

Words 5-8:@\Same as 1-4 for second column.

Words 9-24:@\Same as 1-4 for third through sixth columns.
@End[Description]

A NoCokePepsi Pup is sent when the status of the named Coke machine is not
available to the server, @i[but normally is].  A server should generate no
response for a machine about which it knows nothing.  Word 0 of the Pup
contains an error code.  The remainder of the Pup may optionally contain a
human-readable explanation of why the status is not available.

@Heading[Appendix: Assigned numbers]
@Tabdivide(3)
@SubHeading[Sockets]
CokeServices@\0771
@SubHeading[Pup types]
@Begin[Format]
CokeRequest@\0200
CokeReply@\0201
NoCokePepsi@\0202
@End[Format]
@SubHeading[Machine Types]
6 slot, 2 bottles in reserve@\1
@SubHeading[NoCokePepsi error codes]
Sensor down@\1
@SubHeading[Coke machines]
1@\Foyer to CS terminal room, CMU (type 1)
