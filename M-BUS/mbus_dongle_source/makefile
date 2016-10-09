# Simple Makefile by Volker Oth (c) 1999
# edited by AVRfreaks.net nov.2001

########### change this lines according to your project ##################

#put the name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
    MCU = at90s2313
    ;MCU = atmega8

#put the name of the target file here (without extension)
	TRG	= m_bus

#put your C sourcefiles here 
	SRC	= 	uart.c\
			$(TRG).c

#put additional assembler source file here
	ASRC    =

#additional libraries and object files to link
	LIB	=

#additional includes to compile
	INC	=   

#compiler flags
	CPFLAGS	= -g -Os -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)
#	CPFLAGS	= -g -O -Wall -ffreestanding  -Wa,-ahlms=$(<:.c=.lst)

#linker flags
	LDFLAGS = -Wl,-Map=$(TRG).map,--cref

########### you should not need to change the following line #############
include $(AVR)/avrfreaks/avr_make

###### dependecies, add any dependencies you need here ###################
$(TRG).o :	$(TRG).h\
			$(TRG).c\
			uart.o

uart.o:		uart.h\
			uart.c
