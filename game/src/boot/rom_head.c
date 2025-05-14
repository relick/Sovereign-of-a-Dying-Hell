#include <genesis.h>

USED_EXTERNALLY
const ROMHeader rom_header = {
#if (ENABLE_BANK_SWITCH != 0)
    "SEGA SSF        ",
#elif (MODULE_MEGAWIFI != 0)
    "SEGA MEGAWIFI   ",
#else
    "SEGA MEGA DRIVE ",
#endif
    "(C)KDEV 2025.FEB",                                 // Copyright, format `(C)XXXX YYYY.ZZZ` with X = dev ID, Y = year, Z = month
    "Sovereign of a Dying Hell v0.3                  ", // Japan name
    "Sovereign of a Dying Hell v0.3                  ", // Overseas name
    "GM 00000000-00",                                   // Serial number, format `GM YYYYYYYY-ZZ` with Y = number, Z = revision
    0x000,                                              // checksum, compute by adding every 16 bit word from $000200 to end of the rom, then truncate
    "J6              ",                                 // List of supported devices. J6 = 3 and 6 button support
    0x00000000,                                         // ROM start address
#if (ENABLE_BANK_SWITCH != 0)
    0x003FFFFF,                                         // ROM end address (4MB)
#else
    0x000FFFFF,                                         // ROM end address (1MB)
#endif
    0xE0FF0000,                                         // RAM start address (should always be $FF0000)
    0xE0FFFFFF,                                         // RAM end address (should always be $FFFFFF)

    // SRAM declaration.
    "RA",                                               // Always RA
    0xF820,                                             // F8 = 8-bit (odd addresses), RAM saves. 20 always
    0x00200001,                                         // SRAM start address
    0x0020FFFF,                                         // SRAM end address
    "            ",                                     // Modem support (leave blank)
    "                                        ",         // Memos
    "JUE             "                                  // Region support (JUE)
};
