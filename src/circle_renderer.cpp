#include "circle_renderer.hpp"
#include <iosfwd>

unsigned int CircleRenderer::vao = 0;
unsigned int CircleRenderer::vbo = 0;
unsigned int CircleRenderer::ebo = 0;
unsigned int CircleRenderer::instance_vbo = 0;
unsigned int CircleRenderer::shader = 0;
size_t CircleRenderer::length = 0;
int CircleRenderer::viewproj_loc = -1;

std::string find_file(const std::filesystem::path &file_name) {
  static const std::filesystem::path paths[] = {"./", "../", "../src/"};
  std::ifstream file;
  for (auto prefix : paths) {
    auto path = prefix;
    path += file_name;
    file = std::ifstream(path);
    if (file.good())
      break;
  }
  if (!file.is_open())
    std::cout << "failed to open: " << file_name << std::endl;
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

void draw_circles(entt::registry &reg, const glm::mat4 &viewproj) {
  const auto view = reg.view<const PhysicsComponent, const ColorComponent>();
  std::vector<CircleData> data;
  view.each([&data](const PhysicsComponent &p, const ColorComponent &c) {
    Color color = ColorFromHSV(c.hue, 1.0, 1.0);
    b2Vec2 pos = b2Body_GetPosition(p.id);
    glm::vec4 col = glm::vec4(static_cast<float>(color.r) / 255.0,
                              static_cast<float>(color.g) / 255.0,
                              static_cast<float>(color.b) / 255.0,
                              static_cast<float>(color.a) / 255.0);
    data.push_back(CircleData{
        pos.x,
        pos.y,
        p.radius,
        col,
    });
  });
  CircleRenderer::update(&data.front(), data.size(), viewproj);
  CircleRenderer::draw();
}
