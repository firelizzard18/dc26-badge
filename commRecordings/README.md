Binary Comms Recordings
======

# About
These are binary files, which were generated by storing the bytes read from the
UART while badges are connected.  You will probably want a hexeditor to read
these files.  If you are on windows, I recommend HxD.  On other OSs, the
command line should have a hexdump tool.

# Format A
  * 0x000 - 0x3FF: data from our human badge.  Should be the same two messages over and over.
  * 0x400 - 0x7FF: data from other badges.  Most files were dumped after recording a single badge type, so this region has the two messages from just that badge.

# Format B
  * 0x000: Checksum for offset 1 `uint32`
  * 0x004: Checksum for offset 2 `uint32`
  * 0x008: Last write offset for data block 1 `uint32`
  * 0x00C: Last write offset for data block 2 `uint32`
  * 0x010 - 0x02F: Reserved
  * 0x030 - 0x417: Data block 1 - from our human badge
  * 0x418 - 0x7FF: Data block 2 - from other badges
