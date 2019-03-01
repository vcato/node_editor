PACKAGES=QtGui QtOpenGL gl glu
CXXFLAGS=-W -Wall -Wundef \
  -pedantic -std=c++14 -I`pkg-config --cflags $(PACKAGES)` \
 -D_GLIBCXX_DEBUG=1 -MD -MP -ggdb3

LDFLAGS=`pkg-config --libs $(PACKAGES)`

all: run_unit_tests build_manual_tests main

run_unit_tests: \
  optional_test.pass \
  maybeint_test.pass \
  any_test.pass \
  diagramstate_test.pass \
  fileaccessor_test.pass \
  charmapperobjects_test.pass \
  evaluateexpression_test.pass \
  evaluatestatement_test.pass \
  evaluatediagram_test.pass \
  diagramexecutor_test.pass \
  diagrameditor_test.pass \
  nodetexteditor_test.pass \
  linetext_test.pass \
  statementtext_test.pass \
  diagramnode_test.pass \
  diagram_test.pass \
  streamparser_test.pass \
  diagramio_test.pass \
  wrapper_test.pass \
  scene_test.pass \
  world_test.pass \
  wrapperstate_test.pass \
  diagramwrapperstate_test.pass \
  observeddiagrams_test.pass \
  scenewrapper_test.pass \
  worldwrapper_test.pass \
  charmapper_test.pass \
  treeeditor_test.pass \
  mainwindow_test.pass

build_manual_tests: \
  qtscenewindow_manualtest \
  qtslider_manualtest \
  qtdiagrameditorwindow_manualtest

OBSERVEDDIAGRAMS = observeddiagrams.o
GENERATENAME = generatename.o
SCENETREE = scenetree.o
SCENEVIEWER = sceneviewer.o
SCENEWINDOW = scenewindow.o $(SCENETREE) $(SCENEVIEWER)
STRINGPARSER = stringparser.o
MAYBEPOINT2D = maybepoint2d.o
CONTAINS = contains.o
EVALUATEEXPRESSION = evaluateexpression.o \
  $(STRINGPARSER) $(MAYBEPOINT2D) $(CONTAINS)
EVALUATESTATEMENT = evaluatestatement.o $(EVALUATEEXPRESSION)
PRINTINDENT = printindent.o
ANYIO = anyio.o $(PRINTINDENT)
DIAGRAMSTATE = diagramstate.o
EVALUATEDIAGRAM = evaluatediagram.o $(EVALUATESTATEMENT) $(ANYIO) $(DIAGRAMSTATE)
SCENE = scene.o $(GENERATENAME)
POINT2DOBJECT=  point2dobject.o
GLOBALVEC = globalvec.o
SCENEOBJECTS = sceneobjects.o $(POINT2DOBJECT) $(GLOBALVEC)
MAKEDIAGRAM = makediagram.o $(DIAGRAMIO)
DEFAULTDIAGRAMS = defaultdiagrams.o $(MAKEDIAGRAM)
CHARMAPPEROBJECTS = charmapperobjects.o
CHARMAPPER = charmapper.o \
  $(DEFAULTDIAGRAMS) $(CHARMAPPEROBJECTS) $(MAYBEPOINT2D) $(SCENEOBJECTS) \
  $(ANY)
DIAGRAMEXECUTOR = diagramexecutor.o
ANY = any.o
WORLD = world.o \
  $(OBSERVEDDIAGRAMS) $(GENERATENAME) $(SCENEWINDOW) $(EVALUATEDIAGRAM) \
  $(DIAGRAMSTATE) $(SCENE) $(SCENEOBJECTS) $(CHARMAPPER) $(DIAGRAMEXECUTOR) \
  $(ANY) $(OBSERVEDDIAGRAM)
