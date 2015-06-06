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

#include <ghtv/qt/ncl/player/image_player.hpp>

#include <QImageReader>

namespace ghtv { namespace qt { namespace ncl { namespace player {

image_player::image_player(std::string const& source, std::string const& root_path, QWidget* player_container
                           , ncl_widget* ncl, gntl::structure::composed::component_location
                           <std::string, std::string> location
                           , ncl::executor* executor_)
  : common_player(executor_, location), ncl(ncl)
{
  std::auto_ptr<QLabel> label(new QLabel(player_container));
  label->move(QPoint(0, 0));
  label->resize(player_container->size());
  size = label->size();

  QString tooltip("Media: ");
  tooltip += location.identifier().c_str();
  tooltip += "\nSource: ";
  tooltip += source.c_str();
  // tooltip += "\nContext: ";
  // tooltip += context_identifier.c_str();
  label->setToolTip(tooltip);
  this->label = label.get();

  url = create_url(source, root_path);
  file = new url_file(url, label.get());
  if(!file->local())
  {
    QObject::connect(file, SIGNAL(download_finished_signal()), this
                     , SLOT(image_download()));
    QObject::connect(file, SIGNAL(error_signal(std::string)), this, SLOT(download_error(std::string)));
  }
  else 
    image_download();
  
  label.release();
}

void image_player::download_error(std::string error_msg)
{
  ncl->error_occurred(error_msg);
}

void image_player::key_process(std::string const& key, bool pressed)
{
}

bool image_player::wants_keys() const
{
  return false; 
}

bool image_player::start_set_property(std::string const& name, std::string const& value)
{
  return false;
}

bool image_player::commit_set_property(std::string const& name)
{
  return false;
}

void image_player::pause()
{
}

void image_player::resume()
{
}

void image_player::image_download()
{
  QImage unscaled_image;
  unscaled_image.load(file->io_device(), "");
  if(!unscaled_image.isNull())
  {
    QImage image = unscaled_image.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap image_pix = QPixmap::fromImage(image);
    label->setPixmap(image_pix);
    label->show();
  }
  else
  {
    std::string error = "Unknown error loading image:\n";
    error += url.toString().toStdString();
    ncl->error_occurred(error);
  }
  setup_timers();
}

} } } }
