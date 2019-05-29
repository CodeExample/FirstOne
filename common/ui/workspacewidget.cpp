#include "workspacewidget.h"
#include "ui_workspacewidget.h"

#include <math.h>

#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QDebug>

#include "cprojecthandler.h"
#include "citems.h"
#include "cshapeitem.h"
#include "glviewportwidget.h"

using namespace sm;

#define RULER_BACKGROUND   0xffeeeeee
#define RULER_PEN          0xff8d9199


WorkspaceWidget::WorkspaceWidget(QWidget *parent) :
  QWidget(parent), ui(new Ui::WorkspaceWidget), _mouse_move(false), _alt_pressed(false)
{
  ui->setupUi(this);

  QOpenGLWidget * viewport = new GLViewportWidget();
  QSurfaceFormat f;
  f.setSamples(8);
  viewport->setFormat(f);
  ui->graphicsView->setViewport(viewport);

  _horz_ruler = new vxRulerDecorator(ui->top_ruler, this, vxRulerDecorator::Horizontal);
  _vert_ruler = new vxRulerDecorator(ui->left_ruler, this, vxRulerDecorator::Vertical);
  _center_ruler = new vxRulerDecorator(ui->center_ruler, this, vxRulerDecorator::Center);

  _horz_ruler->setStyle(QBrush(QColor(RULER_BACKGROUND)), QBrush(QColor(RULER_BACKGROUND)),
                        QPen(QColor(RULER_PEN)), QPen(QColor(RULER_PEN)), QPen(QColor(RULER_PEN)));
  _vert_ruler->setStyle(QBrush(QColor(RULER_BACKGROUND)), QBrush(QColor(RULER_BACKGROUND)),
                        QPen(QColor(RULER_PEN), 1), QPen(QColor(RULER_PEN)), QPen(QColor(RULER_PEN)));
  _center_ruler->setStyle(QBrush(QColor(RULER_BACKGROUND)), QBrush(QColor(RULER_BACKGROUND)),
                        QPen(QColor(RULER_PEN), 1), QPen(QColor(RULER_PEN)), QPen(QColor(RULER_PEN)));

  _metric_changed(vxMetrics::vxmMillimeters);

  connect(CMetric::instance(), SIGNAL(metricChanged(vxMetrics)),
          this, SLOT(_metric_changed(vxMetrics)));

  connect(hprj, SIGNAL(projectReaded(CDocument)), this, SLOT(_project_readed(CDocument)));
}


WorkspaceWidget::~WorkspaceWidget()
{
  delete ui;
}


void WorkspaceWidget::init(QGraphicsScene * scene)
{
  ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
  ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
  ui->graphicsView->setMouseTracking(true);

  qApp->installEventFilter(this);
  ui->graphicsView->installEventFilter(this);

  ui->graphicsView->setScene(scene);
}



void WorkspaceWidget::_active_project_changed()
{
 // _metricChanged(vxEnvironment::currentMetric());
}

void WorkspaceWidget::_metric_changed(vxMetrics metric)
{
 // vxProject * project = vxEnvironment::activeProject();
  double dpi = hprj->document() ? hprj->document().resolutionDPI() : 72;
  switch(metric)
  {
  case vxmMillimeters:
    _horz_ruler->setCurrentDimension(CMetric::instance()->resolutionPxMm(72), 100, "mm");
    _vert_ruler->setCurrentDimension(CMetric::instance()->resolutionPxMm(72), 100, "mm");
    break;

  case vxmPixels:
    _horz_ruler->setCurrentDimension(72./dpi, 100,  "px");
    _vert_ruler->setCurrentDimension(72./dpi, 100,  "px");
    break;

  case vxmMicrons:
    _horz_ruler->setCurrentDimension(CMetric::instance()->resolutionPxMm(72), 100, "micron", 1000);
    _vert_ruler->setCurrentDimension(CMetric::instance()->resolutionPxMm(72), 100, "micron", 1000);
    break;

  case vxmInches:
    _horz_ruler->setCurrentDimension(72, 1000, tr("inch"));
    _vert_ruler->setCurrentDimension(72, 1000, tr("inch"));
    break;
  }

  _horz_ruler->update();
  _vert_ruler->update();
}





QGraphicsView * WorkspaceWidget::view() const
{
  return ui->graphicsView;
}


QWidget * WorkspaceWidget::viewport()
{
  return ui->graphicsView->viewport();
}


QPointF WorkspaceWidget::mapToScene(const QPoint& p) const
{
  return ui->graphicsView->mapToScene(p);
}


