#ifndef WRAPPERSTATE_HPP
#define WRAPPERSTATE_HPP


#include "wrapper.hpp"
#include "basicvariant.hpp"
#include "createobject.hpp"


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

  WrapperValuePolicy(NumericWrapper::Value arg)
  : _type(numeric_type)
  {
    createObject(_value.numeric_value,arg);
  }

  WrapperValuePolicy(StringWrapper::Value arg)
  : _type(string_type)
  {
    createObject(_value.string_value,arg);
  }

  WrapperValuePolicy(Enumeration arg)
  : _type(enumeration_type)
  {
    createObject(_value.enumeration_value,arg);
  }

  bool isString() const { return _type==string_type; }
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
      NumericWrapper::Value numeric_value;
      Enumeration enumeration_value;
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
  using Tag = std::string;
  Tag tag;
  WrapperValue value;
  std::vector<WrapperState> children;

  WrapperState(const Tag &tag_arg)
  : tag(tag_arg)
  {
  }

  bool operator==(const WrapperState &arg) const
  {
    if (tag!=arg.tag) return false;
    if (value!=arg.value) return false;
    if (children!=arg.children) return false;
    return true;
  }
};


struct ScanStateResultPolicy {
  struct Error {
    std::string message;
  };

  union Value {
    WrapperState state;
    Error error;

    Value()
    {
    }

    ~Value()
    {
      // This value should have been destructed at this point
    }
  };

  enum class Type {
    state,
    error
  };

  ScanStateResultPolicy(const Error &error)
  : _type(Type::error)
  {
    createObject(_value.error,error);
  }

  ScanStateResultPolicy(const WrapperState &state)
  : _type(Type::state)
  {
    createObject(_value.state,state);
  }

  template <typename Function>
  void withMemberPtrFor(Type type,const Function &f)
  {
    switch (type) {
      case Type::state: return f(&Value::state);
      case Type::error: return f(&Value::error);
    }

    assert(false);
  }

  ScanStateResultPolicy(VariantPolicyNoInitTag)
  {
  }

  bool isState() const
  {
    return _type==Type::state;
  }

  const WrapperState &state() const
  {
    assert(_type==Type::state);
    return _value.state;
  }

  Type _type;
  Value _value;
};


struct ScanStateResult : BasicVariant<ScanStateResultPolicy> {
  using BasicVariant<ScanStateResultPolicy>::BasicVariant;

  static ScanStateResult error(const std::string &message)
  {
    return Error{message};
  }
};


extern WrapperState stateOf(const Wrapper &wrapper);

extern void
  printStateOn(std::ostream &stream,const WrapperState &state,int indent = 0);

#if 1
extern ScanStateResult scanStateFrom(std::istream &stream);
#endif



#endif /* WRAPPERSTATE_HPP */
