#include "cenvironment.h"

#include <QDir>
#include <QStandardPaths>

using namespace sm;

CEnvironment * CEnvironment::_instance=nullptr;

QString CEnvironment::layoutFileName()
{
  QString config_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  if (!QDir(config_path).exists())
  {
    QDir("").mkpath(config_path);
  }

  return QFileInfo(QDir(config_path), "layout.cfg").absoluteFilePath();
}


vxMetrics CMetric::_current_metric = vxmMillimeters;

double CMetric::_dpi = 0;

CMetric *CMetric::_instance=nullptr;


void CMetric::setCurrentMetric(vxMetrics m)
{
  if (_current_metric != m)
  {
    _current_metric = m;
    emit _instance->metricChanged(m);
  }
}

QString CMetric::getMetricTitle(vxMetrics metrics)
{
  switch(metrics)
  {
  case vxmMillimeters:
    return "mm";

  case vxmPixels:
    return "px";

  case vxmInches:
    return "inch";

  case vxmMicrons:
    return "mcrn";
  }

  return "undef";
}


vxMetrics CMetric::currentMetric()
{
  return _current_metric;
}

double CMetric::convertMetricToPx(double value)
{
  return convertMetricToPx(_current_metric, _dpi, value);
}


double CMetric::resolutionPxMm(double dpi)
{
  return dpi/MM_PER_INCH;
}


double CMetric::convertMetricToPx(vxMetrics metrics, double dpi, double value)
{
  switch(metrics)
  {
  case vxmMillimeters:
    return value * resolutionPxMm(dpi);

  case vxmPixels:
    return value;

  case vxmInches:
    return value * dpi;

  case vxmMicrons:
    return value * resolutionPxMm(dpi)/1000;
  }

  return value;
}


double CMetric::convertPxToMetric(vxMetrics metrics, double dpi, double value)
{
  switch(metrics)
  {
  case vxmMillimeters:
    return value/resolutionPxMm(dpi);

  case vxmPixels:
    return value;

  case vxmInches:
    return value/dpi;

  case vxmMicrons:
    return 1000*value/resolutionPxMm(dpi);
  }

  return 1;
}


double CMetric::convertPxToMetric(double value)
{
  return convertPxToMetric(_current_metric, _dpi, value);
}

