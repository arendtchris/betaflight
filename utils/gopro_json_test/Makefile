CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra

ROOT := ../..
INCLUDES := -I. -I$(ROOT)/src/main
SOURCES := gopro_json_file_test.c $(ROOT)/src/main/common/gopro_json.c
BINARY := gopro_json_file_test

JSON ?= sample_gopro_state.json
OBJECT ?= status
KEY ?= 8

.PHONY: all run run-status run-settings clean

all: $(BINARY)

$(BINARY): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(BINARY)

run: $(BINARY)
	./$(BINARY) $(JSON) $(OBJECT) $(KEY)

run-status: $(BINARY)
	./$(BINARY) $(JSON) status $(KEY)

run-settings: $(BINARY)
	./$(BINARY) $(JSON) settings $(KEY)

clean:
	rm -f $(BINARY)
