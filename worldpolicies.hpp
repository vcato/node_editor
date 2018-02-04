#include "defaultdiagrams.hpp"
#include "tree.hpp"


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


struct LocalPositionPolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Local Position");
  }

  Diagram defaultDiagram();

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


struct MotionPassPolicy : SimplePolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor);

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Motion Pass");
  }
};


struct CharmapperPolicy : SimplePolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor);

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Charmapper");
  }
};


struct BodyPolicy : SimplePolicy {
  void visitOperations(const Path &,const OperationVisitor &)
  {
  }

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Body");
  }
};


struct ScenePolicy : SimplePolicy {
  void visitOperations(const Path &path,const OperationVisitor &visitor);

  void visitType(const TreeItem::Visitor &visitor) const
  {
    visitor.voidItem("Scene");
  }
};



struct RootPolicy : SimplePolicy {
  Tree &tree;

  RootPolicy(Tree &tree_arg)
  : tree(tree_arg)
  {
  }

  ~RootPolicy()
  {
  }

  void visitOperations(const Path &path,const Tree::OperationVisitor &visitor);

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


extern void
  visitRootOperations(
    const Path &path,
    const Tree::OperationVisitor &visitor,
    WorldInterface &world
  );
}
