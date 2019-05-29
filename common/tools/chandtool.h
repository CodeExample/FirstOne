#ifndef VXHANDTOOL_H
#define VXHANDTOOL_H

#include "ctools.h"

namespace  sm {


class CHandTool : public CAbstractTool
{
public:
    CHandTool(QWidget * =nullptr);
    ~CHandTool();

    int toolType() const override
    {
      return ToolHand;
    }

    void setGraphicsView(QGraphicsView *) override;

    void toolActivated() override;
private:
    QPoint _last_pos;

    bool _pressed=false;

    bool _action_in_progress;

    bool eventFilter(QObject *, QEvent *) override;

    void _scroll_area(QGraphicsSceneMouseEvent*);

    void lockTool() override {}

    void unlockTool() override {}

};

}


#endif // VXHANDTOOL_H
