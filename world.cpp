#include "world.hpp"

#include <iostream>
#include "worldwrapper.hpp"
#include "generatename.hpp"
#include "sceneobjects.hpp"
#include "evaluatediagram.hpp"
#include "diagramexecutor.hpp"

using std::make_unique;
using std::unique_ptr;
using std::cerr;
using std::function;
using std::string;
using std::vector;
using Member = World::Member;
using CharmapperMember = World::CharmapperMember;
using SceneMember = World::SceneMember;



World::World()
: observed_diagrams([&](const Diagram &diagram){
    notifyDiagramChanged(diagram);
  })
{
}

World::~World()
{
}


const Member* World::findMember(const string &name) const
{
  for (auto &member_ptr : world_members) {
    assert(member_ptr);
    if (member_ptr->name==name) {
      return member_ptr.get();
    }
  }

  return nullptr;
}


string World::generateMemberName(const string &prefix) const
{
  auto name_exists_function = [this](const string &name){
    return findMember(name)!=nullptr;
  };
  return generateName(prefix,name_exists_function);
}


Charmapper& World::addCharmapper()
{
  auto charmapper_member_ptr = make_unique<CharmapperMember>();
  charmapper_member_ptr->name = generateMemberName("Charmapper");
  Charmapper &charmapper = charmapper_member_ptr->charmapper;
  world_members.push_back(std::move(charmapper_member_ptr));
  return charmapper;
}


Scene& World::addScene()
{
  unique_ptr<SceneMember> scene_member_ptr = make_unique<SceneMember>();
  scene_member_ptr->name = generateMemberName("Scene");
  Scene& scene = scene_member_ptr->scene;
  SceneWindow &scene_window = createSceneViewerWindow(*scene_member_ptr);
  scene_window.setScenePtr(&scene,scene_member_ptr->name);
  scene_member_ptr->scene_window_ptr = &scene_window;
  world_members.push_back(std::move(scene_member_ptr));
  return scene;
}


void World::visitMember(int child_index,const MemberVisitor &visitor)
{
  assert(world_members[child_index]);
  World::Member &child = *world_members[child_index];
  child.accept(visitor);
}


void
  World::visitMember(int child_index, const ConstMemberVisitor &visitor) const
{
  assert(world_members[child_index]);
  const World::Member &child = *world_members[child_index];
  child.acceptConst(visitor);
}


void
  World::forEachSceneMember(
    function<void(const SceneMember &scene)> function_to_call
  ) const
{
  forEachMemberOfType<SceneMember>(
    [&](const SceneMember &member,int/*member_index*/){
      function_to_call(member);
    }
  );
}


void
  World::forEachSceneMember(
    function<void(World::SceneMember &scene)> function_to_call
  )
{
  forEachMemberOfType<SceneMember>(
    [&](SceneMember &member,int/*member_index*/){
      function_to_call(member);
    }
  );
}


void
  World::forEachCharmapperMember(
    function<void(const World::CharmapperMember &scene)> function_to_call
  ) const
{
  forEachMemberOfType<CharmapperMember>(
    [&](const CharmapperMember &member,int/*member_index*/){
      function_to_call(member);
    }
  );
}


void
  World::forEachCharmapperMember(
    function<void(World::CharmapperMember &,int member_index)> function_to_call
  )
{
  forEachMemberOfType<CharmapperMember>(function_to_call);
}


vector<Charmapper*> World::allCharmapPtrs()
{
  vector<Charmapper *> charmapper_ptrs;

  forEachCharmapperMember(
    [&](CharmapperMember &charmapper_member,int /*member_index*/){
      charmapper_ptrs.push_back(&charmapper_member.charmapper);
    }
  );

  return charmapper_ptrs;
}


void World::applyCharmaps()
{
  applyCharmaps(allCharmapPtrs());
}


namespace {
struct ObservedDiagramEvaluator : AbstractDiagramEvaluator {
  ObservedDiagrams &observed_diagrams;

