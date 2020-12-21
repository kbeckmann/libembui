TARGET = demo

DEBUG = 1
#OPT = -Og -ggdb
OPT = -Og -ggdb -fsanitize=address

BUILD_DIR = build

C_SOURCES =  \
main.c \
../src/embui.c


PREFIX = 

CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

C_DEFS =

C_INCLUDES =  \
-I. \
-I../src


ASFLAGS = $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS  = $(C_DEFS) $(C_INCLUDES) `sdl2-config --cflags` $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#LIBS = -lm `sdl2-config --libs`
LIBS = -lasan -lm `sdl2-config --libs`
LDFLAGS = $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

all: $(BUILD_DIR)/$(TARGET).elf

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	-rm -fR $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)