double WorkspaceWidget::scale() const
{
  return ui->graphicsView->matrix().m11();
}


void WorkspaceWidget::setInverseRuler(double w, double h)
{
  _horz_ruler->setInverseRelative(w);
  _vert_ruler->setInverseRelative(h);
}


void WorkspaceWidget::hideRulers()
{
  _horz_ruler->hide();
  _vert_ruler->hide();
  _center_ruler->hide();
}


void WorkspaceWidget::showRulers()
{
  _horz_ruler->show();
  _vert_ruler->show();
  _center_ruler->show();
}


QImage WorkspaceWidget::getSceneScreen() const
{
  QRectF r(QPointF(0,0), ui->graphicsView->viewport()->size());

  QImage scene_image(r.size().toSize(), QImage::Format_ARGB32);

  QRectF scene_r = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect()).boundingRect();

  vxEnvironment::instance()->setProperty(ATTR_HIDE_SELECTION, 1);

  QPainter painter(&scene_image);
  ui->graphicsView->scene()->render(&painter, r, scene_r);

  vxEnvironment::instance()->setProperty(ATTR_HIDE_SELECTION, 0);

  return scene_image;
}



void WorkspaceWidget::_increase_zoom_to(double d, const QPointF&)
{
  double resltn = 1;

  d *= resltn;

  QTransform tx;
  if (d > 0)
    tx.scale(1.09, 1.09);
  else
    tx.scale(0.91, 0.91);

  QTransform viewport_tx = ui->graphicsView->transform()*tx;

  ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  ui->graphicsView->setTransform(viewport_tx);

  ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}



 void WorkspaceWidget::_adjust_scrollbars()
 {
   if(ui->vertScrollBar->isSliderDown() ||
      ui->horzScrollBar->isSliderDown())
     return;

   QRectF scene_r = ui->graphicsView->sceneRect();
   QRectF view_on_scene_r = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect()).boundingRect();

   QRectF doc_r = hprj->document().boundingRect();//inEnvironment::activeDocument()

   if (view_on_scene_r.width() > doc_r.width() && view_on_scene_r.height() > doc_r.height())
   {
     ui->graphicsView->centerOn(doc_r.center());
   }

   if (view_on_scene_r.width() > doc_r.width())
   {
     ui->horzScrollBar->setVisible(false);
   } else {
     ui->horzScrollBar->setRange(scene_r.left(), scene_r.right()-view_on_scene_r.width());
     ui->horzScrollBar->setPageStep(view_on_scene_r.width());
     ui->horzScrollBar->setValue(view_on_scene_r.left());
     ui->horzScrollBar->setVisible(true);
   }

   if (view_on_scene_r.height() > doc_r.height())
   {
     ui->vertScrollBar->setVisible(false);
   } else {
     ui->vertScrollBar->setRange(scene_r.top(), scene_r.bottom()-view_on_scene_r.height());
     ui->vertScrollBar->setPageStep(view_on_scene_r.height());
     ui->vertScrollBar->setValue(view_on_scene_r.top());
     ui->vertScrollBar->setVisible(true);
   }

 }


