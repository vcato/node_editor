#ifndef WORLDPOLICIES_HPP_
#define WORLDPOLICIES_HPP_

#include "defaultdiagrams.hpp"
#include "tree.hpp"


namespace world_policies {

using Path = Tree::Path;
using OperationVisitor = Tree::OperationVisitor;
using std::vector;
using std::string;
using std::cerr;

struct SimplePolicy {
};


struct XPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.numericItem("X");
  }
};


struct YPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.numericItem("Y");
  }
};


struct ZPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.numericItem("Z");
  }
};


struct GlobalPositionPolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    vector<string> enumeration_names = {"Components","From Body"};
    visitor.enumeratedItem("Global Position",enumeration_names);
  }
};


struct LocalPositionPolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Local Position");
  }
};


struct SourceBodyPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    vector<string> enumeration_names = {"Body1","Body2","Body3"};
    visitor.enumeratedItem("Source Body",enumeration_names);
  }
};


struct TargetBodyPolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    vector<string> enumeration_names = {"Body1","Body2","Body3"};
    visitor.enumeratedItem("Target Body",enumeration_names);
  }
};


struct PosExprPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Pos Expr");
  }
};


struct MotionPassPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Motion Pass");
  }
};


struct CharmapperPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Charmapper");
  }
};


struct BodyPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
  {
    visitor.voidItem("Body");
  }
};


struct ScenePolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &visitor) const
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

  void visitType(const TreeItem::TypeVisitor &) const
  {
    assert(false);
  }
};


struct EmptyPolicy : SimplePolicy {
  void visitType(const TreeItem::TypeVisitor &) const
  {
    assert(false);
  }
};

}

#endif /* WORLDPOLICIES_HPP_ */
