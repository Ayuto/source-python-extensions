#(C)2004-2008 Deniz Sezen
# Makefile derived from the work of David "BAILOPAN" Anderson

HL2SDK = ../../hl2sdk
SRCDS = ~/srcds_l

### EDIT BELOW FOR OTHER PROJECTS ###

OPT_FLAGS = -O2 -funroll-loops -s -pipe
GCC4_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
DEBUG_FLAGS = -g -ggdb3
CPP = /usr/bin/gcc34
BINARY = es_extensions_i486.so

HL2PUB = $(HL2SDK)/public
HL2LIB = $(HL2SDK)/linux_sdk

OBJECTS = esx_globals.cpp \
	esx_player_manager.cpp \
	esx_py_methods.cpp \
	esx_scanner.cpp \
	esx_signature_manager.cpp \
	serverplugin_convar.cpp \
	serverplugin_base.cpp

LINK = $(HL2LIB)/tier1_i486.a  \
	vstdlib_i486.so \
	tier0_i486.so \
	/usr/lib/gcc/i386-redhat-linux/3.4.6/libstdc++.a \
	/usr/lib/gcc/i386-redhat-linux/3.4.6/libgcc_eh.a \
	libpython2.5.so.1.0 \
	libdyncall_s.a \
	libdynload_s.a -static-libgcc

INCLUDE = -I. -I$(HL2PUB) -I$(HL2PUB)/dlls -I$(HL2PUB)/engine -I$(HL2PUB)/tier0 -I$(HL2PUB)/tier1 \
          -I$(HL2PUB)/vstdlib -I$(HL2SDK)/tier1 -I. -I./Python25 -I./Python25/Include

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

all:
	mkdir -p $(BIN_DIR)
	ln -sf $(SRCDS)/bin/vstdlib_i486.so vstdlib_i486.so
	ln -sf $(SRCDS)/bin/tier0_i486.so tier0_i486.so
	ln -sf $(SRCDS)/cstrike/addons/eventscripts/_engines/python/Lib/plat-linux2/libpython2.5.so.1.0 libpython2.5.so.1.0
	$(MAKE) sourcemm
	rm -rf $(BINARY)

LINK += -Wl,--rpath,\$$ORIGIN/eventscripts/_engines/python/Lib/plat-linux2 


sourcemm: $(OBJ_LINUX)
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
