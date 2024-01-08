SOURCE_FILES=SharedMemory/SharedMemoryIntegral/SharedMemory_integral_IPC.cpp

LINUX_CC=g++
WINDOWS_CC=g++

FREEBSD_HOME=/home/dmytro/FreeBSD-sysroot
FREEBSD_CC=clang++
FREEBSD_FLAGS= \
	-target x86_64-unknown-freebsd13.0 \
 	--sysroot=$(FREEBSD_HOME) \
 	-stdlib=libc++ \
	--verbose \
 	-I$(FREEBSD_HOME)/include \
	-fuse-ld=lld \

FREEBSD_LIBS= \
    -L$(FREEBSD_HOME)/usr/lib \
	-L$(FREEBSD_HOME)/lib \
    -lc++ \
    -lm \
    -lc \
    -nodefaultlibs \
	-lgcc_s \
	-lPIPC \

linux: $(SOURCE_FILES)
	$(LINUX_CC) $(SOURCE_FILES) -o example -lPIPC

clean:
	rm example

freebsd: $(SOURCE_FILES)
	$(FREEBSD_CC) $(FREEBSD_FLAGS) $(SOURCE_FILES) -o example $(FREEBSD_LIBS)

windows: $(SOURCE_FILES)
	$(WINDOWS_CC) $(SOURCE_FILES) -o example.exe -lPIPC