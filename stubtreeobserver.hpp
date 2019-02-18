#include "wrapper.hpp"


struct StubTreeObserver : Wrapper::TreeObserver {
  void itemAdded(const TreePath &) override
  {
  }

  void itemReplaced(const TreePath &) override
  {
  }

  void itemRemoved(const TreePath &) override
  {
  }

  void itemLabelChanged(const TreePath &) override
  {
  }

  void itemValueChanged(const TreePath &) override
  {
  }
};
