#ifndef CGROUP_H
#define CGROUP_H

#include "cgraphics.h"

namespace sm
{

class CGroup : public CGraphics
{
public:
  static CGraphics createInstance(const QString &name="");

  CGroup() : CGraphics() {}
  CGroup(const CObject &object) : CGraphics(object) {}

};

}

#endif // CGROUP_H
