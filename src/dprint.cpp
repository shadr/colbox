#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>

const static char *RESET_COLOR_CODE = "\x1b[0m";

struct Hsv {
  int hue;
  float sat = 1.0f;
  float value = 1.0f;

  Hsv(int hue) : hue(hue) {}
};

struct Rgb {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  Rgb(Hsv h) {
    float hh, p, q, t, ff;
    hh = h.hue;
    int i = (h.hue % 360) / 60;
    ff = hh - i;
    p = h.value * (1.0 - h.sat);
    q = h.value * (1.0 - (h.sat * ff));
    t = h.value * (1.0 - (h.sat * (1.0 - ff)));
    float rr, gg, bb;
    switch (i) {
    case 0:
      rr = h.value, gg = t, bb = p;
      break;
    case 1:
      rr = q, gg = h.value, bb = p;
      break;
    case 2:
      rr = p, gg = h.value, bb = t;
      break;
    case 3:
      rr = p, gg = q, bb = h.value;
      break;
    case 4:
      rr = t, gg = p, bb = h.value;
      break;
    case 5:
      rr = h.value, gg = p, bb = q;
      break;
    }
    r = rr * 255;
    g = gg * 255;
    b = bb * 255;
  }
};

std::ostream &operator<<(std::ostream &out, Rgb color) {
  std::cout << std::dec;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
  std::cout << "\e[38;2;";
#pragma clang diagnostic pop
  std::cout << static_cast<int>(color.r) << ";";
  std::cout << static_cast<int>(color.g) << ";";
  std::cout << static_cast<int>(color.b) << "m";
  return out;
}

void print_hex(uint8_t b) {
  std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex
            << std::uppercase << static_cast<int>(b);
}

void print_memory(const void *bytes, int len, int chunk = 1,
                  int row_length = 16) {
  const char *ptr = static_cast<const char *>(bytes);
  int row_count = 0;
  int chunk_count = 0;
  for (int i = 0; i < len; ++i) {
    if (row_count % row_length == 0) {
      row_count = 0;
      chunk_count = 0;
      if (i != 0) {
        std::cout << std::endl;
      }
      std::cout << RESET_COLOR_CODE << std::hex << std::uppercase
                << reinterpret_cast<long long>(ptr + i) << "| ";
    } else if (chunk_count % chunk == 0) {
      chunk_count = 0;
      std::cout << " ";
    }
    int b = static_cast<int>(ptr[i]);
    Hsv hsv = 300 * b / 255;
    std::cout << hsv;
    print_hex(b);
    row_count += 1;
    chunk_count += 1;
  }
  std::cout << RESET_COLOR_CODE << std::dec << std::endl;
}

template <typename T>
void print_object(const T &object, int chunk = 1, int row_length = 16) {
  const T *p = &object;
  print_memory(p, sizeof(T), chunk, row_length);
}
