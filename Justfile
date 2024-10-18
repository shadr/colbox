default_build_type:='Debug'
# when targeting web build type is defaulted to Release due to stack overflow in Debug builds
default_web_build_type:='Release'

build-init build_type=default_build_type:
	mkdir -p build
	cd build && cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_LINKER_TYPE=MOLD -DCMAKE_BUILD_TYPE={{build_type}}

build:
	cd build && make -j9
	@cp build/compile_commands.json compile_commands.json

run:
	# filters out warning: wayland does not support getting window position
	cd build && ./colbox 2>&1 | grep --invert-match "WARNING: GLFW: Error: 65548"

build-web-init build_type=default_web_build_type:
	mkdir -p build-web
	cd build-web && emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE={{build_type}} -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html"

build-web:
	cd build-web && emmake make -j9

run-web:
	cd build-web && emrun colbox.html
