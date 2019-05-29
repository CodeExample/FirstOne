#include "cproject.h"

#include "cattributes.h"
#include "cdocumentitem.h"
#include "cprojecthandler.h"

using namespace  sm;

class ProjectImpl : public GraphicsImpl
{
public:

  ProjectImpl() {}

  bool read(const QDomElement &dom_elm) override
  {
    QDomElement document_elm = dom_elm.firstChildElement(TAG_DOCUMENT);
    if (document_elm.isNull())
      return false;

    _document = CModel::readObject(document_elm);

    QDomElement objects_elm = dom_elm.firstChildElement(TAG_OBJECTS);
    if (objects_elm.isNull())
      return false;

    const CLayer &root = _document.root();

    if (!root.readObjects(objects_elm))
      return false;

    return true;
  }

  CDocument _document;
};





CDocument CProject::document() const
{
  IMPL_D(Project);
  return d->_document;
}

CProject CProject::createInstance()
{
  return CProject(new ProjectImpl());
}
