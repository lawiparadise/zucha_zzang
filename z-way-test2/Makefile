### Variables ###
TARGET = z-way-t

ZWAY_LIB_DIR = /opt/z-way-server/libs
ZWAY_INC_DIR = /opt/z-way-server/libzway


OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

INCLUDES = -I$(ZWAY_INC_DIR)
#INCLUDES = -I$(WIRINGPI)



LIBDIR = -L$(ZWAY_LIB_DIR)
LIBS += -lzway -lzcommons -lwiringPi

#LDFLAGS += -static
LIBS += -lpthread -lxml2 -lz -lm -lcrypto -larchive 

### Targets ###

all: $(TARGET)

run:
	(export LD_LIBRARY_PATH=$(ZWAY_LIB_DIR):$(LD_LIBRARY_PATH); export DYLD_LIBRARY_PATH=$(ZWAY_LIB_DIR):$(DYLD_LIBRARY_PATH); ./$(TARGET))

$(TARGET): $(OBJECTS)
	$(CC) $(LIBDIR) $(TARGET_LIBDIR) $(LDFLAGS) $(TARGET_ARCH) -o $@ $< $(LIBS)

%.o: %.c $(wildcard *.h)

	$(CC) $(INCLUDES) $(TARGET_INCLUDES) $(CFLAGS) $(TARGET_ARCH) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

full_clean: clean
	rm -Rf $(ZWAY_LIB_DIR)

.PHONY: all clean run full_clean
