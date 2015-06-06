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

#include <ghtv/qt/ncl/player/text_player.hpp>

namespace ghtv { namespace qt { namespace ncl { namespace player {

text_player::text_player(std::string const& source, std::string const& root_path, QWidget* player_container
                         , ncl_widget* ncl, gntl::structure::composed::component_location
                         <std::string, std::string> location
                         , ncl::executor* e)
  : common_player(e, location), ncl(ncl), label(0), file(0)
{
  std::auto_ptr<QLabel> label(new QLabel(player_container));
  label->move(QPoint(0, 0));
  label->resize(player_container->size());
  label->show();
  this->label = label.release();
  
  file = new url_file(create_url(source, root_path), player_container);

  if(!file->local())
  {
    QObject::connect(file, SIGNAL(download_finished_signal()), this
                     , SLOT(text_download()));
    QObject::connect(file, SIGNAL(error_signal(std::string)), this, SLOT(download_error(std::string)));
  }
  else 
    text_download();
}

void text_player::download_error(std::string error_msg)
{
  ncl->error_occurred(error_msg);
}

void text_player::pause()
{
}

void text_player::resume()
{
}

void text_player::key_process(std::string const& key, bool pressed)
{
}
  
bool text_player::wants_keys() const
{
  return false;  
}
bool text_player::start_set_property(std::string const& name, std::string const& value)
{
  return false;
}
bool text_player::commit_set_property(std::string const& name)
{
  return false;
}

void text_player::text_download()
{
  QByteArray content = file->io_device()->readAll();
  if(content.size() > 0)
  {
    QString string = QString::fromLatin1(content.constData());
    label->setTextFormat(Qt::PlainText);
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setText(string);
    setup_timers();
  }
}

} } } }
