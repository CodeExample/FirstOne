#include "glgraphicsscene.h"

#include "cenvironment.h"
#include "cattributes.h"

#include <QDebug>
#include <QGraphicsSceneDragDropEvent>
#include <QImageReader>


using namespace sm;


GLGraphicsScene::GLGraphicsScene()
{
}



void GLGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
  event->acceptProposedAction();
}

void GLGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
  event->acceptProposedAction();
}


void GLGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  if (event->mimeData()->hasFormat("text/uri-list") ||
      event->mimeData()->hasImage())
  {
    event->acceptProposedAction();
  }
  else
  {
    event->ignore();
  }
}


void GLGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  if (event->mimeData()->hasImage())
  {
    if (_drop_image(event->mimeData()))
    {
      event->acceptProposedAction();
    }
  } else
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
      if (_drop_from_uri_list(event->mimeData()))
      {
        event->acceptProposedAction();
      }
    }
}




bool GLGraphicsScene::_drop_image(const QMimeData * mime_data)
{
  QImage image = qvariant_cast<QImage>(mime_data->imageData());

  if (image.isNull())
    return false;

  CEnvironment::notifyImageObjectDroppedToWorkspace(image);
  return true;
}




bool GLGraphicsScene::_drop_from_uri_list(const QMimeData * mime_data)
{
  QList<QUrl> urls_list = mime_data->urls();
  if (urls_list.count() != 1)
    return false;

  QList<QByteArray> formats = QImageReader::supportedImageFormats();

  QUrl url = urls_list.first();

  if (!url.isLocalFile())
    return false;

  QFileInfo file_info(url.toLocalFile());
  QString suffix = file_info.suffix().toLower();

  if (formats.contains(suffix.toLatin1()))
  {
    CEnvironment::notifyImageDroppedToWorkspace(file_info.absoluteFilePath());
    return true;
  }

  if (suffix == EXT_STRX)
  {
    CEnvironment::requestOpenProject(file_info.absoluteFilePath());
    return true;
  }

  return false;
}

