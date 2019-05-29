#-------------------------------------------------
#
# Project created by QtCreator 2018-01-01T12:54:54
#
#-------------------------------------------------

QT       += core gui opengl

CONFIG += c++11

QMAKE_MAC_SDK = macosx10.12

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets printsupport macextras xml
  DEFINES += HAVE_QT5
}

TARGET = StrokesMaker3
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS



SOURCES += \
        $$PWD/main.cpp \
        $$PWD/mainwindow.cpp \
        $$PWD/common/items/coglpainterthread.cpp \
        $$PWD/common/model/cstrokestemplate.cpp \
        $$PWD/common/items/citems.cpp \
        $$PWD/common/model/cshape.cpp \
        $$PWD/common/model/cobject.cpp \
        $$PWD/common/model/cgraphics.cpp \
        $$PWD/common/model/clayer.cpp \
        $$PWD/common/model/cmodel.cpp \
        $$PWD/common/controllers/cprojecthandler.cpp \
        $$PWD/common/utils/vintervals.cpp \
        $$PWD/common/utils/utils.cpp \
        $$PWD/common/model/cmask.cpp \
        $$PWD/common/tools/ctools.cpp \
        $$PWD/common/tools/chandtool.cpp \
        $$PWD/common/tools/cselectiontool.cpp \
        $$PWD/common/tools/cframetool.cpp \
        $$PWD/common/model/cdocument.cpp \
        $$PWD/common/items/cdocumentitem.cpp \
        $$PWD/common/tools/ccompasstool.cpp \
        $$PWD/common/ui/toolbar.cpp \
        $$PWD/common/ui/compasspanel.cpp \
        $$PWD/common/ui/workspacewidget.cpp \
        $$PWD/common/ui/rullerdecorator.cpp \
        $$PWD/common/ui/welcomepage.cpp \
        $$PWD/common/ui/debugwidget.cpp \
        $$PWD/common/ui/objectstreemodel.cpp \
        $$PWD/common/ui/layersform.cpp \
        $$PWD/common/cenvironment.cpp \
        $$PWD/common/ui/historyform.cpp \
        $$PWD/common/tools/cshapetool.cpp \
        $$PWD/common/model/ccommands.cpp \
        $$PWD/common/items/coglstrokespainter.cpp \
        $$PWD/common/tools/cscenetool.cpp \
        $$PWD/common/ui/valuecontrol.cpp \
        $$PWD/common/tools/classotool.cpp \
        $$PWD/common/ui/paramspanelwidget.cpp \
        $$PWD/common/utils/fltransformframe.cpp \
        $$PWD/common/controls/ctransformcontrol.cpp \
        $$PWD/common/items/cshapeitem.cpp \
        $$PWD/common/items/crendermanager.cpp \
        $$PWD/common/items/coglstrokesrender.cpp \
        $$PWD/common/items/coglrenderthread.cpp \
        $$PWD/common/model/cgroup.cpp \
        $$PWD/common/items/cgroupitem.cpp \
        $$PWD/common/utils/cparserstrx.cpp \
        $$PWD/common/model/ccurve.cpp \
        $$PWD/common/model/cimage.cpp \
        $$PWD/common/ui/glviewportwidget.cpp \
        $$PWD/common/ui/glgraphicsscene.cpp \
        $$PWD/common/ui/docparams.cpp \
        $$PWD/common/ui/newprojectdlg.cpp \
        $$PWD/common/controllers/citemscontroller.cpp \
        $$PWD/common/model/cproject.cpp

