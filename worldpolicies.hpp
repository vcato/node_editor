namespace world_policies {
using Path = Tree::Path;
using OperationVisitor = Tree::OperationVisitor;
using std::vector;
using std::string;
using std::cerr;

struct SimplePolicy {
  void comboBoxItemIndexChanged(
    const Path &/*path*/,
    int /*index*/,
    TreeItem::OperationHandler &/*operation_handler*/
  )
  {
    assert(false);
  }

  Diagram defaultDiagram()
  {
    return Diagram();
  }
};


struct XPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.numericItem("X");
  }
};


struct YPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.numericItem("Y");
  }
};


struct ZPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.numericItem("Z");
  }
};


static void createXYZChildren(TreeItem &parent_item)
{
  parent_item.createItem2(XPolicy{});
  parent_item.createItem2(YPolicy{});
  parent_item.createItem2(ZPolicy{});
}


struct GlobalPositionPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    vector<string> enumeration_names = {"Components","From Body"};
    visitor.enumeratedItem("Global Position",enumeration_names);
  }

  void
    comboBoxItemIndexChanged(
      const Path &path,
      int index,
      TreeItem::OperationHandler &operation_handler
    );

  Diagram defaultDiagram() { return Diagram(); }
};


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


struct LocalPositionPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Local Position");
  }

  Diagram defaultDiagram()
  {
    return localPositionDiagram();
  }

  void comboBoxItemIndexChanged(
    const Path &/*path*/,
    int /*index*/,
    TreeItem::OperationHandler &/*operation_handler*/
  )
  {
    assert(false);
  }
};


struct SourceBodyPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    vector<string> enumeration_names = {"Body1","Body2","Body3"};
    visitor.enumeratedItem("Source Body",enumeration_names);
  }
};


struct TargetBodyPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    vector<string> enumeration_names = {"Body1","Body2","Body3"};
    visitor.enumeratedItem("Target Body",enumeration_names);
  }
};


struct PosExprPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Pos Expr");
  }
};


static TreeItem posExprItem()
{
  TreeItem pos_expr_item(PosExprPolicy{});
  pos_expr_item.createItem2(TargetBodyPolicy{});
  pos_expr_item.diagram = posExprDiagram();
  {
    TreeItem &local_position_item =
      pos_expr_item.createItem2(LocalPositionPolicy{});
    createXYZChildren(local_position_item);
  }
  {
    TreeItem &global_position_item =
      pos_expr_item.createItem2(GlobalPositionPolicy{});
    createXYZChildren(global_position_item);
    global_position_item.diagram = fromComponentsDiagram();
  }

  return pos_expr_item;
}


struct MotionPassPolicy : SimplePolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor)
  {
    visitor(
      "Add Pos Expr",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,posExprItem());
      }
    );
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Motion Pass");
  }
};


static TreeItem motionPassItem()
{
  return TreeItem(MotionPassPolicy{});
}


struct CharmapperPolicy : SimplePolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor)
  {
    visitor(
      "Add Motion Pass",
      [path,this](TreeOperationHandler &handler){
        handler.addItem(path,motionPassItem());
      }
    );
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Charmapper");
  }
};


static TreeItem charmapperItem()
{
  return TreeItem(CharmapperPolicy{});
}


struct BodyPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Body");
  }
};


static TreeItem bodyItem()
{
  return TreeItem(BodyPolicy{});
}


struct ScenePolicy : SimplePolicy {
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



static TreeItem sceneItem()
{
  return TreeItem(ScenePolicy{});
}


struct RootPolicy : SimplePolicy {
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
        tree.world().addCharmapper();
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

  void visitType(const TreeItem::Visitor &) const
  {
    assert(false);
  }
};


struct EmptyPolicy : SimplePolicy {
  void visitOperations(const Path &,const Tree::OperationVisitor &)
  {
    cerr << "EmptyPolicy::visitOperations()\n";
  }

  void visitType(const TreeItem::Visitor &) const
  {
    assert(false);
  }
};


static TreeItem globalPositionComponentsItems()
{
  TreeItem items(EmptyPolicy{});
  createXYZChildren(items);
  items.diagram = fromComponentsDiagram();
  return items;
}


static TreeItem globalPositionFromBodyItems()
{
  TreeItem items(EmptyPolicy{});
  items.createItem2(SourceBodyPolicy{});
  TreeItem &local_position_item = items.createItem2(LocalPositionPolicy{});
  items.diagram = fromBodyDiagram();
  createXYZChildren(local_position_item);
  return items;
}


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
