All numbers are in hexadecimal. \
Horn: sends 80 in byte 0 to 0x730 when depressed, then zero when released \
Lights: first depress sends series of ones and zeroes, ending in one, to 0x730, release sends 0 (could be bounce) \
Fog front: sends 2 to 0x730 on depress, sends 0 to 0x730 on release \
Fog rear: sends 4 to 0x730 \
Indicator left: sends 8 to 0x730 \
Indicator right: sends 10 to 0x730 \
Hazard: sends 20 to 0x730 \ 

Lights state is ORed \
Cruise: sends 10 in byte 0 to 0x500 when first depressed after startup \
Sends zero when next depressed. \
Regen: sends 20 in byte 0 to 0x500 when first depressed, send 0 when released \
Solar: send 8 in byte 0 to 0x500 when first depressed, sends 0 when released \
Start: First depress starts heartbeats and sends 1 to 0x500, second sends 0 to 0x500 and stops heartbeats (heartbeats are all zeros to 0x201 and 0x221) \
Drive mode: first depress sends 4 to 0x500, second sends 0 to 0x500 \
FWD/REV: first depress sends 2 to 0x500, second depress sends 0 to 0x500 \
Cruise adjust doesn't seem to do anything \


Combinations of signals to 0x500 are ORed, so full state is sent each time state changes. \


On startup, sends 40 to 0x730 \
 
Starts sets 1 at 0x500, sends zeros to motors \

FWD/REV sets 2 and resets on release \
Drive does nothing \
Regen does nothing \
Cruise does nothing \
Lights do nothing \
Solar sets 4 at 0x500 \


Somehow start sends 46 1C 3C 00 to 0x551 and 0x501 \