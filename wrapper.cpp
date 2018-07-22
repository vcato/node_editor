#include "wrapper.hpp"


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
