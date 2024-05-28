all: prepare
		+$(MAKE) -C src

.PHONY: clean
clean :
	rm -f target/*.o target/*.exe
			
.PHONY: prepare
prepare :
	mkdir -p target

.PHONY: all
run : all
	./target/coffed log.log

debug: all
	gdb --args ./target/coffed /mnt/c/Users/x037816/Documents/logback-config/mmb-services.log 

.PHONY: test
test:
	cd test && ./build.sh $(unittest)
