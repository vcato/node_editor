#include "qtlineedit.hpp"

#include <iostream>

using std::cerr;


QtLineEdit::QtLineEdit()
{
  connect(
    this,
    SIGNAL(textChanged(const QString &)),
    SLOT(textChangedSlot(const QString &))
  );
}


void QtLineEdit::textChangedSlot(const QString &new_text)
{
  text_changed_function(new_text.toStdString());
}
