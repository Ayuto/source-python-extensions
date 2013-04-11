#(C)2004-2008 SourceMM Development Team
# Makefile written by David "BAILOPAN" Anderson

###########################
### ADD OTHER SDKs HERE ###
###########################
HL2SDK_OB  = ../../../sdks/hl2sdk-ob-valve
HL2SDK_L4D = ../../../sdks/hl2sdk-l4d
HL2SDK_L4D2 = ../../../sdks/hl2sdk-l4d2

#######################
### OTHER LIBRARIES ###
#######################
PYTHON  = ./python
DYNCALL = ./dyncall
DYNDETOURS = ./dyndetours

###################################
### SOURCE DEDICATED SERVER DIR ###
###################################
SRCDS_BASE = ~/Desktop/srcds

####################
### SOURCE FILES ###
####################
OBJECTS = spe_binutils.cpp \
	spe_dyncall.cpp \
	spe_hook_manager.cpp \
	spe_main.cpp \
	spe_misc.cpp \
	spe_player.cpp \
	spe_python.cpp \
	spe_scanner.cpp \
	spe_sigcache.cpp \
	spe_callback.cpp \
	spe_convar.cpp

######################
### COMPILER FLAGS ###
######################
OPT_FLAGS = -funroll-loops -pipe
GCC4_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
DEBUG_FLAGS = -g -g3 -ggdb3
CPP = /usr/bin/gcc

#################################
### ENGINE SPECIFIC VARIABLES ###
#################################
override ENGSET = false

# Using the Left4Dead Engine?
ifeq "$(ENGINE)" "l4d"
 HL2SDK = $(HL2SDK_L4D)
 HL2PUB = $(HL2SDK_L4D)/public
 HL2LIB = $(HL2SDK_L4D)/linux_sdk
 CFLAGS = -DENGINE_VERSION=3
 INCLUDE += -I$(HL2SDK_L4D)/public/game/server
 SRCDS = $(SRCDS_BASE)/l4d
 BINARY = sp_extensions_l4d.so
 override ENGSET = true
endif

# Using the Left4Dead2 Engine?
ifeq "${ENGINE}" "l4d2"
 HL2SDK = $(HL2SDK_L4D2)
 HL2PUB = $(HL2SDK_L4D2)/public
 HL2LIB = $(HL2SDK_L4D2)/linux_sdk
 CFLAGS = -DENGINE_VERSION=3
 INCLUDE += -I$(HL2SDK_L4D2)/public/game/server
 SRCDS = $(SRCDS_BASE)/l4d2
 BINARY = sp_extensions_l4d2.so
 override ENGSET = true
endif

# Using the Orangebox Engine?
ifeq "$(ENGINE)" "ob"
 HL2SDK = $(HL2SDK_OB)
 HL2PUB = $(HL2SDK_OB)/public
 HL2LIB = $(HL2SDK_OB)/lib/linux
 CFLAGS += -DENGINE_VERSION=2
 INCLUDE = -I$(HL2SDK_OB)/game/server
 SRCDS = $(SRCDS_BASE)/orangebox
 BINARY = sp_extensions_ep2.so
 LINK += $(HL2LIB)/tier2_i486.a \
	$(HL2LIB)/tier3_i486.a \
	libtier0_srv.so \
	libvstdlib_srv.so
 override ENGSET = true
endif

#######################
### LINKING OPTIONS ###
#######################
LINK += $(HL2LIB)/tier1_i486.a \
	$(HL2LIB)/mathlib_i486.a \
	$(DYNCALL)/lib/libdyncall_s.a \
	$(DYNCALL)/lib/libdynload_s.a \
	/usr/lib/gcc/i486-linux-gnu/4.4.5/libstdc++.a \
	/usr/lib/gcc/i486-linux-gnu/4.4.5/libgcc_eh.a \
	$(DYNDETOURS)/libDynDetours.a \
	$(DYNDETOURS)/AsmJit/lib/libAsmJit.a \
	-static-libgcc

LINK += -Wl,--rpath,\$$ORIGIN/eventscripts/_engines/python/Lib/plat-linux2

###########################
### INCLUDE DIRECTORIES ###
###########################
INCLUDE += -I. \
	-I$(HL2PUB) \
	-I$(HL2PUB)/dlls \
	-I$(HL2PUB)/engine \
	-I$(HL2PUB)/tier0 \
	-I$(HL2PUB)/tier1 \
	-I$(HL2PUB)/vstdlib \
	-I$(HL2SDK)/tier1 \
	-I$(HL2SDK)/dlls \
	-I$(PYTHON)/linux/Include \
	-I$(DYNCALL)/include \
	-I$(DYNDETOURS)/AsmJit \
	-I$(DYNDETOURS)

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS += $(DEBUG_FLAGS)
else
	BIN_DIR = Release
	CFLAGS += $(OPT_FLAGS)
endif

GCC_VERSION := $(shell $(CPP) -dumpversion >&1 | cut -b1)

CFLAGS += -D_LINUX -DNDEBUG -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp \
		-Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca \
		-Dstrcmpi=strcasecmp -Wall -Wno-non-virtual-dtor -Wno-uninitialized -fPIC \
		-fno-exceptions -fno-rtti -msse -msse2 -m32 -g -g3 -ggdb3

ifeq "$(GCC_VERSION)" "4"
	CFLAGS += $(GCC4_FLAGS)
endif

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all: check
	mkdir -p $(BIN_DIR)
	mkdir -p ./Binaries

	# Create links to the shared objects
	cp $(HL2LIB)/libvstdlib_srv.so libvstdlib_srv.so
	cp $(HL2LIB)/libtier0_srv.so libtier0_srv.so

	# Copy python
	#cp $(PYTHON)/libs/libpython2.5.so.1.0 libpython2.5.so.1.0

	# Build SPE
	$(MAKE) spe

	# Copy to the final directory
	mv $(BIN_DIR)/$(BINARY) ./Binaries/$(BINARY)

check:
	if [ "$(ENGSET)" = "false" ]; then \
		echo "You must supply ENGINE={ob, l4d, l4d2}."; \
		exit 1; \
	fi

spe: check $(OBJ_LINUX)
	$(CPP) $(INCLUDE) -m32 $(CFLAGS) $(OBJ_LINUX) $(LINK) -shared -ldl -lm -o$(BIN_DIR)/$(BINARY)

debug:
	$(MAKE) all DEBUG=true

default: all

clean:
	rm -rf Release/*.o
	rm -rf Release/$(BINARY)
	rm -rf Debug/*.o
	rm -rf Debug/$(BINARY)