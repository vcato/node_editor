struct Any {
  enum Type {
    void_type,
    float_type
  };

  union {
    float float_value;
  };

  Any()
  : type(void_type)
  {
  }

  Any(float arg)
  : type(float_type)
  {
    new (&float_value)float(arg);
  }

  template <typename T> friend const T& any_cast(const Any &);

  template <typename T>
  const T& as() const
  {
    return any_cast<T>(*this);
  }

  ~Any()
  {
    switch (type) {
      case float_type:
        using T = float;
        float_value.~T();
        break;
      case void_type:
        break;
    }
  }

  Type type;
};


template<>
inline const float& any_cast<float>(const Any &arg)
{
  assert(arg.type==Any::float_type);
  return arg.float_value;
}
