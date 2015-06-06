/* (c) Copyright 2011-2014 Felipe Magno de Almeida
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GHTV_QT_JAVA_JAVA_WIDGET_HPP
#define GHTV_QT_JAVA_JAVA_WIDGET_HPP

#include <ghtv/qt/java/awt/form.hpp>
#include <ghtv/qt/java/component_window.hpp>

#include <boost/functional/value_factory.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <QWidget>
#include <QThread>
#include <QDir>
#include <QString>
#include <jni.h>

#include <iostream>
#include <fstream>
#include <cassert>

namespace ghtv { namespace qt { namespace java {

class java_widget : public QWidget
{
  Q_OBJECT
public:
  ~java_widget();
  java_widget(QWidget* parent, QDir jre_path, QString java_class, QString classpath
              , QDir java_root, boost::optional<QDir> ghtv_jar);

  void start();
  bool process_key(QKeyEvent* event, bool pressed);

  void reading_exception(std::string buffer);
  void exception_occurred(JNIEnv* env, jthrowable exception);
  void error_occurred(std::string const& error_message)
  {
    Q_EMIT error_occurred_signal(error_message);
  }

  // typedef jvb::reg::extends<awt::component> form_extends_type;
  // typedef jvb::reg::object form_object_type;

  awt::form create_form(JNIEnv* env);

  std::string error_buffer;

  QDir jre_path;
  QString java_class, classpath;
  QDir java_root;
  boost::optional<QDir> ghtv_jar;
  jobject error_object;
  JavaVM* jvm;
  jobject focused_form;
  component_window* main_form;

Q_SIGNALS:
  void error_occurred_signal(std::string);
};

} } }

#endif
