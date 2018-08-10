#ifndef ANY_HPP_
#define ANY_HPP_

#include <cassert>
#include <vector>
#include <iosfwd>
#include "printonany.hpp"
#include "basicvariant.hpp"

struct AnyPolicy;

using Any = BasicVariant<AnyPolicy>;

struct Object {
  bool operator==(const Object &) const { return true; }
};

struct Class {
  bool operator==(const Class &) const { return true; }
};

struct AnyPolicy {
  public:
    enum Type {
      void_type,
      float_type,
      vector_type,
      object_type,
      class_type
    };

    struct Void {
      bool operator==(const Void &) const { return true; }
    };

    AnyPolicy(NoInitTag)
    {
    }

    AnyPolicy()
    : _type(void_type)
    {
      createObject(_value.void_value,Void());
    }

    AnyPolicy(float arg)
    : _type(float_type)
    {
      createObject(_value.float_value,arg);
    }

    AnyPolicy(std::vector<Any> &&arg)
    : _type(vector_type)
    {
      createObject(_value.vector_value,std::move(arg));
    }

    AnyPolicy(Object &&arg)
    : _type(object_type)
    {
      createObject(_value.object_value,std::move(arg));
    }

    AnyPolicy(Class &&arg)
    : _type(class_type)
    {
      createObject(_value.class_value,std::move(arg));
    }

    bool isVoid() const { return _type==void_type; }
    bool isVector() const { return _type==vector_type; }
    bool isFloat() const { return _type==float_type; }
    bool isObject() const { return _type==object_type; }
    bool isClass() const { return _type==class_type; }

    std::string typeName() const
    {
      switch (_type) {
        case void_type: return "void";
        case vector_type: return "vector";
        case float_type: return "float";
        case object_type: return "object";
        case class_type: return "class";
      }

      assert(false);
      return "unknown";
    }

    const std::vector<Any> &asVector() const
    {
      assert(_type==vector_type);
      return _value.vector_value;
    }

    float asFloat() const
    {
      assert(_type==float_type);
      return _value.float_value;
    }

    Void asVoid() const
    {
      assert(_type==void_type);
      return _value.void_value;
    }

  protected:
    union Value {
      Void void_value;
      float float_value;
      std::vector<Any> vector_value;
      Object object_value;
      Class class_value;

      Value() {}
      ~Value() {}
    };

    template <typename V>
    static auto withMemberPtrFor(Type t,const V& v)
    {
      switch (t) {
        case float_type:  return v(&Value::float_value);
        case void_type:   return v(&Value::void_value);
        case vector_type: return v(&Value::vector_value);
        case object_type: return v(&Value::object_value);
        case class_type: return v(&Value::class_value);
      }

      assert(false);
    }

    Type _type;
    Value _value;
};


template <>
inline void printOn(std::ostream &stream,const float &arg)
{
  stream << arg;
}

template <>
inline void printOn(std::ostream &stream,const Any::Void &)
{
  stream << "void()";
}


template <>
inline void printOn(std::ostream &stream,const std::vector<Any> &arg)
{
  stream << "[";

  auto iter = arg.begin();

  if (iter!=arg.end()) {
    printOn(stream,*iter);
    ++iter;
  }

  while (iter!=arg.end()) {
    stream << ",";
    printOn(stream,*iter);
    ++iter;
  }

  stream << "]";
}


template <>
inline void printOn(std::ostream &,const Object &)
{
  assert(false);
}


template <>
inline void printOn(std::ostream &,const Class &)
{
  assert(false);
}


inline std::ostream& operator<<(std::ostream &stream,const Any &arg)
{
  printOn(stream,arg);
  return stream;
}


#endif /* ANY_HPP_ */
