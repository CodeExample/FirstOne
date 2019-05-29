#include "cparserstrx.h"

#include "cattributes.h"
#include "clayer.h"
#include "cgroup.h"
#include "cshape.h"
#include "ccurve.h"
#include "cdocument.h"
#include "cimage.h"

#include <QXmlSimpleReader>
#include <QBuffer>
#include <QByteArray>
#include <QTime>

using namespace sm;


struct vNode
{
  enum States
  {
    Empty     = 0x0,
    On        = 0x1,
    Spline    = 0x3,
    Curve     = 0x4,
    Cubic     = 0x5,
    NodesMask = 0x7,

    Smooth    = 0x8,
    Selected  = 0x10,

    Locked    = 0x20,

    R_x40     = 0x40,
    R_x80     = 0x80,
  };

  QPointF point;
  uint state;
};



class StrxSAXHandler : public QXmlDefaultHandler
{
public:

  StrxSAXHandler(const CGraphics &root)
  {
    stack << root;
  }


  bool parseFile(const QString &fileName)
  {
    QFile file(fileName);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    _file_name = fileName;
    return reader.parse(inputSource);
  }


  bool parseString(const QString &plist_str)
  {
    QXmlInputSource inputSource;
    inputSource.setData(plist_str);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    return reader.parse(inputSource);
  }

  bool fatalError(const QXmlParseException &exception)
  {
    qWarning() << "Parse error at line " << exception.lineNumber()
               << ", " << "column " << exception.columnNumber() << ": "
               << exception.message();

    return false;
  }


  bool startElement(const QString &,
                    const QString &,
                    const QString &tag,
                    const QXmlAttributes &attr)
  {
    QString caption = attr.value("caption");
    CGraphics top = stack.last();
    if (tag == TAG_LR_SECTION)
    {
      if (stack.count() <= 1)
      {
        CLayer layer = CLayer::createInstance(caption);
        top.add(layer);
        stack.push_back(layer);
      } else {
        CGroup group = CGroup::createInstance(caption);
        top.add(group);
        stack.push_back(group);
      }
      return true;
    }
    if (tag == TAG_FREE_MESH)
    {
      CShape shape = CShape::createInstance(caption);
      top.add(shape);
      stack.push_back(shape);
      return true;
    }
    if (tag == TAG_TMPL_LINEAR)
    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance("Linear");
      top.add(tmpl);
      stack.push_back(tmpl);
      return true;
    }
    if (tag == TAG_TMPL_FREE)
    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance("Free");
      top.add(tmpl);
      stack.push_back(tmpl);
      return true;
    }
    if (tag == TAG_VECTOR_MASK)
    {
      CMask mask = CMask::createInstance();
      CShape shape = stack.last();
      if (shape.type() == Shape)
      {
        shape.setMask(mask);
      }
      stack.push_back(mask);
      return true;
    }
    if (tag == TAG_CURVE ||
        tag == TAG_curve)
    {
      CCurve curve = CCurve::createInstance();
      stack.push_back(curve);
      _nodes_count = attr.value(ATTR_NODES_COUNT).toInt();
      _closed = attr.value(ATTR_CLOSED).toInt();
      return true;
    }
    if (tag == TAG_NODES)
    {
      _is_nodes = true;
      return true;
    }
    if (tag == TAG_DOCUMENT)
    {
      double w_mm = attr.value(ATTR_WIDTH_MM).toDouble();
      double h_mm = attr.value(ATTR_HEIGHT_MM).toDouble();
      int dpi = attr.value(ATTR_DPI).toInt();
      CDocument doc(QSizeF(w_mm, h_mm), dpi, vxmMillimeters);
      stack.push_back(doc);
      _document = doc;
      _document.setFileName(_file_name);
      return true;
    }
    if (tag == TAG_IMAGES)
    {
      return true;
    }
    if (tag == TAG_OBJECTS)
    {
      return true;
    }
/*    if (tag == TAG_BIG_PICT)
    {
      CImage pict = CImage::createInstance();
      stack.append(pict);
      return true;
    }
    if (tag == TAG_IMAGE_DATA)
    {
      CImage pict = stack.last();
      _read_path_curve()
    }

*/
    indent += "  ";
    qDebug() << QString("%1<%2>").arg(indent).arg(tag);

    return true;
  }



  bool endElement(const QString &,
                  const QString &,
                  const QString &tag)
  {
    indent.remove(0,2);
    if (tag == TAG_LR_SECTION ||
        tag == TAG_FREE_MESH ||
        tag == TAG_TMPL_LINEAR ||
        tag == TAG_TMPL_FREE ||
        tag == TAG_VECTOR_MASK)
    {
      stack.pop_back();
      return true;
    }
    if (tag == TAG_CURVE ||
        tag == TAG_curve)
    {
      CCurve curve = stack.takeLast();
      if (stack.last().type() == Mask)
      {
        CMask mask = stack.last();
        mask.setPath(curve.path());
      }
      return true;
    }
    if (tag == TAG_NODES)
    {
      _is_nodes = false;
      return true;
    }
    if (tag == TAG_DOCUMENT)
    {
      _document = stack.takeLast();
      return true;
    }


    return true;
  }


  bool characters(const QString &str)
  {
    if (_is_nodes)
    {
      QPainterPath path = _read_path_curve(str, _nodes_count, _closed);
      if (stack.last().type() == Curve)
      {
        CCurve curve = stack.last();
        curve.setPath(path);
      }
      return true;
    }


    return true;
  }


  QPainterPath _read_path_curve(const QString &str, int nodes_cnt, bool closed)
  {
    QPainterPath path;
    QByteArray ba = str.toLatin1();

    QByteArray decompressed = qUncompress(QByteArray::fromBase64(ba.data()));

    QBuffer buffer(&decompressed);
    buffer.open(QIODevice::ReadOnly);
    qint64 sz_node = sizeof(vNode);

    QList<vNode> nodes;
    for(int i=0; i < nodes_cnt;++i)
    {
      vNode node;
      buffer.read((char*)&node, sz_node);
      nodes << node;
    }

    buffer.close();

    for(int i=0; i < nodes_cnt;)
    {
      if (nodes.at(i).state & vNode::On)
      {
        if (path.elementCount() == 0)
          path.moveTo(nodes.at(i++).point);
        else
          path.lineTo(nodes.at(i++).point);
      } else
      if (nodes.at(i).state & vNode::Curve)
      {
        path.cubicTo(nodes.at(i++).point,
                     nodes.at(i++).point,
                     nodes.at((i++)%nodes_cnt).point);
      }
    }

    if (closed)
    {
      path.closeSubpath();
    }

    return path;
  }

  QString indent;
  QList<CGraphics> stack;
  bool _is_nodes = false;
  int _nodes_count;
  bool _closed=false;
  CDocument _document;
  QString _file_name;
};




CParserSTRX::CParserSTRX(const QString &file_name) : _file_name(file_name)
{
}


bool CParserSTRX::parseTo(const CGraphics &target)
{
  StrxSAXHandler saxh(target);
  saxh.parseFile(_file_name);
  document = saxh._document;
  return true;
}
