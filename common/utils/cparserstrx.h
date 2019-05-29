#ifndef CPARSERSTRX_H
#define CPARSERSTRX_H

#include "cgraphics.h"

#include <QXmlDefaultHandler>


namespace sm
{


class CParserSTRX
{
public:
  CParserSTRX(const QString &file_name);

  bool parseTo(const CGraphics&);
  CGraphics document;
private:
  QString _file_name;
};

}
#endif // CPARSERSTRX_H
