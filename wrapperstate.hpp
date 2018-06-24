#include "wrapper.hpp"
#include "basicvariant.hpp"


struct WrapperValuePolicy {
  struct NoInitTag {};
  struct Void {};
  struct Enumeration { EnumerationWrapper::Value index; };

  WrapperValuePolicy()
  : WrapperValuePolicy(Void{})
  {
  }

  WrapperValuePolicy(Void)
  : _type(void_type)
  {
    new (&_value.void_value)Void{};
  }

  WrapperValuePolicy(NumericWrapper::Value arg)
  : _type(numeric_type)
  {
    new (&_value.numeric_value)auto(arg);
  }

  WrapperValuePolicy(StringWrapper::Value arg)
  : _type(string_type)
  {
    new (&_value.string_value)auto(arg);
  }

  WrapperValuePolicy(Enumeration arg)
  : _type(enumeration_type)
  {
    new (&_value.enumeration_value)auto(arg);
  }

  enum Type {
    void_type,
    numeric_type,
    enumeration_type,
    string_type
  };

  protected:
    WrapperValuePolicy(NoInitTag)
    {
    }

    union Value {
      Value() {}
      ~Value() {}

      Void void_value;
      NumericWrapper::Value numeric_value;
      EnumerationWrapper::Value enumeration_value;
      std::string string_value;
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


struct WrapperState {
  std::string label;
  WrapperValue value;
  std::vector<WrapperState> children;
};


extern WrapperState stateOf(const Wrapper &wrapper);
