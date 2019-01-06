#ifndef ANY_HPP_
#define ANY_HPP_

#include <cassert>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include "contains.hpp"
#include "optional.hpp"
#include "basicvariant.hpp"


struct AnyPolicy;
struct Object;

using Any = BasicVariant<AnyPolicy>;

class Class {
  public:
    using NamedParameters = std::map<std::string,Any>;
    using MakeObjectSignature =
      Optional<Object>(const NamedParameters &,std::ostream &error_stream);
    using MakeObjectFunction = std::function<MakeObjectSignature>;

    Class(MakeObjectFunction make_object_function_arg)
    : make_object_function(std::move(make_object_function_arg))
    {
    }

    bool operator==(const Class &) const;

    Optional<Object>
      maybeMakeObject(
        const NamedParameters &parameters,
        std::ostream &error_stream
      ) const;

  private:
    MakeObjectFunction make_object_function;
};


// This is the base class for dynamic objects that can hold arbitrary
// members with dynamic values.
class Object {
  public:
    struct Data {
      virtual Data *clone() = 0;
      virtual std::string typeName() const = 0;
      virtual Any member(const std::string &member_name) const = 0;
      virtual std::vector<std::string> memberNames() const = 0;
      virtual ~Data() {}
    };

    Object(std::unique_ptr<Data> data_ptr_arg)
    : data_ptr(std::move(data_ptr_arg))
    {
      assert(data_ptr);
    }

    Object(const Object &arg)
    : data_ptr(arg.data_ptr->clone())
    {
    }

    ~Object()
    {
    }

    const Data &data() const
    {
      assert(data_ptr);
      return *data_ptr;
    }

    inline Optional<Any> maybeMember(const std::string &member_name) const;

    Object &operator=(const Object &arg);

    bool operator==(const Object &/*arg*/) const
    {
      // This isn't right, but I'm not sure what to do about it yet.
      return true;
    }

  private:
    std::unique_ptr<Data> data_ptr;
};


struct Function {
  using FunctionMember =
    std::function<Optional<Any>(const std::vector<Any> &)>;
  FunctionMember function_member;

  inline Optional<Any> operator()(const std::vector<Any> &parameters) const;

  bool operator==(const Function &) const
  {
    // Not sure about function equality yet.  We can't compare std::functions.
    assert(false);
  }
};


struct AnyPolicy {
  public:
    enum Type {
      void_type,
      float_type,
      string_type,
      vector_type,
      object_type,
      class_ptr_type,
      function_type
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

    AnyPolicy(const std::string &arg)
    : _type(string_type)
    {
      createObject(_value.string_value,arg);
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

    AnyPolicy(Function arg)
    : _type(function_type)
    {
      createObject(_value.function_value,std::move(arg));
    }

    bool isVoid() const { return _type==void_type; }
    bool isVector() const { return _type==vector_type; }
    bool isFloat() const { return _type==float_type; }
    bool isString() const { return _type==string_type; }
    bool isObject() const { return _type==object_type; }
    bool isClassPtr() const { return _type==class_ptr_type; }
    bool isFunction() const { return _type==function_type; }

    std::string typeName() const
    {
      switch (_type) {
        case void_type: return "void";
        case vector_type: return "vector";
        case string_type: return "string";
        case float_type: return "float";
        case object_type: return "object";
        case class_ptr_type: return "class_ptr";
        case function_type: return "function";
      }

      assert(false);
      return "unknown";
    }

    Void asVoid() const
    {
      assert(_type==void_type);
      return _value.void_value;
    }

    float asFloat() const
    {
      assert(_type==float_type);
      return _value.float_value;
    }

    const std::string &asString() const
    {
      assert(_type==string_type);
      return _value.string_value;
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

    const Function &asFunction() const
    {
      assert(_type==function_type);
      return _value.function_value;
    }

  protected:
    union Value {
      Void void_value;
      float float_value;
      std::string string_value;
      std::vector<Any> vector_value;
      Object object_value;
      Class *class_ptr_value;
      Function function_value;

      Value() {}
      ~Value() {}
    };

    template <typename V>
    static auto withMemberPtrFor(Type t,const V& v)
    {
      switch (t) {
        case float_type:  return v(&Value::float_value);
        case void_type:   return v(&Value::void_value);
        case string_type: return v(&Value::string_value);
        case vector_type: return v(&Value::vector_value);
        case object_type: return v(&Value::object_value);
        case class_ptr_type: return v(&Value::class_ptr_value);
        case function_type: return v(&Value::function_value);
      }

      assert(false);
    }

    Type _type;
    Value _value;
};


inline Optional<Any> Object::maybeMember(const std::string &member_name) const
{
  assert(data_ptr);

  if (!contains(data_ptr->memberNames(),member_name)) {
    return {};
  }

  return data_ptr->member(member_name);
}


inline Optional<Any>
  Function::operator()(const std::vector<Any> &parameters) const
{
  assert(function_member);
  return function_member(parameters);
}

#endif /* ANY_HPP_ */
