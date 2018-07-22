#include "wrapper.hpp"

#include <algorithm>


using std::vector;
using std::string;
using std::function;
using std::cerr;


void
  visitEnumerationSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    std::function<void(const EnumerationWrapper &)> f
  )
{
  struct EnumerationVisitor : Wrapper::SubclassVisitor {
    using Function = std::function<void(const EnumerationWrapper &)>;
    Function function;

    EnumerationVisitor(const Function &function_arg)
    : function(function_arg)
    {
    }

    virtual void operator()(const VoidWrapper &) const
    {
      assert(false);
    }

    virtual void operator()(const NumericWrapper &) const
    {
      assert(false);
    }

    virtual void operator()(const EnumerationWrapper &wrapper) const
    {
      function(wrapper);
    }

    virtual void operator()(const StringWrapper &) const
    {
      assert(false);
    }
  };

  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &sub_wrapper){
      sub_wrapper.accept(EnumerationVisitor(f));
    }
  );
}


void
  visitStringSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    const std::function<void(const StringWrapper &)> &f
  )
{
  struct StringVisitor : Wrapper::SubclassVisitor {
    using Function = std::function<void(const StringWrapper &)>;
    Function function;

    StringVisitor(const Function &function_arg)
    : function(function_arg)
    {
    }

    void operator()(const VoidWrapper &) const override
    {
      assert(false);
    }

    void operator()(const NumericWrapper &) const override
    {
      assert(false);
    }

    void operator()(const EnumerationWrapper &) const override
    {
      assert(false);
    }

    void operator()(const StringWrapper &wrapper) const override
    {
      function(wrapper);
    }
  };


  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &wrapper){ wrapper.accept(StringVisitor(f)); }
  );
}


void
  visitNumericSubWrapper(
    const Wrapper &wrapper,
    const TreePath &path,
    const std::function<void(const NumericWrapper &)> &f
  )
{
  struct NumericVisitor : Wrapper::SubclassVisitor {
    using Function = std::function<void(const NumericWrapper &)>;
    Function function;

    NumericVisitor(const Function &function_arg)
    : function(function_arg)
    {
    }

    virtual void operator()(const VoidWrapper &) const
    {
      assert(false);
    }

    virtual void operator()(const NumericWrapper &wrapper) const
    {
      function(wrapper);
    }

    virtual void operator()(const EnumerationWrapper &) const
    {
      assert(false);
    }

    virtual void operator()(const StringWrapper &) const
    {
      assert(false);
    }
  };

  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &wrapper){ wrapper.accept(NumericVisitor(f)); }
  );
}


static string makeTag(string label)
{
  std::transform(label.begin(),label.end(),label.begin(),::tolower);
  std::replace(label.begin(),label.end(),' ','_');
  return label;
}


static WrapperValue valueOf(const Wrapper &wrapper)
{
  WrapperValue value;

  struct Visitor : Wrapper::SubclassVisitor {
    WrapperValue &value;

    Visitor(WrapperValue &value_arg)
    : value(value_arg)
    {
    }

    void operator()(const VoidWrapper &) const override
    {
      value = WrapperValue::Void{};
    }

    void operator()(const NumericWrapper &numeric_wrapper) const override
    {
      value = numeric_wrapper.value();
    }

    void
      operator()(const EnumerationWrapper &enumeration_wrapper) const override
    {
      const auto &names = enumeration_wrapper.enumerationNames();
      const auto index = enumeration_wrapper.value();
      value = WrapperValue::Enumeration{makeTag(names[index])};
    }

    void operator()(const StringWrapper &string_wrapper) const override
    {
      value = string_wrapper.value();
    }
  };

  Visitor visitor(value);

  wrapper.accept(visitor);

  return value;
}


WrapperState stateOf(const Wrapper &wrapper)
{
  WrapperState result(makeTag(wrapper.label()));
  result.value = valueOf(wrapper);

  for (int i=0, n=wrapper.nChildren(); i!=n; ++i) {
    wrapper.withChildWrapper(
      i,
      [&](const Wrapper &child){
        result.children.push_back(stateOf(child));
      }
    );
  }

  return result;
}
