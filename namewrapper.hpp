struct NameWrapper : NoOperationWrapper<LeafWrapper<StringWrapper>> {
  const char *label_member;
  std::string &name;
  using ChangedFunc = std::function<void(const TreePath &,TreeObserver &)>;
  ChangedFunc changed_func;

  NameWrapper(
    const char *label,
    std::string &name_arg,
    ChangedFunc changed_func_arg
  )
  : label_member(label),
    name(name_arg),
    changed_func(changed_func_arg)
  {
  }

  Label label() const override { return label_member; }

  virtual std::string value() const
  {
    return name;
  }

  void
    setValue(
      const std::string &arg,
      const TreePath &path,
      TreeObserver &tree_observer
    ) const
  {
    name = arg;
    changed_func(path,tree_observer);
  }

  void setState(const WrapperState &new_state) const override
  {
    if (new_state.value.isString()) {
      name = new_state.value.asString();
    }
    else {
      assert(false);
    }
  }
};
