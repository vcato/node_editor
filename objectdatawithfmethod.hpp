struct ObjectDataWithFMethod : Object::Data {
  ObjectDataWithFMethod(
    std::function<Optional<Any>(const std::vector<Any> &)> f_arg
  ): f(std::move(f_arg))
  {
  }

  Data *clone() override { return new auto(*this); }

  std::string typeName() const override { return "Test"; }

  Any member(const std::string &member_name) const override
  {
    if (member_name=="f") {
      return Function{f};
    }

    std::cerr << "member_name: " << member_name << "\n";
    assert(false);
  }

  std::vector<std::string> memberNames() const override
  {
    return {"f"};
  }

  std::function<Optional<Any>(const std::vector<Any> &)> f;
};
