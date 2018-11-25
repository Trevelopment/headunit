TOP = $(realpath ..)

APPLICATION_NAME=AndroidAuto
APPLICATION_VERSION=$(shell git describe --tags)

M3TOOLCHAIN=./m3-toolchain
PROTOTOOLCHAIN=./protobuf-2.6.1-arm
LIBUNWIND=./libunwind-1.2-rc1
CC=$(M3TOOLCHAIN)/bin/arm-cortexa9_neon-linux-gnueabi-gcc
CXX=$(M3TOOLCHAIN)/bin/arm-cortexa9_neon-linux-gnueabi-g++
LD=$(M3TOOLCHAIN)/bin/arm-cortexa9_neon-linux-gnueabi-ld
SYSROOT=$(M3TOOLCHAIN)/arm-cortexa9_neon-linux-gnueabi/sysroot

INCLUDES=$(shell ./pkg-config-wrapper $(SYSROOT) --cflags gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 dbus-1 libusb-1.0  libcrypto openssl glib-2.0 dbus-c++-1 dbus-c++-glib-1 libudev alsa) -I$(TOP)/hu -I$(TOP)/common -I$(TOP)/hu/generated.arm  -I$(PROTOTOOLCHAIN)/include -I$(LIBUNWIND)/include

CFLAGS=-O3 -g -rdynamic -pthread --sysroot=$(SYSROOT) -DCMU=1 -D__STDC_FORMAT_MACROS -march=armv7-a -mtune=cortex-a9 -mfpu=neon
CXXFLAGS=$(CFLAGS) -std=c++11 -static-libstdc++ -D_GLIBCXX_USE_C99

#static link  dbus-c++-glib-1 since it's not on the car
LFLAGS= --sysroot=$(SYSROOT) -g -rdynamic -pthread -ldl -static-libstdc++ $(shell ./pkg-config-wrapper $(SYSROOT) --libs gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 dbus-1 libusb-1.0 libcrypto openssl glib-2.0 dbus-c++-1 libudev alsa) -l:libdbus-c++-glib-1.a -L$(PROTOTOOLCHAIN)/lib -lprotobuf -L$(LIBUNWIND)/lib -lunwind

#headunit binary
SRCS = $(TOP)/hu/hu_aad.cpp
SRCS += $(TOP)/hu/hu_aap.cpp
SRCS += $(TOP)/hu/hu_ssl.cpp
SRCS += $(TOP)/hu/hu_usb.cpp
SRCS += $(TOP)/hu/hu_uti.cpp
SRCS += $(TOP)/hu/hu_tcp.cpp
SRCS += $(TOP)/hu/generated.arm/hu.pb.cc


SRCS += $(TOP)/common/audio.cpp
SRCS += $(TOP)/common/glib_utils.cpp
SRCS += $(TOP)/common/command_server.cpp
SRCS += $(TOP)/common/web++/web++.cpp

SRCS += nm/mzd_nightmode.cpp
SRCS += gps/mzd_gps.cpp
SRCS += bt/mzd_bluetooth.cpp

SRCS += hud/hud.cpp

SRCS += outputs.cpp
SRCS += $(TOP)/common/config.cpp
SRCS += callbacks.cpp

SRCS += main.cpp

OBJS = $(addsuffix .arm.o, $(basename $(SRCS)))
DEPS = $(addsuffix .arm.d, $(basename $(SRCS)))

APP = headunit

all: tag proto dbusxx $(APP)

tag:
	echo "#define HEADUNIT_VERSION \"`git describe --tag`\"" > version.h

proto: $(TOP)/hu/generated.arm/hu.pb.cc

dbusxx: dbus/generated_cmu.h

$(APP): $(OBJS)
	$(CXX) -Os -s -o $(APP) $(OBJS) $(LFLAGS)

$(TOP)/hu/generated.arm/hu.pb.cc $(TOP)/hu/generated.arm/hu.pb.h: $(TOP)/hu/hu.proto
	$(PROTOTOOLCHAIN)/bin/protoc $< --proto_path=$(TOP)/hu/ --cpp_out=$(TOP)/hu/generated.arm/

dbus/generated_cmu.h: dbus/cmu_interfaces.xml
	cd dbus ; ./dbusxx-xml2cpp cmu_interfaces.xml --proxy=generated_cmu.h

%.arm.o : %.c
	$(CC) -Os -s $(CFLAGS) $(INCLUDES) -c $<  -o $@

%.arm.o : %.cpp
	$(CXX) -Os -s $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

%.arm.o : %.cc
	$(CXX) -Os -s $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	rm -f *~ $(TOP)/hu/generated.arm/* dbus/generated_*.* $(OBJS) $(APP) $(DEPS) $(APPLICATION_NAME)_*.zip version.h

install: all
	cp -a -f headunit installer/config/androidauto/data_persist/dev/bin/; cp -a -f headunit.json installer/config/androidauto/data_persist/dev/bin/
release: install
	zip -r -FS $(APPLICATION_NAME)_$(APPLICATION_VERSION).zip installer/

-include $(DEPS)

.PHONY: clean
