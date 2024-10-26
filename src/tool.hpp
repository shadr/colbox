#ifndef TOOL_H_
#define TOOL_H_

struct BaseTool {
  virtual void update(b2WorldId) = 0;
  virtual void ui() = 0;
  virtual void draw() = 0;
};

struct NoneTool : BaseTool {
  void update(b2WorldId) override {}
  void ui() override {}
  void draw() override {}
};

struct ToolWithRadius : BaseTool {
  float radius = 300.0f;

  void ui() override;
  void draw() override;
};

struct ForceTool : ToolWithRadius {
  void update(b2WorldId) override;
};

struct BasePaintProperty {
  virtual void ui() = 0;
  virtual void set_property(b2ShapeId) = 0;
};

struct PaintFrictionProperty : BasePaintProperty {
  float value;
  void ui() override;
  void set_property(b2ShapeId) override;
};

struct PaintRestitutionProperty : BasePaintProperty {
  float value;
  void ui() override;
  void set_property(b2ShapeId) override;
};

struct PaintPropertyTool : ToolWithRadius {
  BasePaintProperty *property = new PaintFrictionProperty();

  void update(b2WorldId) override;
  void ui() override;

private:
  int ui_property_index = 0;
};

#endif // TOOL_H_
