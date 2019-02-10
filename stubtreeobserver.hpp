#include "wrapper.hpp"


struct StubTreeObserver : Wrapper::TreeObserver {
  void itemAdded(const TreePath &) override
  {
    assert(false);
  }

  void itemReplaced(const TreePath &) override
  {
  }

  void itemRemoved(const TreePath &) override
  {
    assert(false);
  }

  void itemLabelChanged(const TreePath &) override
  {
    assert(false);
  }

  void itemValueChanged(const TreePath &) override
  {
    assert(false);
  }
};
