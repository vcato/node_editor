#ifndef BASICVARIANT_HPP
#define BASICVARIANT_HPP


#include <utility>
#include <iostream>
#include "createobject.hpp"
#include "noinittag.hpp"


template <typename Policy>
class BasicVariant : public Policy {
  private:
    using Self = BasicVariant;
    using Value = typename Policy::Value;

  public:
    using Type = typename Policy::Type;
    using Policy::Policy;

    BasicVariant()
    : Policy()
    {
    }

    explicit BasicVariant(const BasicVariant& arg)
    : Policy(NoInitTag{})
    {
      _create(arg);
    }

    BasicVariant(BasicVariant&& arg)
    : Policy(NoInitTag{})
    {
      _create(std::move(arg));
    }

    ~BasicVariant()
    {
      _destroy();
    }

    BasicVariant& operator=(BasicVariant&& arg)
    {
      if (this->_type != arg._type) {
        _destroy();
        _create(std::move(arg));
      }
      else {
        Self::withMemberPtrFor(
          this->_type,
          MemberMoveAssignment{this->_value,std::move(arg._value)}
        );
      }

      return *this;
    }

    BasicVariant& operator=(const BasicVariant &arg)
    {
      if (this->_type != arg._type) {
        _destroy();
        _create(arg);
      }
      else {
        Self::withMemberPtrFor(
          this->_type,
          MemberCopyAssignment{this->_value,arg._value}
        );
      }

      return *this;
    }

    bool operator==(const BasicVariant &arg) const
    {
      if (this->_type!=arg._type) return false;

      return
        Self::withMemberPtrFor(
          this->_type,
          MemberEquality{this->_value,arg._value}
        );
    }

    bool operator!=(const BasicVariant &arg) const
    {
      return !operator==(arg);
    }

    Type type() const { return this->_type; }

    template <typename Function>
    auto visit(const Function &f) const
    {
      Self::withMemberPtrFor(
        this->_type,
        [&](auto Value::*member_ptr){ return f(this->_value.*member_ptr); }
      );
    }

  private:
    struct MemberCopyAssignment {
      Value &a;
      const Value &b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        (a.*p) = (b.*p);
      }
    };

    struct MemberMoveAssignment {
      Value &a;
      Value &&b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        (a.*p) = std::move(b.*p);
      }
    };

    struct MemberCopy {
      Value &a;
      const Value &b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        createObject(a.*p,b.*p);
      }
    };

    struct MemberMove {
      Value &a;
      Value &&b;

      template <typename T>
      void operator()(T Value::*p) const
      {
        createObject(a.*p,std::move(b.*p));
      }
    };

    struct MemberEquality {
      const Value &a,&b;

      template <typename T>
      bool operator()(T Value::*p) const
      {
        return (a.*p) == (b.*p);
      }
    };

    struct MemberDestroy {
      Value &_value;

      template <typename T>
      void operator()(T Value::*p) const
      {
        (_value.*p).~T();
      }
    };

    void _create(BasicVariant&& arg)
    {
      this->_type = arg._type;

      Self::withMemberPtrFor(
        this->_type,
        MemberMove{this->_value,std::move(arg._value)}
      );
    }

    void _create(const BasicVariant& arg)
    {
      this->_type = arg._type;

      Self::withMemberPtrFor(
        this->_type,
        MemberCopy{this->_value,arg._value}
      );
    }

    void _destroy()
    {
      Self::withMemberPtrFor(
        this->_type,
        MemberDestroy{this->_value}
      );
    }
};


template <typename T>
void printOn(std::ostream &stream,const T &,int indent_level);


template <typename Policy>
inline void
  printOn(std::ostream &stream,const BasicVariant<Policy> &arg,int indent_level)
{
  arg.visit([&](auto &arg){ printOn(stream,arg,indent_level); });
}


#endif /* BASICVARIANT_HPP */
