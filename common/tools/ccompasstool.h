#ifndef CCOMPASSTOOL_H
#define CCOMPASSTOOL_H

#include "ctools.h"
#include "compasspanel.h"

namespace  sm {


class CCompassTool : public CAbstractTool
{
  Q_OBJECT
public:
    CCompassTool(QWidget * =nullptr);
    ~CCompassTool();

    int toolType() const override
    {
      return ToolCompass;
    }

    void setGraphicsView(QGraphicsView *) override;

    void toolActivated() override;

    QWidget * paramsPanel() override;

private:
    QPointF _center;
    QPointF _scene_center;
    double _angle;
    QPoint _last_pos;
    QCursor _cursor;
    bool _pressed=false;

    bool _action_in_progress;

    CompassPanel * _panel=nullptr;

    bool eventFilter(QObject *, QEvent *) override;

    void _rotate_area(QGraphicsSceneMouseEvent*);

    void lockTool() override {}

    void unlockTool() override {}

    double _get_angle();

private slots:
    void _reset();
    void _angle_changed(double);
};

}


#endif // CCOMPASSTOOL_H
