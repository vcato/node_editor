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

FAKEEXECUTOR = fakeexecutor.o
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
ANY = any.o
STRINGUTIL = stringutil.o
LINETEXT = linetext.o $(STRINGUTIL)
STATEMENTTEXT = statementtext.o
DIAGRAMNODE = diagramnode.o $(LINETEXT) $(STATEMENTTEXT) $(STRINGUTIL)
DIAGRAM = diagram.o $(DIAGRAMNODE)
EVALUATEDIAGRAM = evaluatediagram.o \
  $(EVALUATESTATEMENT) $(ANYIO) $(DIAGRAMSTATE) $(DIAGRAM) $(ANY)
SCENE = scene.o $(GENERATENAME)
POINT2DOBJECT=  point2dobject.o
GLOBALVEC = globalvec.o
SCENEOBJECTS = sceneobjects.o $(POINT2DOBJECT) $(GLOBALVEC) $(SCENE)
STREAMPARSER = streamparser.o
WRAPPERSTATE = wrapperstate.o $(PRINTINDENT) $(STRINGUTIL) $(STREAMPARSER)
DIAGRAMWRAPPERSTATE = diagramwrapperstate.o $(DIAGRAM) $(WRAPPERSTATE)
DIAGRAMIO = diagramio.o $(DIAGRAMWRAPPERSTATE) $(WRAPPERSTATE) $(STREAMPARSER)
MAKEDIAGRAM = makediagram.o $(DIAGRAMIO)
DEFAULTDIAGRAMS = defaultdiagrams.o $(MAKEDIAGRAM)
CHARMAPPEROBJECTS = charmapperobjects.o $(SCENEOBJECTS) $(MAYBEPOINT2D)
CHARMAPPER = charmapper.o \
  $(DEFAULTDIAGRAMS) $(CHARMAPPEROBJECTS) $(MAYBEPOINT2D) $(SCENEOBJECTS) \
  $(ANY)
DIAGRAMEXECUTOR = diagramexecutor.o $(ANY)
OBSERVEDDIAGRAM = observeddiagram.o
WORLD = world.o \
  $(OBSERVEDDIAGRAMS) $(GENERATENAME) $(SCENEWINDOW) $(EVALUATEDIAGRAM) \
  $(DIAGRAMSTATE) $(SCENE) $(SCENEOBJECTS) $(CHARMAPPER) $(DIAGRAMEXECUTOR) \
  $(ANY) $(OBSERVEDDIAGRAM)
QTSLOT = qtslot.o moc_qtslot.o
QTMENU = qtmenu.o $(QTSLOT)
QTTREEWIDGETITEM = qttreewidgetitem.o
QTLINEEDIT = qtlineedit.o moc_qtlineedit.o
QTSLIDER = qtslider.o moc_qtslider.o $(QTLINEEDIT)
QTSPINBOX = qtspinbox.o moc_qtspinbox.o
QTCOMBOBOX = qtcombobox.o moc_qtcombobox.o
TREEUPDATING = treeupdating.o
WRAPPERUTIL = wrapperutil.o $(OBSERVEDDIAGRAM)
NODETEXTEDITOR = nodetexteditor.o
CIRCLE = circle.o
DIAGRAMEDITOR = diagrameditor.o \
  $(DIAGRAM) $(NODETEXTEDITOR) $(DIAGRAMNODE) $(CIRCLE) $(DIAGRAMIO)
DIAGRAMEDITORWINDOW = diagrameditorwindow.o $(DIAGRAMEDITOR)
TREEEDITOR = treeeditor.o \
  $(TREEUPDATING) $(WRAPPERUTIL) $(DIAGRAMEDITORWINDOW) $(DIAGRAMEDITOR) \
  $(OBSERVEDDIAGRAM)
DRAW = draw.o
QTDIAGRAMEDITOR = qtdiagrameditor.o moc_qtdiagrameditor.o $(DRAW) $(QTMENU)
QTDIAGRAMEDITORWINDOW = qtdiagrameditorwindow.o $(QTDIAGRAMEDITOR)
QTTREEEDITOR = qttreeeditor.o moc_qttreeeditor.o \
  $(QTTREEWIDGETITEM) $(QTSLIDER) $(QTSPINBOX) $(QTCOMBOBOX) $(QTLINEEDIT) \
  $(TREEEDITOR) $(QTDIAGRAMEDITORWINDOW)
MAINWINDOW = mainwindow.o
QTMAINWINDOW = qtmainwindow.o moc_qtmainwindow.o \
  $(QTMENU) $(QTTREEEDITOR) $(MAINWINDOW)
QTSCENETREE = qtscenetree.o $(QTTREEWIDGETITEM)
QTSCENEVIEWER = qtsceneviewer.o $(DRAW)
QTSCENEWINDOW = qtscenewindow.o $(QTSCENETREE) $(QTSCENEVIEWER)
QTWORLD = qtworld.o $(WORLD) $(QTSCENEWINDOW)
WRAPPER = wrapper.o $(DIAGRAMWRAPPERSTATE)
CHARMAPPERWRAPPER = charmapperwrapper.o
SCENEWRAPPER = scenewrapper.o
WORLDWRAPPER = worldwrapper.o $(CHARMAPPERWRAPPER) $(SCENEWRAPPER)
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