  ObservedDiagramEvaluator(
    const DiagramExecutionContext &context,
    ObservedDiagrams &observed_diagrams_arg
  )
  : AbstractDiagramEvaluator(context),
    observed_diagrams(observed_diagrams_arg)
  {
  }

  Optional<Any>
    maybeEvaluateWith(
      DiagramState &diagram_state,
      const Diagram &diagram,
      const Environment *parent_environment_ptr,
      const Optional<string> &optional_expected_type_name
    )
  {
    DiagramExecutor executor(context,parent_environment_ptr);
    executor.optional_expected_return_type_name = optional_expected_type_name;
    evaluateDiagram(diagram,executor,diagram_state);
    return std::move(executor.maybe_return_value);
  }

  Optional<Any>
    maybeEvaluate(
      const Diagram &diagram,
      const Environment *parent_environment_ptr,
      const Optional<string> &optional_expected_type_name
    ) override
  {
    ObservedDiagram *maybe_observed_diagram =
      observed_diagrams.findObservedDiagramFor(diagram);

    if (!maybe_observed_diagram) {
      DiagramState temporary_diagram_state;
      return
        maybeEvaluateWith(
          temporary_diagram_state,
          diagram,
          parent_environment_ptr,
          optional_expected_type_name
        );
    }

    ObservedDiagram &observed_diagram = *maybe_observed_diagram;
    observed_diagram.diagram_state = DiagramState();

    Optional<Any> result =
      maybeEvaluateWith(
        observed_diagram.diagram_state,
        diagram,
        parent_environment_ptr,
        optional_expected_type_name
      );

    observed_diagram.notifyObserversThatDiagramStateChanged();
    return result;
  }
};
}


void World::applyCharmaps(const vector<Charmapper*> &charmapper_ptrs)
{
  forEachSceneMember([&](SceneMember &scene_member){
    scene_member.scene.displayFrame() = scene_member.scene.backgroundFrame();
  });

  Environment environment;

  // for each scene, add a scene object to the environment
  forEachSceneMember(
    [&](World::SceneMember &scene_member){
      environment[scene_member.name] = makeSceneObject(scene_member.scene);
    }
  );

  DiagramExecutionContext
    context{/*show_stream*/cerr,/*error_stream*/cerr,&environment};

  ObservedDiagramEvaluator evaluator(context,observed_diagrams);

  for (auto charmapper_ptr : charmapper_ptrs) {
    assert(charmapper_ptr);
    charmapper_ptr->apply(evaluator);
  }

  forEachSceneMember([&](const SceneMember &scene_member){
    if (scene_member.scene_window_ptr) {
      scene_member.scene_window_ptr->notifySceneChanged();
    }
  });
}


CharmapperMember &World::charmapperMember(int index)
{
  Member *member_ptr = world_members[index].get();
  assert(member_ptr);
  CharmapperMember *charmapper_member_ptr =
    dynamic_cast<CharmapperMember*>(member_ptr);
  assert(charmapper_member_ptr);
  return *charmapper_member_ptr;
}


SceneMember &World::sceneMember(int index)
{
  Member *member_ptr = world_members[index].get();
  assert(member_ptr);
  SceneMember *scene_member_ptr =
    dynamic_cast<SceneMember*>(member_ptr);
  assert(scene_member_ptr);
  return *scene_member_ptr;
}


unique_ptr<Member> World::removeMember(int index)
{
  Member *member_ptr = world_members[index].get();

  if (auto scene_member_ptr = dynamic_cast<SceneMember*>(member_ptr)) {
    assert(scene_member_ptr->scene_window_ptr);
    destroySceneViewerWindow(*scene_member_ptr->scene_window_ptr);
  }

  unique_ptr<Member> unique_member_ptr = std::move(world_members[index]);
  world_members.erase(world_members.begin() + index);
  return unique_member_ptr;
}


void World::notifyDiagramChanged(const Diagram &)
{
  applyCharmaps();
}
