# RFID Protocol

| Message   | Data             | Action                                                                                                                                               |
|-----------|------------------|------------------------------------------------------------------------------------------------------------------------------------------------------|
| (request) | -                | respond with the most recently read address and checksum, unless it has been cleared or not read yet, in which case respond with six 0xff bytes |
| 0x44      | address - 1 byte | Set the address of the reader device                                                                                                                 |
| 0x54      | -                | Clear most recently read address                                                                                                                     |