QTSLOT = qtslot.o moc_qtslot.o
QTMENU = qtmenu.o $(QTSLOT)
QTTREEWIDGETITEM = qttreewidgetitem.o
QTSLIDER = qtslider.o moc_qtslider.o
QTSPINBOX = qtspinbox.o moc_qtspinbox.o
QTCOMBOBOX = qtcombobox.o moc_qtcombobox.o
QTLINEEDIT = qtlineedit.o moc_qtlineedit.o
TREEUPDATING = treeupdating.o
OBSERVEDDIAGRAM = observeddiagram.o
WRAPPERUTIL = wrapperutil.o $(OBSERVEDDIAGRAM)
DIAGRAM = diagram.o
NODETEXTEDITOR = nodetexteditor.o
LINETEXT = linetext.o
STATEMENTTEXT = statementtext.o
STRINGUTIL = stringutil.o
DIAGRAMNODE = diagramnode.o $(LINETEXT) $(STATEMENTTEXT) $(STRINGUTIL)
CIRCLE = circle.o
DIAGRAMWRAPPERSTATE = diagramwrapperstate.o $(DIAGRAMNODE) $(DIAGRAM)
WRAPPERSTATE = wrapperstate.o $(PRINTINDENT) $(STRINGUTIL) $(STREAMPARSER)
STREAMPARSER = streamparser.o
DIAGRAMIO = diagramio.o $(DIAGRAMWRAPPERSTATE) $(WRAPPERSTATE) $(STREAMPARSER)
DIAGRAMEDITOR = diagrameditor.o \
  $(DIAGRAM) $(NODETEXTEDITOR) $(DIAGRAMNODE) $(CIRCLE) $(DIAGRAMIO)
DIAGRAMEDITORWINDOW = diagrameditorwindow.o $(DIAGRAMEDITOR)
TREEEDITOR = treeeditor.o \
  $(TREEUPDATING) $(WRAPPERUTIL) $(DIAGRAMEDITORWINDOW) $(DIAGRAMEDITOR) \
  $(OBSERVEDDIAGRAM)
DRAW = draw.o
QTDIAGRAMEDITOR = qtdiagrameditor.o moc_qtdiagrameditor.o $(DRAW)
QTDIAGRAMEDITORWINDOW = qtdiagrameditorwindow.o $(QTDIAGRAMEDITOR)
QTTREEEDITOR = qttreeeditor.o moc_qttreeeditor.o \
  $(QTTREEWIDGETITEM) $(QTSLIDER) $(QTSPINBOX) $(QTCOMBOBOX) $(QTLINEEDIT) \
  $(TREEEDITOR) $(QTDIAGRAMEDITORWINDOW)
QTMAINWINDOW = qtmainwindow.o moc_qtmainwindow.o \
  $(QTMENU) $(QTTREEEDITOR) $(MAINWINDOW)
QTSCENETREE = qtscenetree.o
QTSCENEVIEWER = qtsceneviewer.o
QTSCENEWINDOW = qtscenewindow.o $(QTSCENETREE) $(QTSCENEVIEWER)
QTWORLD = qtworld.o $(WORLD) $(QTSCENEWINDOW)
WRAPPER = wrapper.o $(DIAGRAMWRAPPERSTATE)
CHARMAPPERWRAPPER = charmapperwrapper.o
SCENEWRAPPER = scenewrapper.o
WORLDWRAPPER = worldwrapper.o $(CHARMAPPERWRAPPER) $(SCENEWRAPPER)
MAINWINDOW = mainwindow.o
FAKETREEEDITOR = faketreeeditor.o
FAKETREE = faketree.o
FAKEDIAGRAMEDITOR = fakediagrameditor.o
FAKEDIAGRAMEDITORWINDOWS = fakediagrameditorwindows.o
POINT2D = point2d.o
TESTDIAGRAMEVALUATOR = testdiagramevaluator.o \
  $(EVALUATEDIAGRAM) $(DIAGRAMEXECUTOR)

moc_%.cpp: %.hpp
	moc-qt4 $^ >$@

%.pass: %
	./$*
	touch $@

main: main.o \
  $(QTMAINWINDOW) $(QTWORLD) $(WRAPPER) $(WORLDWRAPPER)
	$(CXX) -o $@ $^ $(LDFLAGS)

optional_test: optional_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

maybeint_test: maybeint_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

any_test: any_test.o any.o printindent.o anyio.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramstate_test: diagramstate_test.o diagramstate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

fileaccessor_test: fileaccessor_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

charmapperobjects_test: charmapperobjects_test.o charmapperobjects.o \
  sceneobjects.o maybepoint2d.o point2dobject.o globalvec.o any.o \
  printindent.o scene.o generatename.o point2d.o anyio.o
	$(CXX) -o $@ $^ $(LDFLAGS)

