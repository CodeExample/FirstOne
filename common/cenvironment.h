#ifndef CENVIRONMENT_H
#define CENVIRONMENT_H

#include <QtCore>
#include <QDomDocument>
#include <QDockWidget>
#include <QDialog>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QOpenGLWidget>
#include <QSet>

#define ATTR_STROKES_MAKER     "strokes_maker"
#define ATTR_PROJECT_CONFIG    "project_config"

#define ATTR_VIEW_RECT         "view_rect"

#define MM_PER_INCH         25.4
#define PIX_PER_MM          2.83


#define metr CMetric::instance()

namespace sm
{

enum vxMetrics
{
  vxmMillimeters = 0,
  vxmPixels,
  vxmInches,
  vxmMicrons
};


class CEnvironment : public QObject
{
  Q_OBJECT

public:
  static CEnvironment * instance()
  {
    if (!_instance)
    {
      _instance = new CEnvironment();
    }
    return _instance;
  }


  static QString layoutFileName();

  static void notifyImageDroppedToWorkspace(const QString& file_name)
  {
    emit instance()->imageDroppedToWorkspace(file_name);
  }

  static void notifyImageObjectDroppedToWorkspace(const QImage& image)
  {
    emit instance()->imageObjectDroppedToWorkspace(image);
  }

  static void requestOpenProject(const QString& file_name)
  {
    emit instance()->openProjectRequest(file_name);
  }


private:
  static CEnvironment * _instance;

signals:
  void imageDroppedToWorkspace(QString file_name);
  void imageObjectDroppedToWorkspace(QImage image);
  void openProjectRequest(QString file_name);
};


class CMetric : public QObject
{
  Q_OBJECT

public:
  static QString getMetricTitle(vxMetrics);

  static vxMetrics currentMetric();

  static void setCurrentMetric(vxMetrics);

  static double convertMetricToPx(vxMetrics, double dpi, double value);

  static double convertMetricToPx(double values);

  static double convertPxToMetric(vxMetrics, double dpi, double value);

  static double convertPxToMetric(double value);

  static double resolutionPxMm(double dpi);

  static CMetric * instance()
  {
    if (!_instance)
    {
      _instance = new CMetric();
    }
    return _instance;
  }

private:
  static vxMetrics _current_metric;
  static double _dpi;
  static CMetric *_instance;

signals:
  void metricChanged(vxMetrics);

};


}

#endif // CENVIRONMENT_H
