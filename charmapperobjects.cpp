#include "charmapperobjects.hpp"

#include "sceneobjects.hpp"
#include "maybepoint2d.hpp"

using std::string;
using std::cerr;


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
            dynamic_cast<BodyObjectData*>(value.asObject().data_ptr);

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

      return Object(*new PosExprObjectData(body_link,*maybe_position));
    };

  return Class(make_pos_expr_object_function);
}


Optional<PosExprData> maybePosExpr(const Any &result)
{
  if (!result.isObject()) {
    return {};
  }

  PosExprObjectData *pos_expr_object_data_ptr =
    dynamic_cast<PosExprObjectData*>(result.asObject().data_ptr);

  if (!pos_expr_object_data_ptr) {
    return {};
  }

  PosExprData pos_expr = *pos_expr_object_data_ptr;

  return pos_expr;
}
