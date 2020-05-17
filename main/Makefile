# this makefile is used to compile all files in the Ultimate USB to Controlport project.
# no executable file is generated, compiling the files is used to verify that changes made to the source code are working.
# this makefile was tested on OsX where the Arduino app is installed in the the user's Applications directory.
# change the ARDUINO_BASE variable below if the Arduino app is installed in a different location.
# you can also set the ARDUINO_BASE unix environment variable before calling the makefile as is:
# $ ARDUINO_BASE=/.../... make
#
# the following makefile targets are useful defined.
# all: compile all library and example source code
# doxygen: generate the doxygen documentation
# doc: generate the doxygen documentation and start a web browser
# clean: remove any generated files

ARDUINO_BASE?=$(HOME)/Applications/Arduino.app/Contents/Java/hardware
ARDUINO_HEADERS?=$(ARDUINO_BASE)/arduino/avr
ARDUINO_PATH?=$(ARDUINO_BASE)/tools/avr

# path the the USB Host Shield 2.0 Library
USBSHIELD_BASE?=$(HOME)/Documents/Arduino/libraries/USB_Host_Shield_2.0

export PATH := $(PATH):$(ARDUINO_PATH)/bin
SHELL := env PATH=$(PATH) /bin/bash

CXX=avr-c++
CXXFLAGS+=-Wall -Wextra -Wno-error=narrowing \
	-O -std=gnu++11 \
	-fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics \
	-mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR \
	-I$(ARDUINO_HEADERS)/cores/arduino \
	-I$(ARDUINO_HEADERS)/variants/standard \
	-I$(ARDUINO_HEADERS)/libraries/SPI/src \
	-I$(USBSHIELD_BASE) \
	-I.

SOURCES_CPP=$(shell echo *.cpp)
all:	$(SOURCES_CPP:.cpp=.o) main.o

clean:
	find . -name '*~' -delete
	find . -name '*.o' -delete
	$(RM) -rf html/

doxygen:
	doxygen Doxyfile

doc:	doxygen
	if [ `uname` = Darwin ] ; then open html/index.html ; else $(BROWSER) html/index.html ; fi

%.o:	%.ino
	$(CXX) $(CXXFLAGS) -x c++ -c -o $@ $<
