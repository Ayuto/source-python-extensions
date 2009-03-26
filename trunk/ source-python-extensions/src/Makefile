#(C)2004-2008 Deniz Sezen
# Makefile derived from the work of David "BAILOPAN" Anderson

SRCDS_BASE = ~/srcds_l

###########################
### ADD OTHER SDKS HERE ###
###########################
HL2SDK_OB = ../../../hl2sdk-ob
HL2SDK_EP1 = ../../../hl2sdk

####################
### SOURCE FILES ###
####################
OBJECTS = esx_globals.cpp \
	esx_hook_manager.cpp \
	esx_player_manager.cpp \
	esx_py_methods.cpp \
	esx_scanner.cpp \
	esx_signature_manager.cpp \
	esx_event_parser.cpp \
	serverplugin_convar.cpp \
	serverplugin_base.cpp \
	sourcehook.cpp

###################################
### COMPILER OPTIONS ANDS FLAGS ###
###################################
OPT_FLAGS = -O2 -funroll-loops -s -pipe
GCC4_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
DEBUG_FLAGS = -g -ggdb3
CPP = /usr/bin/gcc34

#######################
### LINKING OPTIONS ###
#######################
LINK = $(HL2LIB)/tier1_i486.a  \
	vstdlib_i486.so \
	tier0_i486.so \
	/usr/lib/gcc/i386-redhat-linux/3.4.6/libstdc++.a \
	/usr/lib/gcc/i386-redhat-linux/3.4.6/libgcc_eh.a \
	libpython2.5.so.1.0 \
	libdyncall_s.a \
	libdynload_s.a -static-libgcc

INCLUDE = -I. \
	-I$(HL2PUB) \
	-I$(HL2PUB)/dlls \
	-I$(HL2PUB)/engine \
	-I$(HL2PUB)/tier0 \
	-I$(HL2PUB)/tier1 \
	-I$(HL2PUB)/vstdlib \
	-I$(HL2SDK)/tier1 \
	-I./Python25 \
	-I./Python25/Include \
	-I./sourcehook

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS = $(DEBUG_FLAGS)
else
	BIN_DIR = Release
	CFLAGS = $(OPT_FLAGS)
endif

#################################
### ENGINE SPECIFIC VARIABLES ###
#################################
override ENGSET = false

# Using the OB engine?
ifeq "$(ENGINE)" "orangebox"
 HL2SDK = $(HL2SDK_OB)
 HL2PUB = $(HL2SDK_OB)/public
 HL2LIB = $(HL2SDK_OB)/lib/linux
 INCLUDE += -I$(HL2PUB)/game/server
 SRCDS = $(SRCDS_BASE)/orangebox
 BINARY = sp_extensions_ep2_i486.so
 LINK += $(HL2LIB)/tier2_i486.a \
         $(HL2LIB)/tier3_i486.a
 CFLAGS += -DORANGE_BOX 
 override ENGSET = true
endif

# Using the old EP1 engine?
ifeq "$(ENGINE)" "ep1"
 HL2SDK = $(HL2SDK_EP1)
 HL2PUB = $(HL2SDK_EP1)/public
 HL2LIB = $(HL2SDK_EP1)/linux_sdk
 SRCDS  = $(SRCDS_BASE)/ep1
 BINARY = sp_extensions_ep1_i486.so
 override ENGSET = true
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
	ln -sf $(SRCDS)/bin/libpython2.5.so.1.0 libpython2.5.so.1.0
	$(MAKE) sourcemm
	rm -rf $(BINARY)

LINK += -Wl,--rpath,\$$ORIGIN/eventscripts/_engines/python/Lib/plat-linux2 

check:
	if [ "$(ENGSET)" = "false" ]; then \
		echo "You must supply ENGINE=orangebox or ENGINE=ep1"; \
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
