using NumericValue = int;
using StringValue = std::string;


struct WrapperValuePolicy {
  struct Void {
    bool operator==(const Void &) const { return true; }
  };

  struct Enumeration
  {
    std::string name;

    bool operator==(const Enumeration &arg) const
    {
      return name==arg.name;
    }
  };

  WrapperValuePolicy()
  : WrapperValuePolicy(Void{})
  {
  }

  WrapperValuePolicy(Void)
  : _type(void_type)
  {
    createObject(_value.void_value,Void{});
  }

  WrapperValuePolicy(NumericValue arg)
  : _type(numeric_type)
  {
    createObject(_value.numeric_value,arg);
  }

  WrapperValuePolicy(StringValue arg)
  : _type(string_type)
  {
    createObject(_value.string_value,arg);
  }

  WrapperValuePolicy(Enumeration arg)
  : _type(enumeration_type)
  {
    createObject(_value.enumeration_value,arg);
  }

  bool isVoid() const { return _type==void_type; }
  bool isString() const { return _type==string_type; }
  bool isNumeric() const { return _type==numeric_type; }
  bool isEnumeration() const { return _type==enumeration_type; }

  const std::string& asString() const
  {
    assert(_type==string_type);
    return _value.string_value;
  }

  const Enumeration &asEnumeration() const
  {
    assert(_type==enumeration_type);
    return _value.enumeration_value;
  }

  NumericValue asNumeric() const
  {
    assert(_type==numeric_type);
    return _value.numeric_value;
  }

  enum Type {
    void_type,
    numeric_type,
    enumeration_type,
    string_type
  };

  protected:
    WrapperValuePolicy(VariantPolicyNoInitTag)
    {
    }

    union Value {
      Value() {}
      ~Value() {}

      Void void_value;
      NumericValue numeric_value;
      Enumeration enumeration_value;
      StringValue string_value;
    };

    template <typename V>
    static auto withMemberPtrFor(Type t,const V& v)
    {
      switch (t) {
        case void_type:  return v(&Value::void_value);
        case numeric_type: return v(&Value::numeric_value);
        case enumeration_type: return v(&Value::enumeration_value);
        case string_type: return v(&Value::string_value);
      }

      assert(false);
    }

    Type _type;
    Value _value;
};


using WrapperValue = BasicVariant<WrapperValuePolicy>;
