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

#ifndef GHTV_QT_DEBUG_MEDIA_WINDOW_HPP
#define GHTV_QT_DEBUG_MEDIA_WINDOW_HPP

#include <ghtv/qt/ncl/debug/medias_table_widget.hpp>
#include <ghtv/qt/ncl/debug/presentations_table_widget.hpp>
#include <ghtv/qt/ncl/ncl_widget.hpp>
#include <ghtv/qt/ncl/initialized_data.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace ghtv { namespace qt { namespace ncl { namespace debug {

class debug_media_window : public QWidget
{
  Q_OBJECT
public:
  debug_media_window(ncl_widget* ncl, QWidget* parent)
    : ncl(ncl), medias_table_widget_(0)
    , presentations_table_widget_(0), buttons_widget(0)
  {
    setAttribute(Qt::WA_DeleteOnClose);
    ncl->pause_events();

    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* warning = new QLabel("While this window is opened, the NCL won't process any new"
                                 " events or keys (medias will continue to play and generate events,"
                                 " but none will be processed)");
    layout->addWidget(warning);

    medias_table_widget_ = new medias_table_widget(ncl);
    layout->addWidget(medias_table_widget_);

    buttons_widget = create_medias_table_buttons();
    layout->addWidget(buttons_widget);
    setLayout(layout);
  }

  QWidget* create_medias_table_buttons()
  {
    QWidget* buttons_widget = new QWidget;
    QHBoxLayout* buttons_layout = new QHBoxLayout;

    QPushButton* list_presentations = new QPushButton("List presentations");
    QObject::connect(list_presentations, SIGNAL(clicked()), this, SLOT(list_presentations()));
    buttons_layout->addWidget(list_presentations);
    QPushButton* show_general_media_info = new QPushButton("General Information");
    buttons_layout->addWidget(show_general_media_info);

    buttons_widget->setLayout(buttons_layout);
    return buttons_widget;
  }

  QWidget* create_presentations_table_buttons()
  {
    QWidget* buttons_widget = new QWidget;
    QHBoxLayout* buttons_layout = new QHBoxLayout;
    
    QPushButton* back = new QPushButton("Back to medias");
    QObject::connect(back, SIGNAL(clicked()), this, SLOT(back_media_table_widget()));
    buttons_layout->addWidget(back);

    buttons_widget->setLayout(buttons_layout);
    return buttons_widget;
  }

  void remove_old_table()
  {
    delete presentations_table_widget_;
    presentations_table_widget_ = 0;
    delete medias_table_widget_;
    medias_table_widget_ = 0;
    delete buttons_widget;
    buttons_widget = 0;
  }

  ~debug_media_window()
  {
    Q_EMIT destroying_window_signal();
    ncl->resume_events();
  }

  ncl_widget* ncl;
  medias_table_widget* medias_table_widget_;
  presentations_table_widget* presentations_table_widget_;
  QWidget* buttons_widget;
Q_SIGNALS:
  void destroying_window_signal();

public Q_SLOTS:
  void back_media_table_widget()
  {
    remove_old_table();
    
    medias_table_widget_ = new medias_table_widget(ncl);
    layout()->addWidget(medias_table_widget_);

    buttons_widget = create_medias_table_buttons();
    layout()->addWidget(buttons_widget);
  }

  void list_presentations()
  {
    QTableWidgetItem* item = medias_table_widget_->media_table->item
      (medias_table_widget_->media_table->currentRow(), 0);
    assert(item != 0);
    std::string media_id = item->text().toStdString();

    remove_old_table();

    presentations_table_widget_ = new presentations_table_widget(media_id, ncl);
    layout()->addWidget(presentations_table_widget_);
    buttons_widget = create_presentations_table_buttons();
    layout()->addWidget(buttons_widget);
  }
};

} } } }

#endif
