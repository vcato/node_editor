#include "wrapper.hpp"


using std::vector;
using std::string;
using std::function;
using std::cerr;


static vector<string> split(const string &arg)
{
  string rest = arg;
  vector<string> result;

  for (;;) {
    size_t pos = rest.find('|');
    if (pos==rest.npos) {
      result.push_back(rest);
      break;
    }
    string component = rest.substr(0,pos);
    result.push_back(component);
    rest = rest.substr(pos+1);
  }

  return result;
}


static int indexOfChildWithLabel(const Wrapper &wrapper,const string &label)
{
  int index = -1;
  int n_children = wrapper.nChildren();

  for (int i=0; i!=n_children; ++i) {
    wrapper.withChildWrapper(i,[&](const Wrapper &child_wrapper){
      if (child_wrapper.label()==label) {
        assert(index==-1);
        index = i;
      }
    });
  }

  if (index<0) {
    cerr << "Couldn't find label " << label << "\n";

    for (int i=0; i!=n_children; ++i) {
      wrapper.withChildWrapper(i,[&](const Wrapper &child_wrapper){
        cerr << "  " << child_wrapper.label() << "\n";
      });
    }
    assert(false);
  }

  return index;
}


TreePath makePath(const Wrapper &wrapper,const string &path_string)
{
  if (path_string=="") {
    return {};
  }
  else {
    vector<string> components = split(path_string);
    size_t component_index = 0;
    TreePath result;

    function<void (const Wrapper &)> add_remaining_components_function =
      [&](const Wrapper &child_wrapper){
        int index =
          indexOfChildWithLabel(child_wrapper,components[component_index]);
        assert(index>=0);
        result.push_back(index);
        ++component_index;

        if (components.size()!=component_index) {
          child_wrapper.withChildWrapper(index,
            add_remaining_components_function
          );
        }
      };

    add_remaining_components_function(wrapper);

    return result;
  }
}


void
  visitEnumeration(
    const Wrapper &wrapper,
    const TreePath &path,
    std::function<void(const EnumerationWrapper &)> f
  )
{
  visitSubWrapper(
    wrapper,
    path,
    [&](const Wrapper &sub_wrapper){
      sub_wrapper.accept(EnumerationVisitor(f));
    }
  );
}
