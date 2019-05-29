#ifndef VXSELECTIONTOOL_H
#define VXSELECTIONTOOL_H

#include "cframetool.h"

class CSelectionTool : public CFrameTool
{
public:
    CSelectionTool();
    ~CSelectionTool() override {}

    int toolType() const override
    {
      return ToolSelector;
    }

    bool frameAction(const QRectF &, QGraphicsSceneMouseEvent *) override;

    bool clickAction(QGraphicsSceneMouseEvent *) override;

    void toolActivated() override;

private:
    void _select_in_frame(const QRectF & scene_rect, bool keep_selection);
    void _select_intersect_frame(const QRectF & scene_rect, bool keep_selection);

};

#endif // VXSELECTIONTOOL_H
