#ifndef TOOL_H_
#define TOOL_H_

enum class Tool {
  None,
  Force,
  Paint,
};

enum class PaintProperty {
  Friction,
  Restitution,
};

struct PaintData {
  PaintProperty prop;
  float value;
};

#endif // TOOL_H_
