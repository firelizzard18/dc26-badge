# DefCon 26 Badge Hacking

The DefCon 26 official badges can communicate with each other via baginal intercourse, which is implemented as a simple protocol over top of UART (9600 baud).

Badge 'unlock' packets are trivial to replay.

## Story

Cody (@rigel314) decided to use a logic analyzer to log the communications between two human badges, and later between a human and a goon badge. From this analysis, he determined that the protocol was UART and he guessed the packet structure. Ethan and Cody then connected a Teensy to the RX and TX pins of a human badge in order to log all of the inter-badge communications. They then proceeded to hunt down and connect with each type of badge. They then successfully used the Teensy to replay this data onto a human badge. Once the concept was proved, they connected the Teensy to an unpowered badge and used that to replay many more badges.

And... it turns out [the protocol is documented](https://github.com/Wireb/badge_bus/wiki).

## TODO

  * Clean up the code
  * Update this document to reflect [the protocol documentation](https://github.com/Wireb/badge_bus/wiki)

## Badges

  1. Human
  2. Contest
  3. Goon
  4. Artist
  5. Vendor
  6. Press
  7. Speaker
  8. CFP (Call For Papers)
  
## Connector Pinout

From top to bottom, the pinout of the baginal intercourse connector is:

  * RX (pin 1 of the µC)
  * Ground
  * Ground
  * TX (pin 5 of the µC)

## Packet Structure

| Length | Segment     | Value            |
| ------ | ----------- | ---------------- |
| 4      | Sync        | `"UUBB"`         |
| 1      | Type        | `0x80` or `0xC0` |
| 1      | (magic)     | `0xAF`           |
| 1      | Length      |                  |
| (len)  | Message     |                  |
| 2      | (signature) |                  |

The sync word is always `0x55554242` (ASCII `"UUBB"`). The type is `0x80` for requests and `0xC0` for responses. The purpose of the magic byte is unknown, but its value is always `0xAF`. The last two bytes (signature) are assumed to be a checksum of some sort.

## Requests

A request has a type of `0x80` and an empty message: `55 55 42 42 80 af 00 11 cb`

## Response

A response has a type of `0xC0` and a message with the form `00 01 00 0X 00` where X is the badge number, 1-8 (see Badges above).

| Badge   | Packet                                      | Number | Signature |
| ------- | ------------------------------------------- | ------ | --------- |
| Human   | `55 55 42 42 c0 af 05 00 01 00 01 00 f3 e4` | `0x01` | `0xf3e4`  |
| Contest | `55 55 42 42 c0 af 05 00 01 00 02 00 a6 b7` | `0x02` | `0xa6b7`  |
| Goon    | `55 55 42 42 c0 af 05 00 01 00 03 00 95 86` | `0x03` | `0x9586`  |
| Artist  | `55 55 42 42 c0 af 05 00 01 00 04 00 0c 11` | `0x04` | `0x0c11`  |
| Vendor  | `55 55 42 42 c0 af 05 00 01 00 05 00 3f 20` | `0x05` | `0x3f20`  |
| Press   | `55 55 42 42 c0 af 05 00 01 00 06 00 6a 73` | `0x06` | `0x6a73`  |
| Speaker | `55 55 42 42 c0 af 05 00 01 00 07 00 59 42` | `0x07` | `0x5942`  |
| CFP     | `55 55 42 42 c0 af 05 00 01 00 08 00 49 7c` | `0x08` | `0x497c`  |

## Building Sketches

To compile the included Arduino sketches, each sketch must be in its own folder, named the same as the sketch. For example, to compile `dc26badgedirect.ino`, it must be in a directory named `dc26badgedirect`: `dc26badgedirect/dc26badgedirect.ino`. Arduino is peculiar like that.

## Harvesting

In order to harvest these packets, Ethan removed the battery holders and connected two leads to TX and RX of the connector. Those leads, along with power (3.3v) and ground, were connected to a Teensy 3.0 (an Arduino-compatible microcontroller board). The Teensy was programmed to capture all UART communication and write it to EEPROM. See [dc26badgestorage.ino](dc26badgestorage.ino).

## Replaying

To replay messages to a badge, connect the badge's RX pin to the TX pin of an Arduino. Then transmit the message. It's that simple. See [dc26badgereplay.ino](dc26badgereplay.ino).

## Notes

In linux, the following command will allow you to use `echo`, `cat`, etc on the Teensy's serial connection. On your computer, the Teensy may not be `/dev/ttyACM0`.

```bash
stty -F /dev/ttyACM0 115200 raw -echo -echoe -echok -echoctl -echoke

# read from the teensy
cat /dev/ttyACM0

# write to the teensy
echo message > /dev/ttyACM0
```

## License

Copyright (C) 2018 Ethan Reesor, Cody Creager

Licensed under the MIT license (see the LICENSE file).
