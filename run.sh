# filters out glfw warning that indicates that wayland does not support getting window position
cmake CMakeLists.txt && make && ./main 2>&1  | rg --invert-match "WARNING: GLFW: Error: 65548"