bool WorkspaceWidget::eventFilter(QObject * object, QEvent * event)
{
  if (event->type() == QEvent::Paint)
  {
    _adjust_scrollbars();
    return false;
  }

  bool accepted=false;
  switch(event->type())
  {
  case QEvent::Resize:
    ui->graphicsView->viewport()->setGeometry(ui->graphicsView->contentsRect());
    break;

  case QEvent::GraphicsSceneMouseMove:
  {
    QGraphicsSceneMouseEvent * m = static_cast<QGraphicsSceneMouseEvent*>(event);
    _horz_ruler->setCursorPos(m->scenePos().x());
    _vert_ruler->setCursorPos(m->scenePos().y());
    _mouse_move = true;
    break;
  }

  case QEvent::GraphicsSceneWheel:
  {
    if (_alt_pressed)
    {
      QGraphicsSceneWheelEvent * w = static_cast<QGraphicsSceneWheelEvent*>(event);
      if ((_prev_delta > 0 && w->delta() > 0) ||
          (_prev_delta < 0 && w->delta() < 0))
      {
        _increase_zoom_to(w->delta(), w->scenePos());
      }
      _prev_delta = w->delta();
      accepted = true;
      event->accept();
    }
    break;
  }

  case QEvent::FocusIn:
  {
    if (object == ui->graphicsView)
    {
      _focused = true;
    }
    break;
  }

  case QEvent::FocusOut:
  {
    if (object == ui->graphicsView)
    {
      _focused = false;
    }
    break;
  }

  case QEvent::KeyPress:
  {
    if (_focused)
    {
      QKeyEvent * ke = static_cast<QKeyEvent*>(event);

      if (ke->key() == Qt::Key_Alt)
      {
        _alt_pressed = true;
        accepted = false;
      }
    }
    break;
  }
  case QEvent::KeyRelease:
  {
    if (_focused)
    {
      QKeyEvent * ke = static_cast<QKeyEvent*>(event);

      if (ke->isAutoRepeat())
        break;

      _alt_pressed = false;
      accepted = false;
    }
    break;
  }
  case QEvent::MouseMove:
  {
    QMouseEvent * me = static_cast<QMouseEvent*>(event);
    if (me->buttons() == Qt::NoButton)
    {
      QPointF scene_pos = ui->graphicsView->mapToScene(ui->graphicsView->mapFromGlobal(me->globalPos()));

      QGraphicsScene * scene = ui->graphicsView->scene();
      for(QGraphicsItem * item : scene->items())
      {
        if (item->type() == ShapeItem)
        {
          QPointF item_pos = item->mapFromScene(scene_pos);
          CShapeItem * shape_item = static_cast<CShapeItem*>(item);
          if (item->contains(item_pos))
          {
            item->setAcceptHoverEvents(true);
          } else {
            shape_item->hoverLeave();
            item->setAcceptHoverEvents(false);
          }
        }
      }
    }
    break;
  }


  default:;
  }

  return accepted;
}


void WorkspaceWidget::on_vertScrollBar_sliderMoved(int position)
{
  QRectF scene_r = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect())
      .boundingRect();

  QPointF c = scene_r.center();

  ui->graphicsView->centerOn(c.x(), position + scene_r.height()/2);
}


void WorkspaceWidget::on_horzScrollBar_sliderMoved(int position)
{
  QRectF scene_r = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect())
      .boundingRect();

  QPointF c = scene_r.center();

  ui->graphicsView->centerOn(position + scene_r.width()/2, c.y());
}


bool WorkspaceWidget::_scrollbar_action(QScrollBar*bar,int action)
{
  switch(action)
  {
  case QAbstractSlider::SliderSingleStepAdd:
    bar->setValue(bar->value() + bar->singleStep());
    break;
  case QAbstractSlider::SliderSingleStepSub:
    bar->setValue(bar->value() - bar->singleStep());
    break;
  case QAbstractSlider::SliderPageStepAdd:
    bar->setValue(bar->value() + bar->pageStep());
    break;
  case QAbstractSlider::SliderPageStepSub:
    bar->setValue(bar->value() - bar->pageStep());
    break;
  case QAbstractSlider::SliderToMinimum:
    bar->setValue(bar->minimum());
    break;
  case QAbstractSlider::SliderToMaximum:
    bar->setValue(bar->maximum());
    break;
  default:;
    return false;
  }
  return true;
}

void WorkspaceWidget::on_vertScrollBar_actionTriggered(int action)
{
  if (_scrollbar_action(ui->vertScrollBar, action))
  {
    QRectF scene_r = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect())
        .boundingRect();

    QPointF c = scene_r.center();

    ui->graphicsView->centerOn(c.x(), ui->vertScrollBar->value() + scene_r.height()/2);
  }
}


void WorkspaceWidget::on_horzScrollBar_actionTriggered(int action)
{
  if (_scrollbar_action(ui->horzScrollBar, action))
  {
    QRectF scene_r = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect())
        .boundingRect();

    QPointF c = scene_r.center();

    ui->graphicsView->centerOn(ui->horzScrollBar->value() + scene_r.width()/2, c.y());
  }
}

void WorkspaceWidget::on_comboBox_activated(int index)
{
  switch(index)
  {
  case 0:
    metr->setCurrentMetric(vxmMillimeters);
    break;
  case 1:
    metr->setCurrentMetric(vxmPixels);
    break;
  case 2:
    metr->setCurrentMetric(vxmInches);
    break;
  case 3:
    metr->setCurrentMetric(vxmMicrons);
    break;
  }
}

void WorkspaceWidget::zoomToFit()
{
  QRectF items_r = ui->graphicsView->scene()->itemsBoundingRect();
  ui->graphicsView->fitInView(items_r);
}

void WorkspaceWidget::_project_readed(const CDocument &doc)
{
  ui->graphicsView->fitInView(doc.boundingRect(), Qt::KeepAspectRatio);
}