HEADERS += \
        $$PWD/mainwindow.h \
        $$PWD/ocl_macros.h \
        $$PWD/painterhread.h \
        $$PWD/common/utils/ocl_utils.h \
        $$PWD/common/utils/ocl_macros.h \
        $$PWD/common/items/coglpainterthread.h \
        $$PWD/common/items/citems.h \
        $$PWD/common/model/cstrokestemplate.h \
        $$PWD/common/items/coglpainterdata.h \
        $$PWD/common/model/cshape.h \
        $$PWD/common/model/cobject.h \
        $$PWD/common/model/cgraphics.h \
        $$PWD/common/model/clayer.h \
        $$PWD/common/model/cmodel.h \
        $$PWD/common/controllers/cprojecthandler.h \
        $$PWD/common/utils/vintervals.h \
        $$PWD/common/utils/utils.h \
        $$PWD/common/model/cmask.h \
        $$PWD/common/tools/ctools.h \
        $$PWD/common/tools/chandtool.h \
        $$PWD/common/tools/cselectiontool.h \
        $$PWD/common/tools/cframetool.h \
        $$PWD/common/model/cdocument.h \
        $$PWD/common/items/cdocumentitem.h \
        $$PWD/common/tools/ccompasstool.h \
        $$PWD/common/ui/toolbar.h \
        $$PWD/common/ui/compasspanel.h \
        $$PWD/common/ui/workspacewidget.h \
        $$PWD/common/ui/rullerdecorator.h \
        $$PWD/common/cenvironment.h \
        $$PWD/common/ui/welcomepage.h \
        $$PWD/common/ui/debugwidget.h \
        $$PWD/common/ui/objectstreemodel.h \
        $$PWD/common/ui/layersform.h \
        $$PWD/build_number.h \
        $$PWD/common/ui/historyform.h \
        $$PWD/common/tools/cshapetool.h \
        $$PWD/common/model/ccommands.h \
        $$PWD/common/items/coglstrokespainter.h \
        $$PWD/common/tools/cangletool.h \
        $$PWD/common/tools/cscenetool.h \
        $$PWD/common/tools/cintervaltool.h \
        $$PWD/common/tools/cbasrelieftool.h \
        $$PWD/common/tools/cthicknesstool.h \
        $$PWD/common/ui/thicknesspanel.h \
        $$PWD/common/ui/anglepanel.h \
        $$PWD/common/tools/cthresholdtool.h \
        $$PWD/common/ui/valuecontrol.h \
        $$PWD/common/tools/classotool.h \
        $$PWD/common/ui/paramspanelwidget.h \
        $$PWD/common/utils/fltransformframe.h \
        $$PWD/common/controls/ctransformcontrol.h \
        $$PWD/common/items/cshapeitem.h \
        $$PWD/common/items/crendermanager.h \
        $$PWD/common/items/coglstrokesrender.h \
        $$PWD/common/items/coglrenderthread.h \
        $$PWD/common/model/cgroup.h \
        $$PWD/common/items/cgroupitem.h \
        $$PWD/common/utils/cparserstrx.h \
        $$PWD/common/model/ccurve.h \
        $$PWD/common/model/cimage.h \
        $$PWD/common/cattributes.h \
        $$PWD/common/ui/glviewportwidget.h \
        $$PWD/common/ui/glgraphicsscene.h \
        $$PWD/common/ui/docparams.h \
        $$PWD/common/ui/newprojectdlg.h \
        $$PWD/common/controllers/citemscontroller.h \
        $$PWD/

        +common/model/cproject.h



FORMS += \
        mainwindow.ui \
        $$PWD/common/ui/toolbar.ui \
        $$PWD/common/ui/compasspanel.ui \
        $$PWD/common/ui/workspacewidget.ui \
        $$PWD/common/ui/welcomepage.ui \
        $$PWD/common/ui/debugwidget.ui \
        $$PWD/common/ui/layersform.ui \
        $$PWD/common/ui/historyform.ui \
        $$PWD/common/ui/valuecontrol.ui \
    common/ui/docparams.ui \
    common/ui/newprojectdlg.ui





RESOURCES += \
    resources.qrc \

INCLUDEPATH += ../gl/ \

INCLUDEPATH += $$PWD/common/ \
               $$PWD/common/utils/ \
               $$PWD/common/items/ \
               $$PWD/common/model/ \
               $$PWD/common/controllers/ \
               $$PWD/common/ui/ \
               $$PWD/common/tools/ \
               $$PWD/common/controls/ \
               $$PWD/graph/ \





INCLUDEPATH += /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/10.0.0/include/
QMAKE_CXXFLAGS +=-msse4.1
#QMAKE_CXXFLAGS +=-O2
