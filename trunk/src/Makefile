#(C)2004-2008 SourceMM Development Team
# Makefile written by David "BAILOPAN" Anderson

###########################
### ADD OTHER SDKs HERE ###
###########################
HL2SDK_EP1 = ../sourcesdk-ep1
HL2SDK_EP2 = ../sourcesdk-ep2
HL2SDK_L4D = ../sourcesdk-l4d

#######################
### OTHER LIBRARIES ###
#######################
PYTHON  = ../Python25
DYNCALL = ../dyncall

###################################
### SOURCE DEDICATED SERVER DIR ###
###################################
SRCDS_BASE = ~/srcds

####################
### SOURCE FILES ###
####################
OBJECTS = spe_binutils.cpp \
	spe_convar.cpp \
	spe_main.cpp \
	spe_player.cpp \
	spe_python.cpp \
	spe_scanner.cpp

######################
### COMPILER FLAGS ###
######################
OPT_FLAGS = -O2 -funroll-loops -s -pipe
GCC4_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
DEBUG_FLAGS = -g -ggdb3
CPP = /usr/bin/gcc34

#################################
### ENGINE SPECIFIC VARIABLES ###
#################################
override ENGSET = false

# Using the Left4Dead Engine?
ifeq "$(ENGINE)" "l4d"
 HL2SDK = $(HL2SDK_L4D)
 HL2PUB = $(HL2SDK_L4D)/public
 HL2LIB = $(HL2SDK_L4D)/linux_sdk
 CFLAGS = -DENGINE_VER=3
 INCLUDE += -I$(HL2SDK_L4D)/public/game/server
 SRCDS = $(SRCDS_BASE)/l4d
 BINARY = sp_extensions_l4d_i486.so
 override ENGSET = true
endif

# Using the Orangebox Engine?
ifeq "$(ENGINE)" "ob"
 HL2SDK = $(HL2SDK_OB)
 HL2PUB = $(HL2SDK_OB)/public
 HL2LIB = $(HL2SDK_OB)/linux_sdk
 CFLAGS += -DENGINE_VER=2
 INCLUDE += -I$(HL2SDK_OB)/public/game/server
 SRCDS = $(SRCDS_BASE)/orangebox
 BINARY = sp_extensions_ep2_i486.so
 override ENGSET = true
endif

# Using the Episode 1 engine?
ifeq "$(ENGINE)" "ep1"
 HL2SDK = $(HL2SDK_EP1)
 HL2PUB = $(HL2SDK_EP1)/public
 HL2LIB = $(HL2SDK_EP1)/linux_sdk
 CFLAGS = -DENGINE_VER=1
 SRCDS = $(SRCDS_BASE)/ep1
 BINARY = sp_extensions_ep1_i486.so
 override ENGSET = true
endif

#######################
### LINKING OPTIONS ###
#######################
LINK = $(HL2LIB)/tier1_i486.a \
	$(HL2LIB)/mathlib_i486.a \
	vstdlib_i486.so \
	tier0_i486.so \
	/usr/lib/gcc/i386-redhat-linux/3.4.6/libstdc++.a \
	/usr/lib/gcc/i386-redhat-linux/3.4.6/libgcc_eh.a \
	libpython2.5.so.1.0 -static-libgcc

LINK += -Wl,--rpath,\$$ORIGIN/eventscripts/_engines/python/Lib/plat-linux2

###########################
### INCLUDE DIRECTORIES ###
###########################
INCLUDE = -I. \
	-I$(HL2PUB) \
	-I$(HL2PUB)/dlls \
	-I$(HL2PUB)/engine \
	-I$(HL2PUB)/tier0 \
	-I$(HL2PUB)/tier1 \
	-I$(HL2PUB)/vstdlib \
	-I$(HL2SDK)/tier1 \
	-I$(HL2SDK)/dlls \
	-I$(PYTHON)/Include

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS = $(DEBUG_FLAGS)
else
	BIN_DIR = Release
	CFLAGS = $(OPT_FLAGS)
endif

GCC_VERSION := $(shell $(CPP) -dumpversion >&1 | cut -b1)

CFLAGS += -D_LINUX -DNDEBUG -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp \
		-Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca \
		-Dstrcmpi=strcasecmp -Wall -Wno-non-virtual-dtor -Wno-uninitialized -fPIC \
		-fno-exceptions -fno-rtti -msse -msse2 -m32

ifeq "$(GCC_VERSION)" "4"
	CFLAGS += $(GCC4_FLAGS)
endif

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all: check
	mkdir -p $(BIN_DIR)
	ln -sf $(SRCDS)/bin/vstdlib_i486.so vstdlib_i486.so
	ln -sf $(SRCDS)/bin/tier0_i486.so tier0_i486.so
	$(MAKE) sourcemm
	rm -rf $(BINARY)

check:
	if [ "$(ENGSET)" = "false" ]; then \
		echo "You must supply ENGINE=orangebox or ENGINE=ep1 or ENGINE=l4d."; \
		exit 1; \
	fi

sourcemm: check $(OBJ_LINUX)
	$(CPP) $(INCLUDE) -m32 $(CFLAGS) $(OBJ_LINUX) $(LINK) -shared -ldl -lm -o$(BIN_DIR)/$(BINARY)

debug:	
	$(MAKE) all DEBUG=true

default: all

clean:
	rm -rf Release/sourcehook/*.o
	rm -rf Release/*.o
	rm -rf Release/$(BINARY)
	rm -rf Debug/sourcehook/*.o
	rm -rf Debug/*.o
	rm -rf Debug/$(BINARY)