evaluateexpression_test: evaluateexpression_test.o\
  $(EVALUATEEXPRESSION) $(STRINGUTIL) $(CHARMAPPEROBJECTS) $(SCENE) \
  $(SCENEOBJECTS) $(ANYIO) $(ANY)
	$(CXX) -o $@ $^ $(LDFLAGS)

evaluatestatement_test: evaluatestatement_test.o evaluatestatement.o scene.o \
  sceneobjects.o maybepoint2d.o fakeexecutor.o any.o evaluateexpression.o \
  generatename.o point2dobject.o globalvec.o anyio.o stringparser.o \
  contains.o printindent.o
	$(CXX) -o $@ $^ $(LDFLAGS)

evaluatediagram_test: evaluatediagram_test.o diagram.o diagramnode.o \
  linetext.o statementtext.o stringutil.o evaluateexpression.o \
  evaluatediagram.o defaultdiagrams.o diagramio.o makediagram.o \
  maybepoint2d.o point2dobject.o point2d.o any.o printindent.o contains.o \
  diagramexecutor.o anyio.o evaluatestatement.o streamparser.o wrapperstate.o \
  diagramwrapperstate.o stringparser.o diagramstate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramexecutor_test: diagramexecutor_test.o diagramexecutor.o anyio.o \
  printindent.o evaluatediagram.o linetext.o diagram.o stringutil.o \
  evaluateexpression.o diagramnode.o maybepoint2d.o contains.o \
  statementtext.o evaluatestatement.o any.o stringparser.o diagramstate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagrameditor_test: diagrameditor_test.o diagrameditor.o stringutil.o \
  linetext.o diagramnode.o diagram.o circle.o statementtext.o \
  evaluatediagram.o evaluateexpression.o diagramio.o point2d.o \
  vector2d.o any.o printindent.o maybepoint2d.o contains.o anyio.o \
  evaluatestatement.o streamparser.o wrapperstate.o diagramwrapperstate.o \
  fakediagrameditor.o nodetexteditor.o observeddiagram.o stringparser.o \
  diagramstate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

nodetexteditor_test: nodetexteditor_test.o linetext.o stringutil.o \
  diagramnode.o statementtext.o evaluateexpression.o maybepoint2d.o \
  any.o printindent.o contains.o anyio.o nodetexteditor.o stringparser.o
	$(CXX) -o $@ $^ $(LDFLAGS)

linetext_test: linetext_test.o linetext.o stringutil.o evaluateexpression.o \
  any.o printindent.o maybepoint2d.o scene.o sceneobjects.o generatename.o \
  globalvec.o point2dobject.o contains.o fakeexecutor.o anyio.o \
  evaluatestatement.o stringparser.o
	$(CXX) -o $@ $^ $(LDFLAGS)

statementtext_test: statementtext_test.o statementtext.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramnode_test: diagramnode_test.o diagramnode.o linetext.o stringutil.o \
  statementtext.o evaluateexpression.o maybepoint2d.o any.o printindent.o \
  contains.o anyio.o stringparser.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagram_test: diagram_test.o diagram.o diagramnode.o linetext.o stringutil.o \
  statementtext.o evaluatediagram.o evaluateexpression.o any.o \
  printindent.o maybepoint2d.o contains.o fakeexecutor.o anyio.o \
  evaluatestatement.o stringparser.o diagramstate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

streamparser_test: streamparser_test.o streamparser.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramio_test: diagramio_test.o diagramio.o diagram.o diagramnode.o \
  linetext.o statementtext.o stringutil.o evaluateexpression.o makediagram.o \
  maybepoint2d.o any.o printindent.o contains.o anyio.o streamparser.o \
  wrapperstate.o diagramwrapperstate.o stringparser.o
	$(CXX) -o $@ $^ $(LDFLAGS)

wrapper_test: wrapper_test.o wrapper.o diagram.o diagramnode.o linetext.o \
  statementtext.o stringutil.o evaluatediagram.o defaultdiagrams.o \
  diagramio.o wrapperutil.o evaluateexpression.o makediagram.o any.o \
  printindent.o maybepoint2d.o contains.o anyio.o wrapperstate.o \
  evaluatestatement.o streamparser.o diagramwrapperstate.o observeddiagram.o \
  stringparser.o diagramstate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

