#include "wrapperutil.hpp"

#include "streamvector.hpp"


using std::cerr;
using std::vector;
using std::string;
using std::ostream;
using std::function;


static void
  printOperations(
    ostream &stream,
    const Wrapper &sub_wrapper
  )
{
  stream << sub_wrapper.label() << " operations:\n";

  for (auto &operation_name : sub_wrapper.operationNames()) {
    stream << operation_name << "\n";
  }
}


int
  operationIndex(
    const Wrapper &sub_wrapper,
    const string &operation_name
  )
{
  vector<string> operation_names = sub_wrapper.operationNames();
  int n_operations = operation_names.size();

  for (int index = 0; index!=n_operations; ++index) {
    if (operation_names[index]==operation_name) {
      return index;
    }
  }

  cerr << "Could not find " << operation_name << " operation\n";
  printOperations(cerr,sub_wrapper);
  assert(false);
}


int
  operationIndex2(
    const Wrapper &world_wrapper,
    const TreePath &path,
    const std::string &operation_name
  )
{
  int result = -1;

  visitSubWrapper(
    world_wrapper,
    path,
    [&](const Wrapper &sub_wrapper){
      result = operationIndex(sub_wrapper,operation_name);
    }
  );

  return result;
}


void
  executeOperation(
    const Wrapper &sub_wrapper,
    const TreePath &path,
    const string &operation_name,
    Wrapper::TreeObserver &tree_observer
  )
{
  int operation_index = operationIndex(sub_wrapper,operation_name);
  sub_wrapper.executeOperation(operation_index,path,tree_observer);
}


void
  executeOperation2(
    const Wrapper &world_wrapper,
    const TreePath &scene_path,
    const string &operation_name,
    Wrapper::TreeObserver &tree_observer
  )
{
  visitSubWrapper(
    world_wrapper,
    scene_path,
    [&](const Wrapper &sub_wrapper){
      executeOperation(sub_wrapper,scene_path,operation_name,tree_observer);
    }
  );
}


static const char *addBodyOperationName()
{
  return "Add Body";
}


void
  executeAddBodyFunction(
    const Wrapper &sub_wrapper,
    const TreePath &path,
    Wrapper::TreeObserver &tree_observer
  )
{
  executeOperation(
    sub_wrapper,
    path,
    addBodyOperationName(),
    tree_observer
  );
}


void
  executeAddBodyFunction2(
    const Wrapper &world_wrapper,
    const TreePath &scene_path,
    Wrapper::TreeObserver &tree_observer
  )
{
  executeOperation2(
    world_wrapper,
    scene_path,
    addBodyOperationName(),
    tree_observer
  );
}


Diagram *diagramPtr(const Wrapper &wrapper,const TreePath &path)
{
  Diagram *result_ptr = 0;

  visitSubWrapper(
    wrapper,
    path,
    [&result_ptr](const Wrapper &wrapper){ result_ptr = wrapper.diagramPtr(); }
  );

  if (!result_ptr) {
    cerr << "No diagram found for " << path << "\n";
    return nullptr;
  }

  return result_ptr;
}


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


static int
  indexOfChildWithLabel(
    const Wrapper &wrapper,
    const string &label,
    int which_occurance
  )
{
  int index = -1;
  int n_children = wrapper.nChildren();
  int occurance = 0;

  for (int i=0; i!=n_children; ++i) {
    wrapper.withChildWrapper(i,[&](const Wrapper &child_wrapper){
      if (child_wrapper.label()==label) {
        ++occurance;

        if (occurance==which_occurance) {
          assert(index==-1);
          index = i;
        }
      }
    });
  }

  if (index<0) {
    cerr << "Couldn't find label '" << label << "'-" << which_occurance << "\n";

    for (int i=0; i!=n_children; ++i) {
      wrapper.withChildWrapper(i,[&](const Wrapper &child_wrapper){
        cerr << "  " << child_wrapper.label() << "\n";
      });
    }
    assert(false);
  }

  return index;
}


static int indexOfChildWithLabel(const Wrapper &wrapper,const string &label)
{
  auto index = label.find('-');

  if (index!=label.npos) {
    const string &base_label = label.substr(0,index);
    const string &occurance_string = label.substr(index+1);
    int occurance = std::stoi(occurance_string);
    return indexOfChildWithLabel(wrapper,base_label,occurance);
  }

  return indexOfChildWithLabel(wrapper,label,1);
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
  executeOperation(
    const Wrapper &wrapper,
    const string &path_string,
    const string &operation_name,
    Wrapper::TreeObserver &tree_observer
  )
{
  TreePath path = makePath(wrapper,path_string);
  executeOperation2(wrapper,path,operation_name,tree_observer);
}
