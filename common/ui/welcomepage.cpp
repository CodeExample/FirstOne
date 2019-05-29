#include "welcomepage.h"

#include "cprojecthandler.h"

#include <QDropEvent>
#include <QImageReader>

using namespace sm;


WelcomePage::WelcomePage(QWidget *parent) :
  QFrame(parent), _moved(false)
{
  setupUi(this);
  /*setAcceptDrops(false);
  setMouseTracking(true);

  label->installEventFilter(this);
  label_2->installEventFilter(this);*/
}


bool WelcomePage::eventFilter(QObject *, QEvent *e)
{
  switch(e->type())
  {
  case QEvent::MouseButtonPress:
  {
    _moved = false;
    return true;
  }

  case QEvent::MouseMove:
  {
    _moved = true;
    return true;
  }

  case QEvent::MouseButtonRelease:
  {
    if (!_moved)
      hprj->loadImage("");

    return true;
  }
  default:;
  }

  return false;
}


void WelcomePage::on_label_linkActivated(const QString &)
{
 // vEnvironment::requestActionWizard(true);
}


void WelcomePage::on_label_2_linkActivated(const QString &)
{
  //vEnvironment::requestActionWizard(true);
}



void WelcomePage::initProtection()
{
#ifdef PROTECTION_CFFA
/*  if (vEnvironment::serialServer()->testCoefficients())
  {
    registerButton->setVisible(false);
  } else {
    registerButton->setVisible(true);
  }*/
#endif
}



void WelcomePage::dropEvent(QDropEvent * drop_event)
{
  if (drop_event->mimeData()->hasImage())
  {
    if (_drop_image(drop_event->mimeData()))
    {
      drop_event->acceptProposedAction();
      return;
    }
  } else {
    if (drop_event->mimeData()->hasFormat("text/uri-list"))
    {
      if (_drop_from_uri_list(drop_event->mimeData()))
      {
        drop_event->acceptProposedAction();
        return;
      }
    }
  }
  drop_event->ignore();
}

bool WelcomePage::_drop_image(const QMimeData * mime_data)
{
  QImage image = qvariant_cast<QImage>(mime_data->imageData());

  if (image.isNull())
    return false;

  hprj->loadImage(image);


  return true;
}


bool WelcomePage::_drop_from_uri_list(const QMimeData * mime_data)
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
  if (!formats.contains(suffix.toLatin1()))
    return false;

  hprj->loadImage(file_info.absoluteFilePath());

  return true;
}


void WelcomePage::dragEnterEvent(QDragEnterEvent * drag_event)
{
  if (drag_event->mimeData()->hasFormat("text/uri-list") ||
      drag_event->mimeData()->hasImage())
  {
    drag_event->acceptProposedAction();
  } else {
    drag_event->ignore();
  }
}
