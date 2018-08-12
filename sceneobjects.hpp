struct BodyObjectData : Object::Data {
  BodyObjectData(Scene::Body *body_ptr_arg)
  : body_ptr(body_ptr_arg)
  {
  }

  virtual Data *clone() { return new BodyObjectData(*this); }

  virtual Optional<Any> member(const std::string &/*member_name*/)
  {
    assert(false);
  }

  virtual ~BodyObjectData() {}

  Scene::Body *body_ptr;
};
