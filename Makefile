JAVAC = javac
JAVA = java
DIR = java
PACKAGE = lox
NAME := clox
BUILD_DIR := build
OBJS_DIR := objs

SOURCES := $(wildcard $(DIR)/com/craftinginterpreters/$(PACKAGE)/*.java)
CLASSES := $(addprefix $(BUILD_DIR)/, $(SOURCES:.java=.class))

JAVA_OPTIONS := -Werror

# ASTファイルのパスを定義
AST_FILES := $(DIR)/com/craftinginterpreters/lox/Expr.java $(DIR)/com/craftinginterpreters/lox/Stmt.java

all: $(NAME)

.PHONY: default
default: $(CLASSES)
	@: # Don't show "Nothing to be done" output.

# Compile a single .java file to .class.
$(BUILD_DIR)/$(DIR)/%.class: $(DIR)/%.java $(AST_FILES)
	@ mkdir -p $(BUILD_DIR)/$(DIR)
	@ javac -cp $(DIR) -d $(BUILD_DIR)/$(DIR) $(JAVA_OPTIONS) -implicit:none $<
	@ printf "%8s %-60s %s\n" javac $< "$(JAVA_OPTIONS)"

# ASTファイルが存在しない場合のみ生成
$(AST_FILES): $(DIR)/com/craftinginterpreters/tool/GenerateAst.java
	@ mkdir -p $(BUILD_DIR)/$(DIR)
	javac -cp $(DIR) -d $(BUILD_DIR)/$(DIR) $(JAVA_OPTIONS) $(DIR)/com/craftinginterpreters/tool/GenerateAst.java
	java -cp $(BUILD_DIR)/$(DIR) com.craftinginterpreters.tool.GenerateAst $(DIR)/com/craftinginterpreters/lox

.PHONY: run
run: all
	./clox

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(OBJS_DIR)
	rm -f $(NAME)

clox:
	@ $(MAKE) -f c.make BUILD_DIR=$(OBJS_DIR) NAME=$(NAME)

.PHONY: re
re: clean all