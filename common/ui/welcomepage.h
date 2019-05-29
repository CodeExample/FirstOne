#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include "ui_welcomepage.h"



class WelcomePage : public QFrame, private Ui::WelcomePage
{
  Q_OBJECT

public:
  explicit WelcomePage(QWidget *parent = 0);

  bool eventFilter(QObject *, QEvent *);
  void dropEvent(QDropEvent * drop_event);
  void dragEnterEvent(QDragEnterEvent * drag_event);

public slots:
  void initProtection();

private:
  bool _moved;
  bool _drop_from_uri_list(const QMimeData * mime_data);
  bool _drop_image(const QMimeData * mime_data);

private slots:
  void on_label_linkActivated(const QString &link);
  void on_label_2_linkActivated(const QString &link);
};

#endif // WELCOMEPAGE_H
