@ECHO OFF
fx2-programmer 0x16C0 0x072F set 0xE600 1
fx2-programmer 0x16C0 0x072F program ../firmware/gn3s_firmware.ihx
fx2-programmer 0x16C0 0x072F set 0xE600 0
PAUSE
