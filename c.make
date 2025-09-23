CC      := gcc
CFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter -MMD -MP
MODE    := release
SOURCE_DIR  := c

HEADERS = $(wildcard $(SOURCE_DIR)/*.h)
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS    = $(OBJECTS:.o=.d)

$(NAME): $(OBJECTS)
	@ printf "%8s %-40s %s\n" $(CC) $@ "$(CFLAGS)"
	@ mkdir -p build
	@ $(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@ printf "%8s %-40s %s\n" $(CC) $< "$(CFLAGS)"
	@ mkdir -p $(BUILD_DIR)
	@ $(CC) -c $(CFLAGS) -o $@ $<

-include $(DEPS)