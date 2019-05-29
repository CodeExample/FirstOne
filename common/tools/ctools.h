#ifndef CABSTRACTTOOLBASE_H
#define CABSTRACTTOOLBASE_H

#include <QGraphicsView>

namespace sm
{
#define TOOL_ITEMS_Z_ORDER 99999

enum Tools
{
  NoTool = 0,
  ToolSelector,
  ToolLasso,
  ToolHand,
  ToolCompass,
  ToolRectangle,
  ToolEllipse,
  ToolAngle,
  ToolInterval,
  ToolBasrelief,
  ToolThickness,
  ToolThreshold,
  ToolDottedLine,
  ToolSmooth,
};


class CAbstractTool : public QObject
{
public:
  CAbstractTool(QWidget* =nullptr);
  ~CAbstractTool() override;

  virtual int toolType() const=0;

  void attachTool(CAbstractTool*);

  void setEnabled(bool enbl)
  {
    _enabled_tool = enbl;
  }

  virtual void setGraphicsView(QGraphicsView*);

  virtual void toolActivated();
  virtual void toolDeactivated();

  virtual QWidget * paramsPanel();

  bool eventFilter(QObject *, QEvent *) override;

protected:
  CAbstractTool * _attached_tool;

  QPoint _press_ev_pos;
  QPoint _press_ev_globalPos;
  Qt::MouseButton _press_ev_button;
  Qt::MouseButtons _press_ev_buttons;
  Qt::KeyboardModifiers _press_ev_modifiers;

  bool _mouse_press;
  bool _mouse_move;
  bool _mouse_drag;
  bool _can_accept_drag;
  bool _can_accept_click;
  bool _key_pressed;

  bool _enabled_tool;

  QGraphicsView * _view;
  QGraphicsScene * _scene;

  virtual bool canAcceptDrag(QMouseEvent*) {return false;}
  virtual void mouseBeginDrag(QMouseEvent*) {}
  virtual void mouseEndDrag(QMouseEvent*) {}
  virtual void mouseDrag(QMouseEvent*) {}

  virtual bool canAcceptClick(QMouseEvent*) {return false;}
  virtual void mouseClick(QMouseEvent*) {}

  virtual bool mouseDblClick(QMouseEvent*) {return false;}

  virtual void mouseMove(QMouseEvent*) {}

  virtual bool canAcceptHover(QMouseEvent*) {return false;}
  virtual void mouseHoverEnter(QMouseEvent*) {}
  virtual void mouseHoverLeave(QMouseEvent*) {}
  virtual void lockTool() =0;
  virtual void unlockTool() =0;

  virtual bool keyDown(QKeyEvent*) {return false;}
  virtual void keyUp(QKeyEvent*) {}

private:
  static CAbstractTool * _hovered_tool;

  void _check_mouse_hover(QMouseEvent*);

};

}

#endif // CABSTRACTTOOLBASE_H
