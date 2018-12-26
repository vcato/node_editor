#include "charmapperobjects.hpp"

#include <sstream>
#include "sceneobjects.hpp"
#include "maybepoint2d.hpp"
#include "point2dobject.hpp"

using std::string;
using std::cerr;
using std::make_unique;
using std::ostream;


Class posExprClass()
{
  auto make_pos_expr_object_function =
    [&](const Class::NamedParameters &named_parameters) -> Optional<Object> {
      BodyLink body_link;
      Optional<Point2D> maybe_position;

      for (auto &named_parameter : named_parameters) {
        const string &name = named_parameter.first;
        const Any &value = named_parameter.second;

        if (name=="body") {
          if (!value.isObject()) {
            assert(false);
          }

          auto body_object_data_ptr =
            dynamic_cast<const BodyObjectData*>(&value.asObject().data());

          if (!body_object_data_ptr) {
            assert(false);
          }

          body_link = body_object_data_ptr->body_link;
          assert(body_link.hasValue());
        }
        else if (name=="pos") {
          maybe_position = maybePoint2D(value);

          if (!maybe_position) {
            // This needs to give an error once we have access to the
            // error stream.
            cerr << "Could't make the position\n";
            cerr << "value.type: " << value.typeName() << "\n";
            return {};
          }
        }
        else {
          cerr << "name: " << name << "\n";
          assert(false);
        }
      }

      if (!body_link.hasValue()) {
        // This needs to give an error, but we don't have access to the
        // error stream.
        cerr << "No body specified\n";
        return {};
      }


      if (!maybe_position) {
        assert(false);
      }

      return Object(make_unique<PosExprObjectData>(body_link,*maybe_position));
    };

  return Class(make_pos_expr_object_function);
}


Optional<PosExprData> maybePosExpr(const Any &result,ostream &error_stream)
{
  if (!result.isObject()) {
    error_stream << "not a PosExpr\n";
    return {};
  }

  const PosExprObjectData *pos_expr_object_data_ptr =
    dynamic_cast<const PosExprObjectData*>(&result.asObject().data());

  if (!pos_expr_object_data_ptr) {
    return {};
  }

  PosExprData pos_expr = *pos_expr_object_data_ptr;

  return pos_expr;
}


Any PosExprObjectData::member(const std::string &member_name) const
{
  if (member_name=="body") {
    Object body_link_object(std::make_unique<BodyLinkObjectData>(body_link));
    body_link_object.data();
    return Any(std::move(body_link_object));
  }

  if (member_name=="position") {
    return makePoint2DObject(position);
  }

  assert(false);
}


std::vector<std::string> BodyLinkObjectData::memberNames() const
{
  return {"scene_name","body_name"};
}


Any BodyLinkObjectData::member(const std::string &member_name) const
{
  if (member_name=="scene_name") {
    if (hasValue()) {
      std::ostringstream stream;
      stream << "Scene(" << &scene() << ")";
      return Any(stream.str());
    }
    else {
      return Any();
    }
  }

  if (member_name=="body_name") {
    if (hasValue()) {
      return body().name;
    }
    else {
      return Any();
    }
  }

  std::cerr << "member_name: " << member_name << "\n";
  assert(false); // needs test
}
