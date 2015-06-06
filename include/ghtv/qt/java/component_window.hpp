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

#ifndef GHTV_QT_JAVA_COMPONENT_WINDOW_HPP
#define GHTV_QT_JAVA_COMPONENT_WINDOW_HPP

#include <ghtv/qt/java/awt/graphics2d.hpp>
#include <ghtv/qt/java/awt/image.hpp>
#include <ghtv/qt/java/dimension.hpp>

#include <jvb/adapt_class.hpp>
#include <jvb/jcl/java/io/PrintStream.hpp>
#include <jvb/jcl/java/io/PrintWriter.hpp>

#include <boost/optional.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <cassert>

#include <jni.h>

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPainter>

namespace ghtv { namespace qt { namespace java {

class component_window;

namespace io = jvb::jcl::java::io;

#define GHTV_LISTENERS_METHODS(NAME)                                    \
      (BOOST_PP_CAT(add, NAME), void(NAME))                             \
      (BOOST_PP_CAT(remove, NAME), void(NAME))                          \
      (BOOST_PP_CAT(BOOST_PP_CAT(get, NAME), s), jvb::array<NAME>())

namespace awt {

namespace jvb_class_definition_namespace {
struct Container;
}

using jvb_class_definition_namespace::Container;

JVB_ADAPT_CLASS((java)(awt)(Component)
                , (public)
                , (methods
                   (getX, jvb::int_(), nil)
                   (getY, jvb::int_(), nil)
                   (isVisible, bool(), nil)
                   (setVisible, void(bool), nil)
                   (getWidth, jvb::int_(), nil)
                   (getHeight, jvb::int_(), nil)
                   (setX, void(jvb::int_), nil)
                   (setY, void(jvb::int_), nil)
                   (setPreferredSize, void(Dimension), nil)
                   (getPreferredSize, Dimension(), nil)
                   (setWidth, void(jvb::int_), nil)
                   (setHeight, void(jvb::int_), nil)
                   (setParent, void(Container), nil)
                   (getParent, Container(), nil)
                   (setFocus, void(jvb::long_, bool), nil)
                   (setDelay, void(jvb::long_, jvb::int_), nil)
                   (getName, jvb::String(), nil)
                   (setName, void(jvb::String), nil)
                   // (getGraphicsConfiguration, GraphicsConfiguration(), nil)
                   (getTreeLock, jvb::Object(), nil)
                   // (getToolkit, Toolkit(), nil)
                   (isValid, bool(), nil)
                   (isDisplayable, bool(), nil)
                   (isShowing, bool(), nil)
                   (isEnabled, bool(), nil)
                   (setEnabled, void(bool), nil)
                   (isDoubleBuffered, bool(), nil)
                   (enableInputMethods, void(bool), nil)
                   // (getForeground, Color(), nil)
                   // (setForeground, void(Color), nil)
                   (isForegroundSet, bool(), nil)
                   // (getBackground, Color(), nil)
                   // (setBackground, void(Color), nil)
                   (isBackgroundSet, bool(), nil)
                   // (getFont, Font(), nil)
                   // (setFont, void(Font), nil)
                   (isFontSet, bool(), nil)
                   // (getLocale, Locale(), nil)
                   // (setLocale, void(Locale), nil)
                   // (getColorModel, ColorModel(), nil)
                   // (getLocation, Point(), nil)
                   // (getLocationOnScreen, Point(), nil)
                   (setLocation, void(jvb::int_, jvb::int_), nil)
                   (setSize, void(jvb::int_, jvb::int_), nil)
                   // (getBounds, Rectangle(), nil)
                   (setBounds, void(jvb::int_, jvb::int_, jvb::int_, jvb::int_), nil)
                   // (setBounds, void(Rectangle), nil)
                   // (getBounds, Rectangle(Rectangle), nil)
                   // (getLocation, Point(Point), nil)
                   (isOpaque, bool(), nil)
                   (isLightweight, bool(), nil)
                   (getMinimumSize, Dimension(), nil)
                   (getMaximumSize, Dimension(), nil)
                   (getAlignmentX, jvb::float_(), nil)
                   (getAlignmentY, jvb::float_(), nil)
                   (doLayout, void(), nil)
                   (validate, void(), nil)
                   (invalidate, void(), nil)
                   // (getGraphics, Graphics(), nil)
                   // (getFontMetrics, FontMetrics(Font), nil)
                   // (setCursor, void(Cursor), nil)
                   // (getCursor, Cursor(), nil)
                   (isCursorSet, bool(), nil)
                   // (paint, void(Graphics), nil)
                   // (update, void(Graphics), nil)
                   // (paintAll, void(Graphics), nil)
                   // (print, void(Graphics), nil)
                   // (printAll, void(Graphics), nil)
                   // (imageUpdate, bool(Image, jvb::int_, jvb::int_, jvb::int_, jvb::int_, jvb::int_), nil)
                   (setIgnoreRepaint, void(), nil)
                   // (dispatchEvent, void(AWTEvent), nil)
                   // GHTV_LISTENERS_METHODS(ComponentListener)
                   // GHTV_LISTENERS_METHODS(FocusListener)
                   // GHTV_LISTENERS_METHODS(KeyListener)
                   // GHTV_LISTENERS_METHODS(MouseListener)
                   // GHTV_LISTENERS_METHODS(MouseMotionListener)
                   // GHTV_LISTENERS_METHODS(MouseWheelListener)
                   // GHTV_LISTENERS_METHODS(InputMethodListener)
                   // GHTV_LISTENERS_METHODS(PropertyChangeListener)
                   //
                   // (getInputContext, InputContext(), nil)
                   (enableEvents, void(jvb::long_), nil)
                   (disableEvents, void(jvb::long_), nil)
                   // // (coalesceEvents, AWTEvent(AWTEvent, AWTEvent), nil)
                   // (processEvent, void(AWTEvent), nil)
                   // // (processComponentEvent, void(ComponentEvent), nil)
                   // // (processFocusEvent, void(FocusEvent), nil)
                   // // (processKeyEvent, void(KeyEvent), nil)
                   // // (processMouseEvent, void(MouseEvent), nil)
                   // // (processMouseMotionEvent, void(MouseEvent), nil)
                   // // (processMouseWheelEvent, void(MouseEvent), nil)
                   // // (processInputMethodEvent, void(InputMethodEvent), nil)
                   (addNotify, void(), nil)
                   (removeNotify, void(), nil)
                   (isFocusTraversable, bool(), nil)
                   (isFocusable, bool(), nil)
                   (setFocusable, void(bool), nil)
                   // // (setFocusTraversalKeys, void(jvb::int_, Set), nil)
                   // (getFocusTraversalKeys, Set(jvb::int_), nil)
                   (areFocusTraversalKeysSet, bool(jvb::int_), nil)
                   (setFocusTraversalKeysEnabled, void(bool), nil)
                   (getFocusTraversalKeysEnabled, bool(), nil)
                   (transferFocus, void(), nil)
                   (getFocusCycleRootAncestor, Container(), nil)
                   (isFocusCycleRoot, bool(Container), nil)
                   (transferFocusBackward, void(), nil)
                   (transferFocusUpCycle, void(), nil)
                   (hasFocus, bool(), nil)
                   (isFocusOwner, bool(), nil)
                   (paramString, jvb::String(), nil)
                   (firePropertyChange, void(jvb::String, jvb::Object, jvb::Object), nil)
                  ) // methods
                  (overloads 
                   (getSize,
                    (Dimension())
                    (Dimension(Dimension))
                   ) // getSize
                   (repaint,
                    (void())
                    (void(jvb::long_))
                    (void(jvb::int_, jvb::int_, jvb::int_, jvb::int_))
                    (void(jvb::long_, jvb::int_, jvb::int_, jvb::int_, jvb::int_))
                   ) // repaint
                   // (createImage,
                   //  (Image(ImageProducer))
                   //  (Image(jvb::int_, jvb::int_))
                   // ) // createImage
                   // (createVolatileImage,
                   //  (Image(jvb::int_, jvb::int_))
                   //  (Image(jvb::int_, jvb::int_, ImageCapabilities))
                   // ) // createVolatileImage
                   // (prepareImage,
                   //  (bool(Image, ImageObserver))
                   //  (bool(Image, jvb::int_, jvb::int_, ImageObserver))
                   // ) // prepareImage
                   // (checkImage,
                   //  (jvb::int_(Image, ImageObserver))
                   //  (jvb::int_(Image, jvb::int_, jvb::int_, ImageObserver))
                   // ) // prepareImage
                   // (contains,
                   //  (bool(jvb::int_, jvb::int_))
                   //  (bool(Point))
                   // ) // contains
                   // (getComponentAt,
                   //  (Component(jvb::int_, jvb::int_))
                   //  (Component(Point))
                   // ) // getComponentAt
                   (requestFocus,
                    (void())
                    (bool(bool))
                   ) // requestFocus
                   (requestFocusInWindow,
                    (bool())
                    (bool(bool))
                   ) // requestFocusInWindow
                   // (list,
                   //  (void())
                   //  (void(io::PrintStream))
                   //  (void(io::PrintStream, jvb::int_))
                   //  (void(io::PrintWriter))
                   //  (void(io::PrintWriter, jvb::int_))
                   // ) // list
                  ) // overloads
                  //(implements (ImageObserver)(Serializable))
                  (constructors
                   (Component(Component))
                   (Component(QWidget*))
                  )
                )

JVB_ADAPT_CLASS((java)(awt)(Container)
                , (public)
                , (methods
                   (getComponentCount, jvb::int_(), nil)
                   (getComponent, Component(jvb::int_), nil)
                   (getComponents, jvb::array<Component>(), nil)
                   // (getInsets, Insets(), nil)
                   (addComponent, void(Component), nil)
                   (removeAll, void(), nil)
                   // (getLayout, LayoutManager(), nil)
                   // (setLayout, lwuit::Layout(), nil)
                   (doLayout, void(), nil)
                   (invalidate, void(), nil)
                   (validate, void(), nil)
                   (validateTree, void(), nil)
                   // (setFont, void(Font), nil)
                   (getMinimumSize, Dimension(), nil)
                   (getMaximumSize, Dimension(), nil)
                   (getAlignmentX, jvb::float_(), nil)
                   (getAlignmentY, jvb::float_(), nil)
                   // (paint, void(Graphics), nil)
                   // (update, void(Graphics), nil)
                   // (print, void(Graphics), nil)
                   // (paintComponents, void(Graphics), nil)
                   // (printComponents, void(Graphics), nil)
                   // GHTV_LISTENERS_METHODS(ContainerListener)
                   // (processEvents, void(AWTEvent), nil)
                   // (processContainerEvent, void(ContainerEvent), nil)
                   (isAncestorOf, bool(Component), nil)
                   (paramString, jvb::String(), nil)
                   (list, void(io::PrintStream, jvb::int_), nil)
                   // (setFocusTraversalKeys, void(jvb::int_, Set), nil)
                   // (getFocusTraversalKeys, Set(jvb::int_), nil)
                   (areFocusTraversalKeysSet, bool(jvb::int_), nil)
                   (transferFocusBackward, void(), nil)
                   // (setFocusTraversalPolicy, void(FocusTraversalPolicy), nil)
                   // (getFocusTraversalPolicy, FocusTraversalPolicy(), nil)
                   (isFocusTraversalPolicySet, bool(), nil)
                   (setFocusCycleRoot, void(), nil)
                   (transferFocusDownCycle, void(), nil)
                  ) // methods
                  (overloads
                   (add,
                    (Component(Component))
                    (Component(jvb::String, Component))
                    (Component(Component, jvb::int_))
                    (void(Component, jvb::Object))
                    (void(Component, jvb::Object, jvb::int_))
                   ) // add
                   (remove,
                    (void(jvb::int_))
                    (void(Component))
                   ) // remove
                  //  (getComponentAt,
                  //   (Component(jvb::int_, jvb::int_))
                  //   (Component(Point))
                  //  ) // getComponentAt
                  //  (findComponentAt,
                  //   (Component(jvb::int_, jvb::int_))
                  //   (Component(Point))
                  //  ) // findComponentAt
                   (isFocusCycleRoot,
                    (bool(Container))
                    (bool())
                   ) // isFocusCycleRoot
                  ) // overloads
                  (extends Component)
                )


}

namespace awt {

struct component;

}

class component_window : public QWidget
{
  Q_OBJECT
public:
  component_window(awt::Component parent);
  component_window(QWidget* parent);

  void set_awt_component(awt::component*);

  void resizeEvent(QResizeEvent* event);
  void paintEvent(QPaintEvent* event);
  void register_animation(JNIEnv* env, jvb::object animation);
  void deregister_animation(JNIEnv* env, jvb::object animation);
  void draw_string(std::string const& str, int x, int y, QRect clip);
  void set_preferred_size(awt::Dimension d);
  // void set_coordinate_layout(jvb::int_ x, jvb::int_ y/*, int width, int height*/);
  // void add_window(component_window* window);
  void set_x(jvb::int_ x);
  void set_y(jvb::int_ y);
  void set_width(jvb::int_ w);
  void set_height(jvb::int_ h);
  void set_animation_timer(boost::function<void()>);
  void reset_animation_timer();

  awt::component* component;
  QTimer* animation_timer;
  boost::function<void()> animation_handler;
  QLabel* border_label;
  int translate_x, translate_y;
  bool absolute_coordinates;
public Q_SLOTS:
  void animation_timeout();
};

} } }

#endif
