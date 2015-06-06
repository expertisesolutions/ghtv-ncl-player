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

#include <ghtv/qt/java/java_widget.hpp>
#include <ghtv/qt/java/awt/component.hpp>
#include <ghtv/qt/java/awt/form.hpp>
#include <ghtv/qt/java/awt/graphics2d.hpp>
#include <ghtv/qt/java/awt/image.hpp>
#include <ghtv/qt/java/error_writer.hpp>
#include <ghtv/qt/java/remote_keys.hpp>
#include <ghtv/qt/java/awt/font.hpp>
#include <ghtv/qt/java/xlet_class_loader.hpp>
#include <ghtv/qt/java/file_input_stream.hpp>
#include <ghtv/qt/java/tv_container.hpp>

#include <jvb/jvb.hpp>
#include <jvb/jcl/java/io/PrintWriter.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/functional/factory.hpp>

#include <QKeyEvent>

namespace ghtv { namespace qt { namespace java {

java_widget::java_widget(QWidget* parent, QDir jre_path
                         , QString java_class, QString classpath, QDir java_root
                         , boost::optional<QDir> ghtv_jar)
  : QWidget(parent), jre_path(jre_path), java_class(java_class)
  , classpath(classpath), java_root(java_root), ghtv_jar(ghtv_jar)
  , error_object(0), jvm(0), focused_form(0), main_form(0)
{}

java_widget::~java_widget()
{
}

void java_widget::start()
{
  std::cout << "Width: " << width() << "x" << height() << std::endl;

  if(jre_path.exists())
  {
    QString classpath_options, bootclasspath_options;
    QDir guess_ghtv_jar = QDir::current();

    if(ghtv_jar && ghtv_jar->exists())
      guess_ghtv_jar = *ghtv_jar;

    // classpath_options = "-Djava.class.path=" + java_root.absolutePath();
    // if(!classpath.isEmpty())
    //   classpath_options += ":" + classpath;

    bootclasspath_options = "-Dsun.boot.class.path=";
    //bootclasspath_options += guess_ghtv_jar.absolutePath() + "/ghtv-classloaders.jar";
    // bootclasspath_options += guess_ghtv_jar.absolutePath() + "/ghtv-jar.jar";
    // bootclasspath_options += ":" + guess_ghtv_jar.absolutePath() + "/ghtv-io.jar";
    // bootclasspath_options += ":" + guess_ghtv_jar.absolutePath() + "/ghtv-dtv.jar";
    // bootclasspath_options += ":" + guess_ghtv_jar.absolutePath() + "/ghtv-tv.jar";
    // bootclasspath_options += ":" + guess_ghtv_jar.absolutePath() + "/ghtv-jmf.jar";
    // bootclasspath_options += ":" + guess_ghtv_jar.absolutePath() + "/ghtv-qt-java.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/resources.jar";
    bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/rt.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/sunrsasign.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/jsse.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/jce.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/charsets.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/netx.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/plugin.jar";
    // bootclasspath_options += ":" + jre_path.absolutePath() + "/lib/rhino.jar";
    //bootclasspath_options += ":" + jre_path.absolutePath() + "/classes";

    QByteArray classpath_array = classpath_options.toLocal8Bit();
    QByteArray bootclasspath_array = bootclasspath_options.toLocal8Bit();
    
    std::cout << "Using bootclasspath " << bootclasspath_options.toStdString() << std::endl;
    std::cout << "Using classpath " << classpath_options.toStdString() << std::endl;

    // JavaVMInitArgs vm_args;
    // JavaVMOption options[9];
    // options[0].optionString = classpath_array.data();
    // options[1].optionString = bootclasspath_array.data();
    // options[2].optionString = const_cast<char*>("");
    // options[2].optionString = const_cast<char*>("-verbose");
    // options[3].optionString = const_cast<char*>("-esa");
    // options[4].optionString = const_cast<char*>("-Xbatch");
    // options[5].optionString = const_cast<char*>("-Xrs");
    // options[6].optionString = const_cast<char*>("-Xcheck:jni");
    // options[7].optionString = const_cast<char*>("-Xfuture");
    // options[8].optionString = const_cast<char*>("-Xint");
    // vm_args.version = 0x00010002;
    // vm_args.options = options;
    // vm_args.nOptions = 9;
    // vm_args.ignoreUnrecognized = JNI_TRUE;
    // /* Create the Java VM */
    // JNIEnv* env_ = 0;
    // int res = JNI_CreateJavaVM(&jvm, (void**)&env_, &vm_args);
    // if(res < 0)
    // {
    //   std::cout << "Failed creating JVM" << std::endl;
    //   return;
    // }
    jvb::jvm jvm;
    jvb::environment e(jvm.environment());
    // main_form = new component_window(this);
    // main_form->resize(size());
    // main_form->setVisible(true);
    // assert(isVisible());
    
    try
    {
      std::cout << "1" << std::endl;

      JVB_CREATE_CLASS(ErrorWriter, java::error_writer, e
                       , (factory_constructors
                          (error_writer()
                           , boost::bind(boost::value_factory<error_writer>(), this))
                         )
                         (methods (write, &error_writer::write)
                          (close, &error_writer::close)
                          (flush, &error_writer::flush)
                         )
                      )

      std::cout << "2" << std::endl;

      ErrorWriter error_writer(e);
      jvb::jcl::java::io::PrintWriter print_writer_debug(e, error_writer);

      JVB_CREATE_CLASS(awt::Component, java::component_window, e
                       , (factory_constructors
                          (component_window(awt::Component)
                           , boost::bind(boost::factory<boost::shared_ptr<component_window> >(), _1)
                          )
                          (component_window(QWidget*)
                           , boost::bind(boost::factory<boost::shared_ptr<component_window> >(), _1)
                          )
                         )
                         (methods
                          (setX, &component_window::set_x)
                          (setY, &component_window::set_y)
                          (setWidth, &component_window::set_width)
                          (setHeight, &component_window::set_height)
                          (setPreferredSize, &component_window::set_preferred_size)
                         )
                      )

        std::cout << "Constructing component_window with this as parent " << this
                  << std::endl;

      awt::Component main_component(e, this);

    // main_form = new component_window(this);
    // main_form->resize(size());
    // main_form->setVisible(true);

      // {
      //   javabind::class_ class_ = env.find_class("ghtv/classloader/SecurityManager");
      //   javabind::object security_manager = class_.constructor<javabind::object()>()(class_);
      //   javabind::class_ system_class = env.find_class("java/lang/System");
      //   javabind::static_method<void(javabind::object)> set_security
      //     = system_class.static_method<void(javabind::object)>
      //     ("setSecurityManager", "java/lang/SecurityManager");
      //   set_security(system_class, security_manager);
      // }
      // javabind::object classloader;
      // {
      //   typedef javabind::object(constructor_type)(javabind::object);
      //   javabind::class_ class_ = env.find_class("ghtv/classloader/XletClassLoader");
      //   // javabind::register_class
      //   //   (
      //   //    javabind::reg::class_<java::xlet_class_loader>(class_)
      //   //    .def("nativeLoadClass", &java::xlet_class_loader::load_class
      //   //         , "java/lang/Class")
      //   //   );

      //   javabind::class_ url_class = env.find_class("java/net/URL");
      //   javabind::constructor<constructor_type> constructor = class_.constructor
      //     <constructor_type>("java/net/URL");
        
      //   javabind::string url_string
      //     (env_->NewStringUTF(("file://" + java_root.absolutePath()).toUtf8()), env_);
      //   javabind::object url = url_class.constructor<javabind::object(javabind::string)>
      //     ()(url_class, url_string);
      //   classloader = constructor(class_, url);
      // }
      // // {
      // //   javabind::method<javabind::object(javabind::string)>
      // //     load_class = classloader.class_().method<javabind::object(javabind::string)>
      // //     ("loadClass", "java/lang/Class");
      // //   javabind::string name(env_->NewStringUTF("java/io/InputStream"), env_);
      // //   load_class(classloader, name);
      // // }
      // // {
      // //   boost::filesystem::path path = guess_ghtv_jar.absolutePath().toStdString();
      // //   path /= "FileInputStream.class";
      // //   std::ifstream stm(path.string().c_str());
      // //   stm.seekg(0, std::ios::end);
      // //   std::size_t size = stm.tellg();
      // //   stm.seekg(0, std::ios::beg);
      // //   std::vector<char> buffer(size);
      // //   stm.rdbuf()->sgetn(&buffer[0], size);
      // //   if(jclass cls = env_->DefineClass("java/io/FileInputStream", classloader.raw()
      // //                                     , reinterpret_cast<jbyte*>(&buffer[0]), size))
      // //   {
      // //     std::cout << "Class was loaded" << std::endl;
      // //   }
      // //   else
      // //   {
      // //     std::cout << "Class was NOT loaded" << std::endl;
      // //     throw std::runtime_error("Class was not loaded");
      // //   }
      // // }
      // javabind::class_ component_class = env.find_class("java/awt/Component");
      // {
      //   javabind::register_class
      //   (
      //    javabind::reg::class_<awt::component>(component_class)
      //    .constructor(boost::bind(boost::value_factory<awt::component>(), _1, _2
      //                             , jvm))
      //    .def<javabind::int_(awt::component const&)>
      //    ("nativeGetWidth", boost::bind(&awt::component::width, _1))
      //    .def("nativeGetHeight", &awt::component::height)
      //    .def("nativeSetX", &awt::component::set_x)
      //    .def("nativeSetY", &awt::component::set_y)
      //    .def("nativeSetWidth", &awt::component::set_width)
      //    .def("nativeSetHeight", &awt::component::set_height)
      //    .def("nativeGetX", &awt::component::get_x)
      //    .def("nativeGetY", &awt::component::get_y)
      //    .def("nativeSetVisible", &awt::component::set_visible)
      //    .def("nativeIsVisible", &awt::component::is_visible)
      //    .def("nativeSetPreferredSize", &awt::component::set_preferred_size)
      //    .def("nativeSetDelay", &awt::component::set_delay)
      //    .def("nativeGetPreferredSize", &awt::component::get_preferred_size
      //         , "java/awt/Dimension")
      //    .def("nativeSetFocus", &awt::component::set_focus)
      //    .def("nativeRepaint", &awt::component::repaint)
      //   );
      // }
      // javabind::class_ container_class = env.find_class("java/awt/Container");
      // {
      //   javabind::register_class
      //   (
      //    javabind::reg::class_<awt::container>(container_class)
      //    .base(component_class)
      //    .def("nativeAddComponent", &awt::container::add_component
      //         , /*javabind::descriptors(*/"java/awt/Component"/*, "java/awt/Component")*/)
      //    // .def("nativeRemoveComponent", &awt::container::remove_component
      //    //      , "com/sun/dtv/lwuit/Component")
      //    .def("nativeSetCoordinateLayout", &awt::container::set_coordinate_layout)
      //   );
      // }
      // {
      //   javabind::class_ form_class = env.find_class("ghtv/awt/QForm");
      //   javabind::register_class
      //   (
      //    javabind::reg::class_<awt::form>(form_class)
      //    .constructor(boost::bind(&java_widget::create_form, this, _1, _2, _3))
      //    .base(container_class)
      //    .def("nativeShow", &awt::form::show)
      //    .def("nativeRegisterAnimation", &awt::form::register_animation
      //         , "com/sun/dtv/lwuit/animations/Animation")
      //    .def("nativeDeregisterAnimation", &awt::form::deregister_animation
      //         , "com/sun/dtv/lwuit/animations/Animation")
      //   );
      // }
      // javabind::class_ tv_container_class = env.find_class("javax/tv/graphics/TVContainer");
      // {
      //   javabind::register_class
      //   (
      //    javabind::reg::class_<java::tv_container>(tv_container_class)
      //    // .constructor(boost::bind(&java_widget::create_form, this, _1, _2, _3))
      //    // .base(container_class)
      //    // .def("nativeShow", &java::form::show)
      //    // .def("nativeRegisterAnimation", &java::form::register_animation
      //    //      , "com/sun/dtv/lwuit/animations/Animation")
      //   );
      // }
      // {
      //   javabind::class_ graphics2d_impl = env.find_class("ghtv/awt/QGraphics2D");
      //   javabind::register_class
      //   (
      //    javabind::reg::class_<awt::graphics2d>(graphics2d_impl)
      //    // .def("nativeDispose", &awt::graphics2d::dispose)
      //    .def("nativeSetClip", &awt::graphics2d::set_clip)
      //    .def("nativeGetClip", &awt::graphics2d::get_clip
      //         , "java/awt/Shape")
      //    .def("nativeDrawImage", &awt::graphics2d::draw_image
      //         , javabind::descriptors("java/awt/Image", "java/awt/image/ImageObserver"))
      //    .def("nativeDrawString", &awt::graphics2d::draw_string)
      //   );
      // }
      // {
      //   javabind::class_ image = env.find_class("ghtv/awt/QImage");
      //   javabind::register_class
      //   (
      //    javabind::reg::class_<awt::image>(image)
      //    .constructor(boost::bind(boost::value_factory<awt::image>(), _1, _2
      //                             , java_root.absolutePath()))
      //    .def("nativeConstructor", &awt::image::constructor)
      //    .def("nativeGetWidth", &awt::image::width)
      //    .def("nativeGetHeight", &awt::image::height)
      //    .def("nativeSetScaling", &awt::image::set_scaling)
      //    .def("nativeSetScalingWidth", &awt::image::set_scaling_width)
      //    .def("nativeSetScalingHeight", &awt::image::set_scaling_height)
      //    .def("nativePath", &awt::image::path)
      //   );
      // }
      // {
      //   javabind::class_ font = env.find_class("java/awt/Font");
      //   javabind::register_class
      //     (
      //      javabind::reg::class_<awt::font>(font)
      //      .constructor(boost::bind(boost::value_factory<awt::font>()))
      //      .def("nativeConstructor", &awt::font::constructor)
      //     );
      // }
      // {
      //   javabind::object container = tv_container_class
      //     .constructor<javabind::object()>()(tv_container_class);
      //   javabind::static_field<javabind::object> container_field
      //     = tv_container_class.find_static_field<javabind::object>
      //     ("container", "Ljavax/tv/graphics/TVContainer;");
      //   container_field.set(tv_container_class, container);
      // }
      // {
      //   javabind::class_ cls = env.find_class("ghtv/io/FileInputStream");
      //   javabind::register_class
      //     (
      //      javabind::reg::class_<java::file_input_stream>(cls)
      //      .constructor(boost::bind(boost::value_factory<java::file_input_stream>()
      //                               , _1, _2, java_root))
      //      .def("nativeConstructor", &java::file_input_stream::constructor)
      //      .def("nativeRead", &java::file_input_stream::read_array)
      //      .def("nativeAvailable", &java::file_input_stream::available)
      //      .def("nativeClose", &java::file_input_stream::close)
      //     );
      // }

      // javabind::object user_xlet_class_;
      // {
      //   std::cout << "load class 1" << std::endl;
      //   javabind::method<javabind::object(javabind::string)>
      //     load_class = classloader.class_().method<javabind::object(javabind::string)>
      //     ("loadClass", "java/lang/Class");
      //   std::cout << "load class 2 " << java_class.toStdString() << std::endl;
      //   javabind::string name(env_->NewStringUTF(java_class.toUtf8()), env_);
      //   std::cout << "load class 3" << std::endl;
      //   user_xlet_class_ = load_class(classloader, name);
      //   assert(!!user_xlet_class_);
      //   std::cout << "load class 4" << std::endl;
      // }

      // javabind::class_ user_xlet_class(static_cast<jclass>(user_xlet_class_.raw()), env_);
      // javabind::object user_xlet = user_xlet_class.constructor<javabind::object()>()(user_xlet_class);

      // javabind::class_ jtv_xlet_class = env.find_class("javax/tv/xlet/Xlet");
      // javabind::class_ me_xlet_class = env.find_class("javax/microedition/xlet/Xlet");

      // javabind::class_ xlet_context_class = env.find_class("ghtv/xlet/XletContextImpl");
      // javabind::object xlet_context = xlet_context_class.constructor<javabind::object()>()(xlet_context_class);

      // if(env_->IsAssignableFrom(user_xlet_class.raw(), jtv_xlet_class.raw()))
      // {
      //   std::cout << "searching startXlet" << std::endl;
      //   javabind::method<void()> startxlet = jtv_xlet_class.method<void()>("startXlet");
      //   std::cout << "searching initXlet" << std::endl;
      //   javabind::method<void(javabind::object)> initxlet = jtv_xlet_class.method
      //     <void(javabind::object)>("initXlet"
      //                              , javabind::descriptors("javax/tv/xlet/XletContext"));
      //   std::cout << "Run initXlet" << std::endl;
      //   initxlet(user_xlet, xlet_context);
      //   std::cout << "Run startXlet" << std::endl;
      //   startxlet(user_xlet);
      // }
      // else if(env_->IsAssignableFrom(user_xlet_class.raw(), me_xlet_class.raw()))
      // {
      //   std::cout << "Is a ME app" << std::endl;
      //   std::cout << "searching startXlet" << std::endl;
      //   javabind::method<void()> startxlet = me_xlet_class.method<void()>("startXlet");
      //   std::cout << "searching initXlet" << std::endl;
      //   javabind::method<void(javabind::object)> initxlet = me_xlet_class.method
      //     <void(javabind::object)>("initXlet"
      //                              , javabind::descriptors("javax/microedition/xlet/XletContext"));
      //   std::cout << "Run initXlet" << std::endl;
      //   initxlet(user_xlet, xlet_context);
      //   std::cout << "Run startXlet" << std::endl;
      //   startxlet(user_xlet);
      // }
      // else
      // {
      //   std::cout << "No xlet?" << std::endl;
      // }

    }
    catch(std::exception& ex)
    {
      std::cout << "A exception was thrown " << typeid(e).name()
                << " what: " << ex.what() << std::endl;
      if(jthrowable exception = e.raw()->ExceptionOccurred())
        exception_occurred(e.raw(), exception);
    }
  }
}

void java_widget::reading_exception(std::string buffer)
{
  std::cout << "reading_exception: " << buffer << std::endl;
  error_buffer += buffer;
}

void java_widget::exception_occurred(JNIEnv* env, jthrowable exception)
{
  env->ExceptionClear();

  try
  {
    // std::cout << "1" << std::endl;
    // javabind::object e(exception, env);
    // javabind::class_ throwable_class = javabind::env(env).find_class("java/lang/Throwable");
    // std::cout << "1" << std::endl;
    // javabind::method<void(javabind::object)> printStack
    //   = throwable_class.method<void(javabind::object)>("printStackTrace", "java/io/PrintWriter");

    // javabind::method<void()> printStack_output
    //   = throwable_class.method<void()>("printStackTrace");

    // error_buffer.clear();

    // printStack_output(e);
    // javabind::object error_obj(error_object, env);
    // printStack(e, error_obj);

    // javabind::method<void()> flush = error_obj.class_()
    //   .method<void()>("flush");
    // flush(error_obj);

    // error_occurred(error_buffer);

  }
  catch(std::exception const& e)
  {
    std::cout << "Error calling printStack. Exception was thrown" << std::endl;
    env->ExceptionDescribe();
    env->ExceptionClear();
  }
}

bool java_widget::process_key(QKeyEvent* event, bool pressed)
{
  std::cout << "A key was pressed" << std::endl;

  if(pressed)
  {
    // JNIEnv* env = 0;
    // jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), 0);
    // if(env)
    // {
    //   try
    //   {

    //     if(this->focused_form)
    //     {
    //       javabind::object focused_form(this->focused_form, env);
    //       javabind::method<void(javabind::int_)> keyPressed
    //         = focused_form.class_().method<void(javabind::int_)>("keyPressed");

    //       std::cout << "Should run keyPressed" << std::endl;
    //       switch(event->key())
    //       {
    //       case Qt::Key_I:
    //         keyPressed(focused_form, jint(remote_keys::info));
    //         return true;
    //       case Qt::Key_Return:
    //         keyPressed(focused_form, jint(remote_keys::confirm));
    //         return true;
    //       case Qt::Key_Up:
    //         keyPressed(focused_form, jint(remote_keys::bass_up));
    //         return true;
    //       case Qt::Key_Down:
    //         keyPressed(focused_form, jint(remote_keys::bass_down));
    //         return true;
    //       case Qt::Key_Escape:
    //         keyPressed(focused_form, jint(remote_keys::back));
    //         return true;
    //       default:
    //         std::cout << "NOT Calling keyPressed" << std::endl;
    //         return false;
    //       }
    //     }
    //   }
    //   catch(std::exception&)
    //   {
    //     std::cout << "A exception was thrown" << std::endl;
    //     if(jthrowable exception = env->ExceptionOccurred())
    //       exception_occurred(env, exception);
    //   }
    // }
  }

  return false;
}

// awt::form java_widget::create_form(JNIEnv* env, form_object_type form_object
//                               , form_extends_type::info einfo)
// {
//   std::cout << "Creating Form" << std::endl;
//   if(!this->focused_form)
//     this->focused_form = env->NewWeakGlobalRef(form_object.self(env).raw());
//   return awt::form(env, main_form, form_object, einfo, jvm);
// }

} } }

