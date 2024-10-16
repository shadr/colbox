#include "rlImGui/rlImGui.h"
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <raylib.h>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;

typedef std::pair<box, entt::entity> rtree_value;
typedef bgi::rtree<rtree_value, bgi::quadratic<16>> rtree;
