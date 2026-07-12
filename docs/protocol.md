# SR08 Command Experiments


| Sent | Response Start | Payload |
|---|---|---|
| 5A | 5A | 07 38 83 60... |
| 59 | 59 | 00 14 78 FD... |
| 5B | 5B | 00 14 78 FD... |
| 5C | 5C | 00 14 78 FD... |
| 58 | 5C | 00 14 78 FD... |


## Observation

0x58 produces a 0x5C response.
Possible request/response relationship.

## Command 0x01

TX:

01


RX:

03 55 FC 77 FD ...
13 55 FC 77 FD ...
01 00 00 02 00 00 5A A5 ...


Observation:

- Produces multiple notification packets.
- Contains known header sequence 5A A5.
- Possibly device information or initialization response.

## Command 0x02

TX:

02


RX sequence:

02 00 00 02 00 00 5A A5 ...

03 XX 01 02 ...

13 XX 01 02 ...


Observations:

- Generates multiple packets.
- XX field changes between responses.
- 5A A5 header appears consistently.
- Possible initialization/status packet.