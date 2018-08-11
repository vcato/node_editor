#ifndef ANY_HPP_
#define ANY_HPP_

#include <cassert>
#include <vector>
#include <iosfwd>
#include <functional>
#include "printonany.hpp"
#include "optional.hpp"
#include "basicvariant.hpp"


struct AnyPolicy;
struct Object;

using Any = BasicVariant<AnyPolicy>;

struct Class {
  using MakeObjectFunction = std::function<Object(const Class &)>;
  bool operator==(const Class &) const { return true; }

  Class(MakeObjectFunction make_object_function_arg)
  : make_object_function(std::move(make_object_function_arg))
  {
  }

  MakeObjectFunction make_object_function;
};


struct Object {
  struct Data {
    virtual Data *clone() = 0;
    virtual ~Data() {}
  };

  Data *data_ptr;

  Object(const Class *,Data &data)
  : data_ptr(&data)
  {
  }

  Object(const Object &arg)
  : data_ptr(arg.data_ptr->clone())
  {
  }

  ~Object()
  {
    delete data_ptr;
  }

  Object &operator=(const Object &/*arg*/)
  {
    assert(false);
    return *this;
  }

  bool operator==(const Object &/*arg*/) const
  {
    // This isn't right, but I'm not sure what to do about it yet.
    return true;
  }
};


struct AnyPolicy {
  public:
    enum Type {
      void_type,
      float_type,
      vector_type,
      object_type,
      class_ptr_type
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

    AnyPolicy(Class *arg)
    : _type(class_ptr_type)
    {
      createObject(_value.class_ptr_value,arg);
    }

    bool isVoid() const { return _type==void_type; }
    bool isVector() const { return _type==vector_type; }
    bool isFloat() const { return _type==float_type; }
    bool isObject() const { return _type==object_type; }
    bool isClassPtr() const { return _type==class_ptr_type; }

    std::string typeName() const
    {
      switch (_type) {
        case void_type: return "void";
        case vector_type: return "vector";
        case float_type: return "float";
        case object_type: return "object";
        case class_ptr_type: return "class_ptr";
      }

      assert(false);
      return "unknown";
    }

    const std::vector<Any> &asVector() const
    {
      assert(_type==vector_type);
      return _value.vector_value;
    }

    const Object &asObject() const
    {
      assert(_type==object_type);
      return _value.object_value;
    }

    const Class *asClassPtr() const
    {
      assert(_type==class_ptr_type);
      return _value.class_ptr_value;
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
      Class *class_ptr_value;

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
        case class_ptr_type: return v(&Value::class_ptr_value);
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
inline void printOn(std::ostream &,Class *const &)
{
  assert(false);
}


inline std::ostream& operator<<(std::ostream &stream,const Any &arg)
{
  printOn(stream,arg);
  return stream;
}


#endif /* ANY_HPP_ */
