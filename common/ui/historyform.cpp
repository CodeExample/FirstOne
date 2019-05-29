#include "historyform.h"

#include "cprojecthandler.h"

using namespace sm;

HistoryForm::HistoryForm(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  undoView->setStack(hprj->undoStack());

  buttonUndo->setDefaultAction(hprj->action(ActionUndo));
  buttonRedo->setDefaultAction(hprj->action(ActionRedo));
}


