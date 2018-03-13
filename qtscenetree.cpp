#include "qtscenetree.hpp"

#include <cassert>
#include <QHeaderView>
#include "qttreewidgetitem.hpp"


using std::vector;
using std::string;
using std::cerr;



static void setText(QTreeWidgetItem &item1,const string &text)
{
  item1.setText(/*column*/0,QString::fromStdString(text));
}


QtSceneTree::QtSceneTree()
{
  assert(header());
  header()->close();
}


void QtSceneTree::setItems(const ItemData &root)
{
  clear();
  assert(invisibleRootItem());
  addBodiesTo(*invisibleRootItem(),root.children);
}


void QtSceneTree::insertItem(const vector<int> &path,const ItemData &item)
{
  vector<int> parent_path = path;
  parent_path.pop_back();
  QTreeWidgetItem &parent_item =
    itemFromPath(*invisibleRootItem(),parent_path);
  insertBodyIn(parent_item,/*index*/path.back(),item);
}