scene_test: scene_test.o scene.o generatename.o point2d.o
	$(CXX) -o $@ $^ $(LDFLAGS)

world_test: world_test.o $(WORLD)
	$(CXX) -o $@ $^ $(LDFLAGS)

wrapperstate_test: wrapperstate_test.o wrapperstate.o stringutil.o wrapper.o \
  printindent.o streamparser.o diagramwrapperstate.o diagram.o diagramnode.o \
  linetext.o statementtext.o
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramwrapperstate_test: diagramwrapperstate_test.o diagram.o diagramnode.o \
  linetext.o statementtext.o stringutil.o diagramwrapperstate.o \
  wrapperstate.o printindent.o streamparser.o
	$(CXX) -o $@ $^ $(LDFLAGS)

observeddiagrams_test: observeddiagrams_test.o \
  $(OBSERVEDDIAGRAMS) $(OBSERVEDDIAGRAM)
	$(CXX) -o $@ $^ $(LDFLAGS)

scenewrapper_test: scenewrapper_test.o \
  $(SCENEWRAPPER) $(WRAPPERSTATE) $(SCENE) $(WRAPPER) $(WRAPPERUTIL) \
  $(TREEUPDATING) $(FAKETREE)
	$(CXX) -o $@ $^ $(LDFLAGS)

worldwrapper_test: worldwrapper_test.o \
  $(WORLDWRAPPER) $(DIAGRAM) $(DIAGRAMNODE) $(FAKETREE) $(OBSERVEDDIAGRAM) \
  $(WRAPPERUTIL) $(WRAPPERSTATE) $(WRAPPER) $(SCENE) $(WORLD) $(TREEUPDATING) \
  $(TESTDIAGRAMEVALUATOR)
	$(CXX) -o $@ $^ $(LDFLAGS)

charmapper_test: charmapper_test.o \
  $(CHARMAPPER) $(SCENE) $(POINT2D) $(DIAGRAM) $(TESTDIAGRAMEVALUATOR)
	$(CXX) -o $@ $^ $(LDFLAGS)

treeeditor_test: treeeditor_test.o \
  $(OBSERVEDDIAGRAMS) $(TREEEDITOR) $(FAKEDIAGRAMEDITORWINDOWS) \
  $(FAKEDIAGRAMEDITOR) $(FAKETREE) $(WRAPPER)
	$(CXX) -o $@ $^ $(LDFLAGS)

mainwindow_test: mainwindow_test.o \
  $(MAINWINDOW) $(WORLD) $(FAKETREEEDITOR) $(WRAPPERUTIL) $(FAKETREE) \
  $(WRAPPER) $(WORLDWRAPPER) $(DIAGRAMEDITOR) $(FAKEDIAGRAMEDITOR) \
  $(FAKEDIAGRAMEDITORWINDOWS) $(TREEEDITOR)
	$(CXX) -o $@ $^ $(LDFLAGS)


qtscenewindow_manualtest: qtscenewindow_manualtest.o qtscenewindow.o \
  qtsceneviewer.o draw.o scene.o qttreewidgetitem.o qtscenetree.o \
  scenetree.o scenewindow.o generatename.o sceneviewer.o
	$(CXX) -o $@ $^ $(LDFLAGS)

qtslider_manualtest: qtslider_manualtest.o moc_qtslider.o qtslider.o \
  qtlineedit.o moc_qtlineedit.o
	$(CXX) -o $@ $^ $(LDFLAGS)

qtdiagrameditorwindow_manualtest: qtdiagrameditorwindow_manualtest.o \
  qtdiagrameditor.o moc_qtdiagrameditor.o diagrameditor.o diagramio.o \
  qtmenu.o draw.o diagram.o evaluatediagram.o diagramnode.o circle.o \
  qtslot.o moc_qtslot.o linetext.o statementtext.o stringutil.o \
  qtdiagrameditorwindow.o evaluateexpression.o point2d.o vector2d.o \
  diagrameditorwindow.o any.o printindent.o maybepoint2d.o contains.o \
  anyio.o evaluatestatement.o streamparser.o wrapperstate.o \
  diagramwrapperstate.o nodetexteditor.o diagramexecutor.o observeddiagram.o \
  stringparser.o diagramstate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.pass moc_*.cpp

-include *.d
