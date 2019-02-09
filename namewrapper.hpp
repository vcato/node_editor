struct NameWrapper : NoOperationWrapper<LeafWrapper<StringWrapper>> {
  const char *label_member;
  std::string &name;
  std::function<void()> changed_func;

  NameWrapper(
    const char *label,
    std::string &name_arg,
    std::function<void()> changed_func_arg
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

  void setValue(const std::string &arg) const
  {
    name = arg;
    changed_func();
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
