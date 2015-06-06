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

#ifndef GHTV_QT_MAIN_WINDOW_HPP
#define GHTV_QT_MAIN_WINDOW_HPP

#include <QtGui/QMainWindow>
#include <QtGui/QLabel>

#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/debug/debug_media_window.hpp>
#include <ghtv/qt/ncl/debug/live_media_info.hpp>
#include <ghtv/qt/ncl/debug/errors.hpp>
#include <ghtv/qt/resolution_action.hpp>
#include <ghtv/qt/ncl/testsuite/testsuite_window.hpp>
#ifndef GHTV_DISABLE_JAVA
#include <ghtv/qt/java/java_widget.hpp>
#endif
#ifdef __linux
#include <ghtv/qt/reception.hpp>
#include <ghtv/qt/dsmcc_download.hpp>
#endif

#include <boost/optional.hpp>

#include <string>

namespace ghtv { namespace qt {

class main_window : public QMainWindow
{
  Q_OBJECT

public:
  main_window(unsigned int width, unsigned int height, QWidget *parent = 0);
  ~main_window();
#ifndef GHTV_DISABLE_JAVA
  void open_early_ncl(std::string ncl_filename);
  void open_early_java(std::string java_class, std::string classpath
                       , std::string java_root);
#endif

  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void paintEvent(QPaintEvent*);
  
  unsigned int width_, height_;
  std::string jre_path;
  boost::optional<std::string> ghtv_jar;
  QMenu* file_menu, *debug_menu, *display_menu;
  QString filename;
  QString classpath, java_class, java_root;
  ghtv::qt::ncl::ncl_widget* ncl;
#ifndef GHTV_DISABLE_JAVA
  ghtv::qt::java::java_widget* java;
#endif
  ghtv::qt::ncl::debug::debug_media_window* debug_media_window_;
  ghtv::qt::ncl::debug::live_media_info* live_media_info_;
  ghtv::qt::ncl::debug::errors* errors_window;
  ghtv::qt::dsmcc_download* dsmcc_download_window;
  std::list<std::string> error_messages;
  bool initialized;
  QLabel* error_notification;
  std::vector<ghtv::qt::resolution_action*> resolutions;
  ghtv::qt::ncl::testsuite::testsuite_window* testsuite_window;
  ghtv::qt::reception* reception_window;

  void disable_opens();
Q_SIGNALS:
  void initialized_signal();
public Q_SLOTS:
  void change_resolution(std::size_t, std::size_t, ghtv::qt::resolution_action*);
  void destroying_debug_media_window();
  void destroying_errors_window();
  void destroying_reception_window();
  void destroying_dsmcc_download_window();
  void media_clicked(std::string media_identifier, std::string context_identifier);
  void error_occurred(std::string error);
  void open_queued_ncl();
  void open_dsmcc_app();
  void menu_open_ncl();
#ifndef GHTV_DISABLE_JAVA
  void open_queued_java();
  void menu_open_java();
#endif
  void open_channel(std::string name, unsigned int frequency, unsigned int service_identifier);
  void open_reception();
  void close_file();
  void debug_medias();
  void debug_errors();
  void start_testsuite_mode();
  void stop_testsuite_mode();
  void run_test(QString);
  void stop_test();
};

} }

#endif
