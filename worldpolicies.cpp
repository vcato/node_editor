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
static TreeItem globalPositionComponentsItems()
{
  TreeItem items(EmptyPolicy{});
  createXYZChildren(items);
  items.diagram = fromComponentsDiagram();
  return items;
}
}


namespace world_policies {
static TreeItem globalPositionFromBodyItems()
{
  TreeItem items(EmptyPolicy{});
  items.createItem2(SourceBodyPolicy{});
  TreeItem &local_position_item = items.createItem2(LocalPositionPolicy{});
  items.diagram = fromBodyDiagram();
  createXYZChildren(local_position_item);
  return items;
}
}


namespace world_policies {
void
  GlobalPositionPolicy::comboBoxItemIndexChanged(
    const Path &path,
    int index,
    TreeItem::OperationHandler &operation_handler
  )
{
  switch (index) {
    case 0:
      // Components
      {
        TreeItem items = globalPositionComponentsItems();
        operation_handler.replaceTreeItems(path,items);
      }
      break;
    case 1:
      // From Body
      {
        TreeItem items = globalPositionFromBodyItems();
        operation_handler.replaceTreeItems(path,items);
      }
      break;
    default:
      assert(false);
  }
}
}


namespace world_policies {
static Diagram localPositionDiagram()
{
  Diagram diagram;
  NodeIndex vector_index = diagram.addNode("[$,$,$]");
  diagram.node(vector_index).setPosition({100,180});

  NodeIndex x_index = diagram.addNode("x");
  diagram.connectNodes(x_index,0,vector_index,0);
  diagram.node(x_index).setPosition({20,200});

  NodeIndex y_index = diagram.addNode("y");
  diagram.node(y_index).setPosition({20,150});
  diagram.connectNodes(y_index,0,vector_index,1);

  NodeIndex z_index = diagram.addNode("z");
  diagram.node(z_index).setPosition({20,100});
  diagram.connectNodes(z_index,0,vector_index,2);

  NodeIndex local_postion_index = diagram.addNode("local_position=$");
  diagram.node(local_postion_index).setPosition({230,150});

  diagram.connectNodes(vector_index,0,local_postion_index,0);
  return diagram;
}
}


namespace world_policies {
Diagram LocalPositionPolicy::defaultDiagram()
{
  return localPositionDiagram();
}
}