evaluateexpression_test: evaluateexpression_test.o \
  $(EVALUATEEXPRESSION) $(STRINGUTIL) $(CHARMAPPEROBJECTS) $(ANYIO) $(ANY)
	$(CXX) -o $@ $^ $(LDFLAGS)

evaluatestatement_test: evaluatestatement_test.o \
  $(EVALUATESTATEMENT) $(SCENE) $(SCENEOBJECTS) $(FAKEEXECUTOR) $(ANY) $(ANYIO)
	$(CXX) -o $@ $^ $(LDFLAGS)

optional_test: optional_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

maybeint_test: maybeint_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

any_test: any_test.o $(ANY) $(ANYIO)
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramstate_test: diagramstate_test.o $(DIAGRAMSTATE)
	$(CXX) -o $@ $^ $(LDFLAGS)

fileaccessor_test: fileaccessor_test.o
	$(CXX) -o $@ $^ $(LDFLAGS)

observeddiagrams_test: observeddiagrams_test.o \
  $(OBSERVEDDIAGRAMS) $(OBSERVEDDIAGRAM)
	$(CXX) -o $@ $^ $(LDFLAGS)

charmapperobjects_test: charmapperobjects_test.o \
  $(CHARMAPPEROBJECTS) $(ANY) $(ANYIO)
	$(CXX) -o $@ $^ $(LDFLAGS)

evaluatediagram_test: evaluatediagram_test.o \
  $(EVALUATEDIAGRAM) $(DIAGRAM) $(DEFAULTDIAGRAMS) $(DIAGRAMEXECUTOR) $(ANY)
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramexecutor_test: diagramexecutor_test.o \
  $(DIAGRAMEXECUTOR) $(EVALUATEDIAGRAM)
	$(CXX) -o $@ $^ $(LDFLAGS)

diagrameditor_test: diagrameditor_test.o $(DIAGRAMEDITOR) $(OBSERVEDDIAGRAM) \
  $(FAKEDIAGRAMEDITOR)
	$(CXX) -o $@ $^ $(LDFLAGS)

nodetexteditor_test: nodetexteditor_test.o $(NODETEXTEDITOR) $(DIAGRAMNODE)
	$(CXX) -o $@ $^ $(LDFLAGS)

linetext_test: linetext_test.o $(LINETEXT)
	$(CXX) -o $@ $^ $(LDFLAGS)

statementtext_test: statementtext_test.o $(STATEMENTTEXT)
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramnode_test: diagramnode_test.o $(DIAGRAMNODE)
	$(CXX) -o $@ $^ $(LDFLAGS)

diagram_test: diagram_test.o $(DIAGRAM) $(EVALUATEDIAGRAM) $(FAKEEXECUTOR)
	$(CXX) -o $@ $^ $(LDFLAGS)

streamparser_test: streamparser_test.o $(STREAMPARSER)
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramio_test: diagramio_test.o $(DIAGRAMIO) $(MAKEDIAGRAM)
	$(CXX) -o $@ $^ $(LDFLAGS)

wrapper_test: wrapper_test.o $(WRAPPER) $(WRAPPERUTIL)
	$(CXX) -o $@ $^ $(LDFLAGS)

scene_test: scene_test.o $(SCENE)
	$(CXX) -o $@ $^ $(LDFLAGS)

world_test: world_test.o $(WORLD)
	$(CXX) -o $@ $^ $(LDFLAGS)

wrapperstate_test: wrapperstate_test.o $(WRAPPERSTATE) $(WRAPPER)
	$(CXX) -o $@ $^ $(LDFLAGS)

diagramwrapperstate_test: diagramwrapperstate_test.o $(DIAGRAMWRAPPERSTATE)
	$(CXX) -o $@ $^ $(LDFLAGS)

scenewrapper_test: scenewrapper_test.o \
  $(SCENEWRAPPER) $(WRAPPERSTATE) $(SCENE) $(WRAPPER) $(WRAPPERUTIL) \
  $(TREEUPDATING) $(FAKETREE)
	$(CXX) -o $@ $^ $(LDFLAGS)

worldwrapper_test: worldwrapper_test.o \
  $(WORLDWRAPPER) $(DIAGRAM) $(FAKETREE) $(OBSERVEDDIAGRAM) $(WRAPPERUTIL) \
  $(WRAPPERSTATE) $(WRAPPER) $(SCENE) $(WORLD) $(TREEUPDATING) \
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
  $(WRAPPER) $(WORLDWRAPPER) $(FAKEDIAGRAMEDITOR) \
  $(FAKEDIAGRAMEDITORWINDOWS) $(DIAGRAMEDITOR) $(TREEEDITOR)
	$(CXX) -o $@ $^ $(LDFLAGS)


qtscenewindow_manualtest: qtscenewindow_manualtest.o \
  $(QTSCENEWINDOW) $(SCENE) $(SCENEWINDOW)
	$(CXX) -o $@ $^ $(LDFLAGS)

qtslider_manualtest: qtslider_manualtest.o $(QTSLIDER)
	$(CXX) -o $@ $^ $(LDFLAGS)

qtdiagrameditorwindow_manualtest: qtdiagrameditorwindow_manualtest.o \
  $(QTDIAGRAMEDITORWINDOW) $(DIAGRAMSTATE) $(EVALUATEDIAGRAM) \
  $(OBSERVEDDIAGRAM) $(DIAGRAMEDITORWINDOW) $(DIAGRAMEXECUTOR)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.pass moc_*.cpp

-include *.d
