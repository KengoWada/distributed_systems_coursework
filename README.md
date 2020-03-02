# Distributed Systems Coursework

Course work for BSE3202

## A Sequencer-based multicast protocol using Java RMI (Project 1)

Your task is to create a sequencer-based multicasting service, using a slight variant of the simple version
of the Amoeba protocol (the first protocol described [here](www.cdk3.net/coordination)), and a small client
application to exercise it. You will use Java RMI and multicast sockets.

- The main classes in your implementation will be:

  - **TestSequencer.java** – Each instance (you will create several) will allow the user to enter
    strings and multicast them to a group of instances of TestSequencer. TestSequencer will
    also be capable of “stress-testing” – multicasting messages as fast as possible.

  - **Group.java** – TestSequencer uses an instance of Group for group communication
    services. Group in turn uses a MulticastSocket to receive incoming messages, and uses RMI
    to the sequencer for sending messages and other, sequencer-specific operations

  - Sequencer.java, SequencerImpl.java – the interface to and implementation of a sequencer.

  - **History.java** – the implementation of the sequencer's history. History does not need to
    have a fixed capacity, but obsolete items should be removed whenever its size grows beyond a
    threshold, say 1024 entries.

### Note

- Attached you will find:

  - The RMI interface to an object which will implement the sequencer(Sequencer.java),

  - Two trivial classes associated with the Sequencer definition(SequencerJoinInfo.java and SequencerException.java)

  - Outline code for Group.java, and

  - Some hints on marchalling messages into datagram packets.

- You need to write SequencerImpl.java and History.java from scratch.

- The scheme you produce will not precisely implement the Amoeba protocol. In that protocol,
  messages are sent unreliably to the sequencer in the first instance. In yours, messages to be multicast are
  handed to the sequencer using RMI.

### Choosing a multicast address and time-to-live

SequencerImpl should take as an argument the IP multicast address that the corresponding Groups are to
use. To avoid collisions with numbers chosen by others, incorporate a random number into the multicast
IP address that you use – e.g. 234.day.month.rand where rand is a random number between 1 and 254.

Don't set your sockets' time-to-live (TTL) to more than 1. That way, your multicast packets will not be
transmitted beyond the local router.
