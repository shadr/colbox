default_build_type:='Debug'
# when targeting web build type is defaulted to Release due to stack overflow in Debug builds
default_web_build_type:='Release'

build build_type=default_build_type:
	mkdir -p build
	cd build && cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE={{build_type}}
	cd build && make -j9

run build_type=default_build_type: (build build_type)
	# filters out warning: wayland does not support getting window position
	cd build && ./sgame 2>&1 | grep --invert-match "WARNING: GLFW: Error: 65548"

build-web build_type=default_web_build_type:
	mkdir -p build-web
	cd build-web && emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE={{build_type}} -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3" -DCMAKE_EXECUTABLE_SUFFIX=".html"
	cd build-web && emmake make -j9

run-web build_type=default_web_build_type: (build-web build_type)
	cd build-web && emrun sgame.html
