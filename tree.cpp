#include "tree.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include "defaultdiagrams.hpp"


using std::string;
using std::cerr;
using std::function;
using std::vector;

using ItemType = TreeItem::Type;
using Path = Tree::Path;
using OperationVisitor = Tree::OperationVisitor;


namespace {
struct XPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.numericItem("X");
  }
};
}


namespace {
struct YPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.numericItem("Y");
  }
};
}


namespace {
struct ZPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.numericItem("Z");
  }
};
}


static void createXYZChildren(TreeItem &parent_item)
{
  parent_item.createItem2(ItemType::x,XPolicy());
  parent_item.createItem2(ItemType::y,YPolicy());
  parent_item.createItem2(ItemType::z,ZPolicy());
}


namespace {
struct GlobalPositionPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    vector<string> enumeration_names = {"Components","From Body"};
    visitor.enumeratedItem("Global Position",enumeration_names);
  }
};
}


namespace {
struct LocalPositionPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.voidItem("Local Position");
  }
};
}


namespace {
struct SourceBodyPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    vector<string> enumeration_names = {"Body1","Body2","Body3"};
    visitor.enumeratedItem("Source Body",enumeration_names);
  }
};
}


namespace {
struct TargetBodyPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    vector<string> enumeration_names = {"Body1","Body2","Body3"};
    visitor.enumeratedItem("Target Body",enumeration_names);
  }
};
}


namespace {
struct PosExprPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.voidItem("Pos Expr");
  }
};
}


static TreeItem posExprItem()
{
  TreeItem pos_expr_item(ItemType::pos_expr,PosExprPolicy());
  pos_expr_item.createItem2(ItemType::target_body,TargetBodyPolicy());
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(ItemType::local_position,LocalPositionPolicy());
    createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(
        ItemType::global_position,GlobalPositionPolicy()
      );
    createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }

  return pos_expr_item;
}


namespace {
struct MotionPassPolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor)
  {
    visitor(
      "Add Pos Expr",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,posExprItem());
      }
    );
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.voidItem("Motion Pass");
  }
};
}


static TreeItem motionPassItem()
{
  return TreeItem(ItemType::motion_pass,MotionPassPolicy());
}


namespace {
struct CharmapperPolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor)
  {
    cerr << "CharmapperPolicy::visitOperations()\n";
    visitor(
      "Add Motion Pass",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,motionPassItem());
      }
    );
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.voidItem("Charmapper");
  }
};
}


static TreeItem charmapperItem()
{
  return TreeItem(ItemType::charmapper,CharmapperPolicy());
}


namespace {
struct BodyPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor)
  {
    visitor.voidItem("Body");
  }
};
}


static TreeItem bodyItem()
{
  return TreeItem(ItemType::body,BodyPolicy());
}


namespace {
struct ScenePolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor)
  {
    visitor(
      "Add Body",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,bodyItem());
      }
    );
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Scene");
  }
};
}



static TreeItem sceneItem()
{
  return TreeItem(ItemType::scene,ScenePolicy());
}


void
  TreeItem::Policy::visitOperations(
    const Path &path,
    const OperationVisitor &visitor
  )
{
  assert(ptr);
  ptr->visitOperations(path,visitor);
}


void
  TreeItem::Policy::visitType(
    const Visitor &visitor
  ) const
{
  assert(ptr);
  ptr->visitType(visitor);
}


namespace {
struct RootPolicy {
  Tree &tree;

  RootPolicy(Tree &tree_arg)
  : tree(tree_arg)
  {
  }

  ~RootPolicy()
  {
  }

  void visitOperations(const Path &path,const Tree::OperationVisitor &visitor)
  {
    visitor(
      "Add Charmapper",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,charmapperItem());
      }
    );
    visitor(
      "Add Scene",
      [path,this](TreeOperationHandler &handler){
        tree.world().addScene();
        handler.addItem(path,sceneItem());
      }
    );
  }

  void visitType(const TreeItem::Visitor &)
  {
    assert(false);
  }
};
}


namespace {
struct EmptyPolicy {
  void visitOperations(const Path &,const Tree::OperationVisitor &)
  {
    cerr << "EmptyPolicy::visitOperations()\n";
  }

  void visitType(const TreeItem::Visitor &)
  {
    assert(false);
  }
};
}


Tree::Tree()
: _root_item(ItemType::root,RootPolicy(*this))
{
}


Path Tree::createItem(const Path &parent_path,const Item &item)
{
  return join(parent_path,getItem(parent_path).createItem(item));
}


auto Tree::itemType(const Path &path) const -> ItemType
{
  return getItem(path).type;
}


TreeItem::TreeItem(Type type_arg,Policy policy_arg)
  : type(type_arg), policy(policy_arg)
{
  if (type==Type::local_position) {
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
  }
}


TreeItem::TreeItem(Type type_arg)
: TreeItem(type_arg,EmptyPolicy())
{
}


void TreeItem::visit(const Visitor &visitor) const
{
  policy.visitType(visitor);
}


auto TreeItem::createItem(const TreeItem &item) -> Index
{
  Index result = child_items.size();
  child_items.push_back(TreeItem(item.type,item.policy));
  return result;
}


auto
  TreeItem::createItem2(
    ItemType type,
    TreeItem::Policy policy
  ) -> TreeItem&
{
  child_items.push_back(TreeItem(type,policy));
  return child_items.back();
}


auto TreeItem::createItem2(const TreeItem &item) -> TreeItem&
{
  child_items.push_back(TreeItem(item.type,item.policy));
  return child_items.back();
}


auto TreeItem::getItem(const Path &path,int depth) const -> const TreeItem &
{
  int path_length = path.size();

  if (depth==path_length) {
    return *this;
  }

  return child_items[path[depth]].getItem(path,depth+1);
}


auto Tree::getItem(const Path &path) -> Item&
{
  const Tree &const_self = *this;
  const Item &const_node = const_self.getItem(path);
  return const_cast<Item&>(const_node);
}


auto Tree::getItem(const Path &path) const -> const Item &
{
  return _root_item.getItem(path,0);
}


Diagram& Tree::itemDiagram(const Path &path)
{
  return getItem(path).diagram;
}


void Tree::removeChildItems(const Path &path)
{
  getItem(path).child_items.clear();
}


auto Tree::nChildItems(const Path &path) const -> SizeType
{
  const TreeItem &item = getItem(path);
  assert(item.child_items.size()<=std::numeric_limits<SizeType>::max());
  return item.child_items.size();
}


void Tree::visitOperations(const Path &path,OperationVisitor visitor)
{
  getItem(path).visitOperations(path,visitor);
}


static TreeItem globalPositionComponentsItems()
{
  TreeItem items(TreeItem::Type::root);
  createXYZChildren(items);
  items.diagram = fromComponentsDiagram();
  return items;
}


static TreeItem globalPositionFromBodyItems()
{
  TreeItem items(TreeItem::Type::root);
  items.createItem2(TreeItem::Type::source_body,SourceBodyPolicy());
  TreeItem &local_position_item =
    items.createItem2(TreeItem::Type::local_position,LocalPositionPolicy());
  items.diagram = fromBodyDiagram();
  createXYZChildren(local_position_item);
  return items;
}


void
  Tree::comboBoxItemIndexChanged(
    const Path &path,
    int index,
    OperationHandler &operation_handler
  )
{
  if (itemType(path)==TreeItem::Type::global_position) {
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


void Tree::visitItem(const Item &item,const ItemVisitor &visitor)
{
  item.visit(visitor);
}


WorldInterface &Tree::world()
{
  assert(_world_ptr);
  return *_world_ptr;
}
