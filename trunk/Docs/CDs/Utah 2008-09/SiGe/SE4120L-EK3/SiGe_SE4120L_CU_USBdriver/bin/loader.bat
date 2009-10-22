@ECHO OFF
fx2-programmer 0x04b4 0x8613 set 0xE600 1
fx2-programmer 0x04b4 0x8613 program ../firmware/Vend_Ax.ihx
fx2-programmer 0x04b4 0x8613 set 0xE600 0
PAUSE
