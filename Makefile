BUILDDIR ?= build

all: submodules build

.PHONY: submodules
submodules:
	git submodule update --init --recursive

.PHONY: clean
clean:
	rm -rf "$(BUILDDIR)"

.PHONY: build
build:
	mkdir -p "$(BUILDDIR)"
	cd "$(BUILDDIR)"; cmake ..
	cd "$(BUILDDIR)"; make

.PHONY: run
run:
	./$(BUILDDIR)/linux_gfx

.PHONY: convert
convert:
	cd "$(BUILDDIR)"; ffmpeg -vcodec rawvideo -f rawvideo -pix_fmt rgb565 -s 800x480 -i output.raw -f image2 -vcodec png output.png -y

.PHONY: test
test: build run convert
