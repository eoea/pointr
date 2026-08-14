.PHONY: prepare
prepare:
	git clone --depth 1 https://github.com/raysan5/raylib.git ./thirdparty/raylib/
	$(MAKE) -C ./thirdparty/raylib/src
	$(MAKE) -C ./thirdparty/raylib/src PLATFORM=OSX
	rm -rf ./thirdparty/raylib/.git

.PHONY: clean
clean:
	-rm -rf ./build/pointr.app
	-rm -rf ./thirdparty/raylib
	-rm ./build/pointr

.PHONY: build
.SILENT: build
build:
	clang ./main.c \
		-I. \
		-I./thirdparty/raylib/src \
		./thirdparty/raylib/src/libraylib.a \
		-framework Cocoa \
		-framework IOKit \
		-framework CoreVideo \
		-framework OpenGL \
		-framework QuartzCore \
		-o ./build/pointr

.PHONY: bundle
bundle: build
	mkdir -p ./build/pointr.app/Contents/MacOS
	mkdir -p ./build/pointr.app/Contents/Resources
	cp ./build/pointr ./build/pointr.app/Contents/MacOS/
	chmod +x ./build/pointr.app/Contents/MacOS/pointr
	cp ./macos/Info.plist ./build/pointr.app/Contents/
	cp ./macos/assets/icon.icns ./build/pointr.app/Contents/Resources/
