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
	gdb --args ./target/coffed log.log 

flatpak-build:
	flatpak-builder --user --install --force-clean build-dir com.github.klauswk.Coffed.yml

flatpak-run:
	flatpak run com.github.klauswk.Coffed log.log

.PHONY: test
test:
	cd test && ./build.sh $(unittest)
