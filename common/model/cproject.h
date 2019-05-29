#ifndef CPROJECT_H
#define CPROJECT_H


#include "cdocument.h"
#include "cenvironment.h"

namespace  sm {

class CProject : public CGraphics
{
public:
  static CProject createInstance();

  CProject() {}
  CProject(const CObject &object) : CGraphics(object) {}

  CDocument document() const;
};

}

#endif // CPROJECT_H
