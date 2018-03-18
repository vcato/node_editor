#include "qtscenetree.hpp"

#include <cassert>
#include <QHeaderView>
#include "qttreewidgetitem.hpp"
#include "treepath.hpp"


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
  QTreeWidgetItem &parent_item =
    itemFromPath(*invisibleRootItem(),parentPath(path));
  insertBodyIn(parent_item,/*index*/path.back(),item);
}


void QtSceneTree::removeItem(const std::vector<int> &path)
{
  QTreeWidgetItem &parent_item =
    itemFromPath(*invisibleRootItem(),parentPath(path));
  removeBodyFrom(parent_item,/*index*/path.back());
}
