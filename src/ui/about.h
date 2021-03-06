/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

#include "ui_about.h"

class About : public QDialog {
  Q_OBJECT
 public:
  About(QWidget* parent = 0);

  static const char* kUrl;

  struct Person {
    Person(const QString& n, const QString& e = QString()) : name(n), email(e) {}

    bool operator <(const Person& other) const { return name < other.name; }

    QString name;
    QString email;
  };

 private:
  QString MakeHtml() const;
  QString MakeHtml(const Person& person) const;

 private:
  Ui::About ui_;

  QList<Person> authors_;
  QList<Person> thanks_to_;
};

#endif // ABOUT_H
