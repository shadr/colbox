# filters out glfw warning that indicates that wayland does not support getting window position
make -j8 && ./main 2>&1  | rg --invert-match "WARNING: GLFW: Error: 65548"
