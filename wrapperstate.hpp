#ifndef WRAPPERSTATE_HPP
#define WRAPPERSTATE_HPP


#include <cassert>
#include <vector>
#include "basicvariant.hpp"
#include "createobject.hpp"
#include "wrappervalue.hpp"


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

  bool isError() const
  {
    return _type==Type::error;
  }

  const Error &asError() const
  {
    assert(_type==Type::error);
    return _value.error;
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


extern void
  printStateOn(std::ostream &stream,const WrapperState &state,int indent = 0);

extern ScanStateResult scanStateFrom(std::istream &stream);



#endif /* WRAPPERSTATE_HPP */
