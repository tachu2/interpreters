JAVAC = javac
JAVA = java
DIR = java
PACKAGE = lox

BUILD_DIR := build

SOURCES := $(wildcard $(DIR)/com/craftinginterpreters/$(PACKAGE)/*.java)
CLASSES := $(addprefix $(BUILD_DIR)/, $(SOURCES:.java=.class))

JAVA_OPTIONS := -Werror

.PHONY: default
default: $(CLASSES)
	@: # Don't show "Nothing to be done" output.

# Compile a single .java file to .class.
$(BUILD_DIR)/$(DIR)/%.class: $(DIR)/%.java
	@ mkdir -p $(BUILD_DIR)/$(DIR)
	@ javac -cp $(DIR) -d $(BUILD_DIR)/$(DIR) $(JAVA_OPTIONS) -implicit:none $<
	@ printf "%8s %-60s %s\n" javac $< "$(JAVA_OPTIONS)"

run: default
	$(JAVA) -cp $(BUILD_DIR) $(DIR)/$(PACKAGE)/Lox