#include "worldpolicies.hpp"


namespace world_policies {
void createXYZChildren(TreeItem &parent_item)
{
  parent_item.createItem2(XPolicy{});
  parent_item.createItem2(YPolicy{});
  parent_item.createItem2(ZPolicy{});
}
}


namespace world_policies {
Diagram LocalPositionPolicy::defaultDiagram()
{
  return localPositionDiagram();
}
}
