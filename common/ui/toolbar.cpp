#include "toolbar.h"

#include "cprojecthandler.h"

using namespace sm;

ToolBar::ToolBar(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  connect(CProjectHandler::instance(), SIGNAL(toolChanged(CAbstractTool*)),
          this, SLOT(_tool_changed(CAbstractTool*)));

  buttonSelector->setDefaultAction(hprj->action(ActionSelector));
  buttonPen->setDefaultAction(hprj->action(ActionPen));
  buttonLasso->setDefaultAction(hprj->action(ActionLasso));
  buttonShowImage->setDefaultAction(hprj->action(ActionShowImage));
  buttonAddEllipse->setDefaultAction(hprj->action(ActionEllipse));
  buttonAddSquare->setDefaultAction(hprj->action(ActionSquare));
  buttonSelector->setAutoRaise(true);
  buttonPen->setAutoRaise(true);
  buttonLasso->setAutoRaise(true);
  buttonShowImage->setAutoRaise(true);
  buttonAddEllipse->setAutoRaise(true);
  buttonAddSquare->setAutoRaise(true);
}



void ToolBar::_tool_changed(CAbstractTool * tool)
{
  while(toolParamsLayout->count() > 0)
  {
    QLayoutItem *item = toolParamsLayout->takeAt(0);
    QWidget* widget = item->widget();

    if (widget)
      delete widget;

    delete item;
  }

  QWidget * params = tool->paramsPanel();
  if (params)
  {
    toolParamsLayout->addWidget(params);
  }
}

void ToolBar::on_buttonShowImage_clicked(bool)
{

}
