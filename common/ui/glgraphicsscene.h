#ifndef GLGRAPHICSSCENE_H
#define GLGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QMimeData>

class GLGraphicsScene : public QGraphicsScene
{
public:
  GLGraphicsScene();

protected:

  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
  bool _drop_image(const QMimeData *);
  bool _drop_from_uri_list(const QMimeData *);

};

#endif // GLGRAPHICSSCENE_H
