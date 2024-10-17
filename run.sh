# filters out glfw warning that indicates that wayland does not support getting window position
time cmake --build . -j9 && ./main 2>&1  | rg --invert-match "WARNING: GLFW: Error: 65548"
