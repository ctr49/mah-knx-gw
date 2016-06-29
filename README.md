# mah-knx-gw
Miele@Home KNX Gateway

This gateway will communicate with Miele XGW 3000 (also XGW 2000 but untested for now) and a KNX bus.

The initial version only looks for a local XGW 3000 and fetches the device information xml and parse it. It will print some debug output to stdout (no KNX functionality yet). I currently do not have any Miele@Home interfaces (waiting for my XKM 3000 L), so I cannot proceed until I see actual devices.

Todo:
- enumerate devices
- integrate KNX library
- map device functions to KNX group addresses
- send actions to Miele@Home devices


INSTALL:
prerequisites: libmxl2-dev, for KNX functionality bcusdk-dev or knxd-dev will be required

currently compiles with a simple:
```
gcc `xml2-config --cflags --libs` -o mahknxgw mahknxgw.c
```
