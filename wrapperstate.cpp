#include "wrapperstate.hpp"


static WrapperValue valueOf(const Wrapper &wrapper)
{
  WrapperValue value;

  struct Visitor : Wrapper::Visitor {
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
      value = WrapperValue::Enumeration{enumeration_wrapper.value()};
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
  WrapperState result;
  result.label = wrapper.label();
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
