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

#include <QtGui/QApplication>
#include <ghtv/qt/main_window.hpp>

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#if defined(_WIN32) && !defined(GHTV_FORCE_CONSOLE)
#include <windows.h>

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
#if defined(_WIN32) && !defined(GHTV_FORCE_CONSOLE)
  int argc = __argc;
  char** argv = __argv;
#endif
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("GHTV NCL Player");
    a.addLibraryPath(a.applicationDirPath());

    boost::optional<std::string> ncl_filename;
    boost::optional<std::string> classpath;
    boost::optional<std::string> java_class;
    boost::optional<std::string> java_root;
    boost::optional<std::string> ghtv_jar;
    std::string jre_path;
    unsigned int width = 0, height = 0;
    {
      boost::program_options::options_description description
        ("Allowed options");
      description.add_options()
        ("help", "produce this help message")
        ("ncl", boost::program_options::value<std::string>(), "NCL file to play")
#ifndef GHTV_DISABLE_JAVA
        ("classpath", boost::program_options::value<std::string>(), "Classpath for java")
        ("dir", boost::program_options::value<std::string>(), "Root directory for GINGA-J application")
        ("jre", boost::program_options::value<std::string>(), "Path to JRE")
        ("ghtv-jar", boost::program_options::value<std::string>(), "Path to GHTV JAR files")
        ("class", boost::program_options::value<std::string>(), "Class for Xlet")
#endif
        ("width", boost::program_options::value<unsigned int>(), "Width of the window")
        ("height", boost::program_options::value<unsigned int>(), "Height of the window")
        ;

      boost::program_options::variables_map vm;
      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);
      boost::program_options::notify(vm);

      if(vm.count("help"))
      {
        std::cout << description << std::endl;
        return 0;
      }

      if(vm.count("ncl"))
        ncl_filename = vm["ncl"].as<std::string>();

      if(vm.count("classpath"))
        classpath = vm["classpath"].as<std::string>();

      if(vm.count("class"))
        java_class = vm["class"].as<std::string>();

      if(vm.count("dir"))
        java_root = vm["dir"].as<std::string>();

      if(vm.count("jre"))
        jre_path = vm["jre"].as<std::string>();

      if(vm.count("ghtv-jar"))
        ghtv_jar = vm["ghtv-jar"].as<std::string>();

      if(vm.count("width"))
        width = vm["width"].as<unsigned int>();

      if(vm.count("height"))
        height = vm["height"].as<unsigned int>();
    }

    if(jre_path.empty())
    {
      char* jre_path_env = getenv("J2REDIR");
      if(jre_path_env)
      {
        jre_path = jre_path_env;
      }
    }

    ghtv::qt::main_window w(width, height);
    w.jre_path = jre_path;
    w.ghtv_jar = ghtv_jar;
    w.showMaximized();
    a.processEvents();
#ifndef GHTV_DISABLE_JAVA
    if(ncl_filename && *ncl_filename != "\"\"" && !ncl_filename->empty())
      w.open_early_ncl(*ncl_filename);
    else if(java_class && *java_class != "\"\"" && !java_class->empty()
            && java_root && *java_root != "\"\"" && !java_root->empty())
    {
      std::string classpath_deref;
      if(classpath) classpath_deref = *classpath;
      w.open_early_java(*java_class, classpath_deref, *java_root);
    }
#endif

    return a.exec();
}
