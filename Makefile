BINDIR = ./bin
DST = ./dst

.PHONY: all
all: $(BINDIR) $(DST) $(OBJ_FILES) $(HEADERS)
	cd src && make

bin:
	mkdir -p bin
	mkdir -p dst

.PHONY: clean
clean:
	rm -rf bin
	rm -rf dst
