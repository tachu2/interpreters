JAVAC = javac
JAVA = java
MAIN_CLASS = Main

SOURCES = main.java

CLASSES = $(SOURCES:.java=.class)

all: run

run: compile
	$(JAVA) $(MAIN_CLASS)

compile: $(CLASSES)

%.class: %.java
	$(JAVAC) $<

clean:
	rm -f *.class

.PHONY: all compile run clean