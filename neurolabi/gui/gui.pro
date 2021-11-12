
# #####################################################################
# Automatically generated by qmake (2.01a) Mon Dec 15 16:16:33 2008
# #####################################################################
TEMPLATE = app

contains(TEMPLATE, app) {
  DEFINES += _QT_APPLICATION_
} else {
  CONFIG += staticlib
}

CONFIG(neu3) {
  DEFINES += _NEU3_
}

CONFIG(neu3) | CONFIG(neutu) | CONFIG(flyem)  { #flyem CONFIG is an outdated option
  DEFINES *= _FLYEM_
} else {
  DEFINES += _NEUTUBE_
}

DEFINES *= _ENABLE_LOWTIS_
DEFINES += DRACO_ATTRIBUTE_DEDUPLICATION_SUPPORTED
#DEFINES += _GLIBCXX_USE_CXX11_ABI=0

#This may result in many 'Cannot create directory' warnings. Just ignore it.
CONFIG += object_parallel_to_source

#A way to remove 'empty directory' warnings
OBJECTS_DIR = buildqt create
MOC_DIR = build
UI_DIR = build

message("Objs dir: $${OBJECTS_DIR}")

#DEFINES+=_CLI_VERSION
win32 {
    QMAKE_CXXFLAGS += /bigobj #/GL # Enables whole program optimization.
    #QMAKE_LFLAGS += /LTCG # Link-time Code Generation

    DEFINES += _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN

    QMAKE_CXXFLAGS += /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
    QMAKE_CXXFLAGS += /wd4244 # 'argument' : conversion from 'type1' to 'type2', possible loss of data
    QMAKE_CXXFLAGS += /wd4305 # 'identifier' : truncation from 'type1' to 'type2'
    QMAKE_CXXFLAGS += /wd4819 # The file contains a character that cannot be represented in the current code page (number). Save the file in Unicode format to prevent data loss.
    QMAKE_CXXFLAGS += /utf-8  # https://blogs.msdn.microsoft.com/vcblog/2016/02/22/new-options-for-managing-character-sets-in-the-microsoft-cc-compiler/

    DEPLOYMENT_COMMAND = $$PWD/deploy_win.bat $(QMAKE) $$OUT_PWD

    CONFIG(release, debug|release):!isEmpty(DEPLOYMENT_COMMAND) {
        QMAKE_POST_LINK += $$DEPLOYMENT_COMMAND
    }
}

unix {
  DEFINES += _UNIX_
}

app_name = neutu

CONFIG(debug, debug|release) {
    app_name = neuTube_d
    CONFIG(neu3) {
        app_name = neu3_d
    } else {
      contains(DEFINES, _FLYEM_) {
        app_name = neutu_d
      }
    }
    DEFINES += _DEBUG_ _ADVANCED_ PROJECT_PATH=\"\\\"$$PWD\\\"\"
} else {
#    CONFIG(debug_info) {
#      QMAKE_CXXFLAGS += -g
#    }
    DEFINES += NDEBUG
    app_name = neuTube
    CONFIG(neu3) {
      app_name = neu3
    } else {
      CONFIG(flyem) | CONFIG(neutu) {
        app_name = neutu
      }
    }
}

include(extratarget.pri)

message("Neurolabi target: $$neurolabi.target")

CONFIG(force_link) {
  PRE_TARGETDEPS += neurolabi.PHONY
}

TARGET = $$app_name

QMAKE_POST_LINK += $$quote(echo "making config"; make app_config;)

unix {
  # suppress warnings from 3rd party library, works for gcc and clang
  QMAKE_CXXFLAGS += -isystem ../gui/ext
} else {
  INCLUDEPATH += ../gui/ext
}

CONFIG += rtti exceptions
CONFIG += static_gtest

include(extlib.pri)

QT += printsupport
CONFIG(WEBENGINE) {
  qtHaveModule(webenginewidgets) {
    QT += webenginewidgets
    DEFINES += _USE_WEBENGINE_
  }
}

DEFINES += _QT_GUI_USED_ HAVE_CONFIG_H _ENABLE_DDP_ _ENABLE_WAVG_

#Machine information
HOSTNAME = $$system(echo $HOSTNAME)
USER = $$system(echo $USER)
HOME = $$system(echo $HOME)
GIT = $$system(which git)

contains(GIT, .*git) {
  COMMIT_HASH = $$system("git log --pretty=format:\"%H %p\" -1 | sed s/' '/_/g")
  DEFINES += _CURRENT_COMMIT_=\"\\\"$$COMMIT_HASH\\\"\"
  message($$COMMIT_HASH)
}

message("Defines: $${DEFINES}")

include(add_itk.pri)

#Qt4 (Obsolete)
isEqual(QT_MAJOR_VERSION,4) {
  QT += opengl xml network
  warning("Obsolete setting: Qt 4")
}

#Qt5
isEqual(QT_MAJOR_VERSION,5) | greaterThan(QT_MAJOR_VERSION,5) {
    isEqual(QT_MAJOR_VERSION,5) {
      lessThan(QT_MINOR_VERSION,4) {
        message("Unstable Qt version $${QT_VERSION}.")
        error("Use at least Qt 5.4.0.")
      }
    }
    message("Qt 5")
    QT += concurrent gui widgets network xml
    DEFINES += _QT5_
    CONFIG *= strict_c++ c++11
}

equals(SANITIZE_BUILD, "thread") {
  QMAKE_CXXFLAGS += -fsanitize=thread
  QMAKE_LFLAGS += -fsanitize=thread
  DEFINES += SANITIZE_THREAD
}

equals(SANITIZE_BUILD, "address") {
  QMAKE_CXXFLAGS += -fsanitize=address
  QMAKE_LFLAGS += -fsanitize=address
}

unix {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated
    macx {
        QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy
        LIBS += -framework AppKit -framework IOKit \
            -framework ApplicationServices \
            -framework CoreFoundation

        CONFIG(neutu) | CONFIG(flyem) {
            ICON += images/neutu.icns
        } else {
            CONFIG(neu3) {
                ICON += images/neu3.icns
            } else {
                ICON = images/app.icns
            }
        }
#        QMAKE_INFO_PLIST = images/Info.plist
        QMAKE_CXXFLAGS += -m64

        OSX_VERSION = $$system(sw_vers -productVersion)
        message("Mac OS X $$OSX_VERSION")
        MAC_VERSION_NUMBER = $$split(OSX_VERSION, .)
        OSX_MAJOR_VERSION = $$member(MAC_VERSION_NUMBER, 0)
        OSX_MINOR_VERSION = $$member(MAC_VERSION_NUMBER, 1)
        CONFIG(autotarget) {
          isEqual(QT_MAJOR_VERSION, 5) {
            isEqual(QT_MINOR_VERSION, 12) {
              message("Forcing 10.12 SDK for Qt 5.12")
              QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
              QMAKE_MAC_SDK = macosx10.14
            }
          }
        } else {
          message("No auto mac version check")
          CONFIG(c++11) {
            isEqual(QT_MAJOR_VERSION,4) {
              message("Forcing deployment target: ")
              QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
            }
          }
        }

#        doc.files = doc
#        doc.path = Contents/MacOS
#        QMAKE_BUNDLE_DATA += doc
#        config.files = config.xml
#        config.path = Contents/MacOS
#        QMAKE_BUNDLE_DATA += config
    } else {
        DEFINES += _LINUX_
        LIBS += -lX11 -lm -lpthread -lrt -lGLU -lstdc++

        message(Checking arch...)
        contains(QMAKE_HOST.arch, x86_64) {
            message($$QMAKE_HOST.arch)
            QMAKE_CXXFLAGS += -m64
        }
        QMAKE_CXXFLAGS += -fext-numeric-literals -msse3
        contains(DEFINES, _FLYEM_) {
            RC_FILE = images/app2.icns
        } else {
            RC_FILE = images/app.icns
        }

    } #macx
}

win32 {
  RC_FILE = images/app.rc
}

include (gui_free.pri)
include(test/test.pri)
include(sandbox/sandbox.pri)
include(command/command.pri)
include(neutuse/neutuse.pri)
include(service/service.pri)
include(logging/logging.pri)
include(mvc/mvc.pri)
include(vis3d.pri)
include(vis2d/vis2d.pri)
include(qt/qt.pri)
include(widgets/widgets.pri)
include(flyem/flyem.pri)
include(tracing/tracing.pri)
include(dialogs/dialogs.pri)
include(movie/movie.pri)

# Input
win32 {
  SOURCES += $$PWD/ext/sys/VideoMemoryWin.cpp \
      $$PWD/ext/sys/VidMemViaD3D9.cpp \
      $$PWD/ext/sys/VidMemViaDDraw.cpp \
      $$PWD/ext/sys/VidMemViaDxDiag.cpp
}

unix {
  macx {
      SOURCES += $$PWD/ext/sys/VideoMemoryMac.cpp
  } else {
      SOURCES += $$PWD/ext/sys/VideoMemoryLinux.cpp
  }
}

RESOURCES = gui.qrc ext/QFontIcon/resource.qrc

HEADERS += mainwindow.h \
    concurrent/zworkerwrapper.h \
    dialogs/zparameterdialog.h \
    dvid/zdviddataslicehighrestask.h \
    dvid/zdvidglobal.h \
    dvid/zdvidstacksource.h \
  dvid/zdvidtargetbuilder.h \
    neuapp.h \
    protocols/protocolassignment.h \
    protocols/protocolassignmentdialog.h \
    protocols/protocolassignmentclient.h \
    protocols/protocolchooseassignmentdialog.h \
    protocols/protocolassignmenttask.h \
    protocols/protocolassignmenturl.h \
    dvid/zdvidtargetfactory.h \
    protocols/taskprotocolmocktask.h \
    zimage.h \
    zslider.h \
    plotsettings.h \
    plotter.h \
    zinteractivecontext.h \
    ztraceproject.h \
    zpunctum.h \
    zsinglechannelstack.h \
    zrandom.h \
    zlocsegchainconn.h \
    zobjsitem.h \
    zobjsmodel.h \
    zdirectionaltemplatechain.h \
    zcolormap.h \
    zclickablelabel.h \
    zcolormapeditor.h \
    zselectfilewidget.h \
    zcolormapwidgetwitheditorwindow.h \
    zspinbox.h \
    zcombobox.h \
    zspinboxwithslider.h \
    zeventlistenerparameter.h \
    zspanslider.h \
    ztest.h \
    zglmutils.h \
    znormcolormap.h \
    zlocsegchain.h \
    zcurve.h \
    zxmldoc.h \
    zintmap.h \
    z3dgraph.h \
    zprocessprogressbase.h \
    zopencv_header.h \
    zhdf5writer.h \
    zmoviescene.h \
    zmovieactor.h \
    zswcmovieactor.h \
    zmoviemaker.h \
    zmoviephotographer.h \
    zmoviescript.h \
    zpunctamovieactor.h \
    zstackmovieactor.h \
    zmoviesceneclipper.h\
    zmoviecamera.h \
    flyemdataform.h \
    zswcobjsmodel.h \
    zpunctaobjsmodel.h \
    zobjsmanagerwidget.h \
    zmoviescriptgenerator.h \
    zmoviestage.h \
    zqtbarprogressreporter.h \
    zstackdoccommand.h \
    zcursorstore.h \
    zmessagereporter.h \
    zqtmessagereporter.h \
    zstroke2d.h \
    neutube.h \
    zreportable.h \
    zswcnodeobjsmodel.h \
    zstackstatistics.h \
    biocytin/biocytin.h \
    biocytin/zstackprojector.h \
    zstackviewlocator.h \
    zstackreadthread.h \
    zswccolorscheme.h \
    zpunctumio.h \
    zstatisticsutils.h \
    zswcrangeanalyzer.h \
    zswcnodezrangeselector.h \
    zswcnodecompositeselector.h \
    zswcnodeellipsoidrangeselector.h \
    zswcfilelistmodel.h \
    zswcglobalfeatureanalyzer.h \
    zcommandline.h \
    zswclocationanalyzer.h \
    biocytin/zbiocytinfilenameparser.h \
    zerror.h \
    zhistogram.h \
    zswcgenerator.h \
    zpaintbundle.h \
    zflyemqueryview.h \
    zqslogmessagereporter.h \
    zactionactivator.h \
    zswccurvaturefeatureanalyzer.h \
    zstackdocmenustore.h \
    zstackdocmenufactory.h \
    zpainter.h \
    zmatlabprocess.h \
    zneuronseed.h \
    ztiledstackframe.h \
    ztilemanager.h \
    ztilemanagerview.h \
    ztilegraphicsitem.h \
    ztileinfo.h \
    zmultitaskmanager.h \
    zinteractionevent.h \
    zworkspacefactory.h \
    dvid/zdvidreader.h \
    zflyemdvidreader.h \
    zstroke2darray.h \
    tilemanager.h \
    zactionfactory.h \
    zstackreadfactory.h \
    zstackdoclabelstackfactory.h \
    zsparseobject.h \
    zlabelcolortable.h \
    zdocplayer.h \
    zlinesegmentobject.h \
    openvdb_header.h \
    zopenvdbobject.h \
    zstroke2dobjsmodel.h \
    zdocplayerobjsmodel.h \
    zabstractmodelpresenter.h \
    zstackobjectarray.h \
    zwindowfactory.h \
    dvid/zdvidwriter.h \
    zdialogfactory.h \
    zwidgetfactory.h \
    zlabelededitwidget.h \
    zlabeledcombowidget.h \
    zbuttonbox.h \
#    zkeyeventswcmapper.h \
    newprojectmainwindow.h \
    zmouseeventmapper.h \
    zframefactory.h \
    zactionbutton.h \
    dvid/zdvidbufferreader.h \
    zmouseevent.h \
    zmouseeventrecorder.h \
    zmouseeventprocessor.h \
    zstackoperator.h \
    zsleeper.h \
    dvid/libdvidheader.h \
    zthreadfuturemap.h \
    zstackball.h \
    zstackdochittest.h \
    zkeyeventmapper.h \
    zuserinputevent.h \
    zstackobjectmanager.h \
    zstackobjectgroup.h \
    zcolorscheme.h \
    zpunctumcolorscheme.h \
    zstackpatch.h \
    zrect2d.h \
    zobjectcolorscheme.h \
    zstackdvidgrayscalefactory.h \
    zstackdocreader.h \
    zsegmentationproject.h \
    zsegmentationprojectmodel.h \
    zlabeledspinboxwidget.h \
    zstackviewmanager.h \
    zstackviewparam.h \
    zflyemdataloader.h \
    zprogressmanager.h \
    dvid/zdvidrequest.h \
    dvid/zdviddataslicetaskfactory.h \
    dvid/zdviddataslicetask.h \
    dvid/zdvidtile.h \
    dvid/zdvidresolution.h \
    dvid/zdvidtileinfo.h \
    dvid/zdvidversionmodel.h \
    zmessagemanager.h \
    zmessageprocessor.h \
    zmessage.h \
    zmainwindowmessageprocessor.h \
    zstackdocloader.h \
    zstackwidget.h \
    zbodysplitbutton.h \
    zmessagefactory.h \
    zmessagemanagermodel.h \
    zflyemcontrolform.h \
    zpixmap.h \
    dvid/zdvidgrayslice.h \
    dvid/zdvidsparsestack.h \
    dvid/zdvidsparsevolslice.h \
    dvid/zdvidtileensemble.h \
    dvid/zdvidlabelslice.h \
    dvid/zdvidversiondag.h \
    dvid/zdvidversion.h \
    dvid/zdvidversionnode.h \
    dvid/zdvidannotationcommand.h \
    dvid/zflyembookmarkcommand.h \
    dvid/zdvidannotation.h \
    dvid/zdvidsynapse.h \
    dvid/zdvidsynapseensenmble.h \
    dvid/zdvidpath.h \
    dvid/zdvidpatchdatafetcher.h \
    dvid/zdvidpatchdataupdater.h \
    dvid/zdviddatafetcher.h \
    dvid/zdviddataupdater.h \
    dvid/zdvidsynapsecommand.h \
    dvid/zdvidannotation.hpp \
    dvid/zdvidgrayslicescrollstrategy.h \
    dvid/zdvidroi.h \
    dvid/zdvidgrayscale.h \
    dvid/zdvidneurontracer.h \
    dvid/zdvidresultservice.h \
    zwidgetmessage.h \
    zprogresssignal.h \
    zkeyeventstrokemapper.h \
    zkeyoperation.h \
    zkeyoperationmap.h \
    zkeyoperationconfig.h \
    ztextmessage.h \
    ztextmessagefactory.h \
    z3dgraphfactory.h \
    zkeyeventmapperfactory.h \
    zkeyoperationmapsequence.h \
    zpuncta.h \
    biocytin/zbiocytinprojectiondoc.h \
    zstackdocfactory.h \
    zintcuboidobj.h \
    ztask.h \
    zstackobjectpainter.h \
    zslicedpuncta.h \
    zmultiscalepixmap.h \
    biocytin/zbiocytinprojmaskfactory.h \
    zpunctumselector.h \
    zgraphobjsmodel.h \
    zsurfaceobjsmodel.h \
    zcubearray.h \
    zroiwidget.h \
    zstackdocselector.h \
    zactionlibrary.h \
    zmenufactory.h \
    zcrosshair.h \
    zapplication.h \
    zdvidutil.h \
    zstackreader.h \
    zcubearraymovieactor.h \
    z3dwindow.h \
    z3dmainwindow.h \
    zscrollslicestrategy.h \
    zviewproj.h \
    zorthoviewhelper.h \
    dvid/zdvidstore.h \
    zglobal.h \
    zstackgarbagecollector.h \
    neu3window.h \
    zstackdockeyprocessor.h \
    zutils.h \
    zflags.h \
    zbbox.h \
    zspinboxwithscrollbar.h \
    zsysteminfo.h \
    zvertexbufferobject.h \
    zvertexarrayobject.h \
    zmesh.h \
    zmeshio.h \
    zmeshutils.h \
    zstringutils.h \
    ztakescreenshotwidget.h \
    zioutils.h \
    zmeshobjsmodel.h \
    zopenglwidget.h \
    misc/zvtkutil.h \
    zstackdocaccessor.h \
    zcontrastprotocol.h \
    zmeshfactory.h \
    zstackdocptr.h \
    zstackdoc3dhelper.h \
    zstackobjectinfo.h \
    zstackobjectptr.h \
    zstackdocproxy.h \
    zroiobjsmodel.h \
    zstackobjectaccessor.h \
    zgraphptr.h \
    misc/zmarchingcube.h \
    ilastik/marching_cubes.h \
    ilastik/laplacian_smoothing.h \
    zarbsliceviewparam.h \
    zstackviewhelper.h \
    dvid/zdviddataslicehelper.h \
    zstackdocnullmenufactory.h \
    zmenuconfig.h \
    zobjsmodelmanager.h \
    zobjsmodelfactory.h \
    concurrent/zworkthread.h \
    concurrent/zworker.h \
    concurrent/ztaskqueue.h \
    dvid/zdvidbodyhelper.h \
    z3dwindowcontroller.h \
    zstackblockfactory.h \
    dvid/zdvidstackblockfactory.h \
    zstackblocksource.h \
    dvid/zdvidblockstream.h \
    imgproc/zstackmultiscalewatershed.h \
    protocols/protocolswitcher.h \
    protocols/protocolchooser.h \
    protocols/protocolmetadata.h \
    protocols/protocoldialog.h \
    protocols/doNthingsprotocol.h \
    protocols/synapsepredictionprotocol.h \
    protocols/synapsepredictioninputdialog.h \
    protocols/synapsepredictionbodyinputdialog.h \
    protocols/synapsereviewprotocol.h \
    protocols/synapsereviewinputdialog.h \
    protocols/connectionvalidationprotocol.h \
    protocols/todoreviewprotocol.h \
    protocols/todoreviewinputdialog.h \
    protocols/orphanlinkprotocol.h \
    protocols/todosearcher.h \
    protocols/taskbodymerge.h \
    protocols/taskprotocoltask.h \
    protocols/taskbodyreview.h \
    protocols/tasktesttask.h \
    protocols/tasksplitseeds.h \
    protocols/bodyprefetchqueue.h \
    protocols/taskbodyhistory.h \
    protocols/taskbodycleave.h \
    protocols/protocoltaskfactory.h \
    protocols/protocoltaskconfig.h \
    protocols/taskfalsesplitreview.h \
    protocols/taskprotocoltaskfactory.h \
    protocols/taskmergereview.h \
    main.h \
    dvid/zdvidgraysliceensemble.h \
    dvid/zdvidenv.h \
    protocols/taskutils.h

FORMS += mainwindow.ui \
    flyemdataform.ui \
    tilemanager.ui \
    newprojectmainwindow.ui \
    zflyemcontrolform.ui \
    protocols/protocolassignmentdialog.ui \
    protocols/protocolchooseassignmentdialog.ui \
    protocols/protocolchooser.ui \
    protocols/doNthingsprotocol.ui \
    protocols/synapsepredictionprotocol.ui \
    protocols/synapsepredictioninputdialog.ui \
    protocols/synapsepredictionbodyinputdialog.ui \
    protocols/synapsereviewprotocol.ui \
    protocols/synapsereviewinputdialog.ui \
    protocols/connectionvalidationprotocol.ui \
    protocols/todoreviewprotocol.ui \
    protocols/todoreviewinputdialog.ui \
    protocols/orphanlinkprotocol.ui \
    protocols/protocoldialog.ui \
    neu3window.ui

SOURCES += main.cpp \
    concurrent/zworkerwrapper.cpp \
    dialogs/zparameterdialog.cpp \
    dvid/zdviddataslicehighrestask.cpp \
    dvid/zdvidglobal.cpp \
    dvid/zdvidstacksource.cpp \
    dvid/zdvidtargetbuilder.cpp \
    dvid/zdvidtargetfactory.cpp \
    mainwindow.cpp \
    neuapp.cpp \
    zimage.cpp \
    zslider.cpp \
    plotsettings.cpp \
    plotter.cpp \
    zinteractivecontext.cpp \
    ztraceproject.cpp \
    zpunctum.cpp \
    zdirectionaltemplatechain.cpp \
    zrandom.cpp \
    z3dwindow.cpp \
    zobjsitem.cpp \
    zobjsmodel.cpp \
    zcolormap.cpp \
    zclickablelabel.cpp \
    zcolormapeditor.cpp \
    zlocsegchainconn.cpp \
    zlocsegchain.cpp \
    zcurve.cpp \
    zselectfilewidget.cpp \
    zcolormapwidgetwitheditorwindow.cpp \
    zspinbox.cpp \
    zcombobox.cpp \
    zspinboxwithslider.cpp \
    zeventlistenerparameter.cpp \
    zspanslider.cpp \
    ztest.cpp \
    znormcolormap.cpp \
    z3dgraph.cpp \
    zprocessprogressbase.cpp \
    zmoviescene.cpp \
    zmovieactor.cpp \
    zswcmovieactor.cpp \
    zmoviemaker.cpp \
    zmoviephotographer.cpp \
    zmoviescript.cpp \
    zpunctamovieactor.cpp \
    zstackmovieactor.cpp \
    zmoviesceneclipper.cpp \
    zmoviecamera.cpp \
    flyemdataform.cpp \
    zswcobjsmodel.cpp \
    zpunctaobjsmodel.cpp \
    zobjsmanagerwidget.cpp \
    zmoviescriptgenerator.cpp \
    zmoviestage.cpp \
    zqtbarprogressreporter.cpp \
    zstackdoccommand.cpp \
    zcursorstore.cpp \
    zqtmessagereporter.cpp \
    zstroke2d.cpp \
    zswcnodeobjsmodel.cpp \
    biocytin/biocytin.cpp \
    zstackviewlocator.cpp \
    zstackreadthread.cpp \
    zswccolorscheme.cpp \
    zpunctumio.cpp \
    zswcfilelistmodel.cpp \
    zcommandline.cpp \
    neutube.cpp \
    zpaintbundle.cpp \
    zflyemqueryview.cpp \
    zqslogmessagereporter.cpp \
    zactionactivator.cpp \
    zswccurvaturefeatureanalyzer.cpp \
    zstackdocmenustore.cpp \
    zstackdocmenufactory.cpp \
    zpainter.cpp \
    dvid/zdvidrequest.cpp \
    zmatlabprocess.cpp \
    zneuronseed.cpp \
    ztiledstackframe.cpp \
    ztilemanager.cpp \
    ztilemanagerview.cpp \
    ztilegraphicsitem.cpp \
    ztileinfo.cpp \
    zmultitaskmanager.cpp \
    zinteractionevent.cpp \
    zworkspacefactory.cpp \
    dvid/zdvidreader.cpp \
    zflyemdvidreader.cpp \
    zstroke2darray.cpp \
    tilemanager.cpp \
    zactionfactory.cpp \
    zstackreadfactory.cpp \
    zstackdoclabelstackfactory.cpp \
    zsparseobject.cpp \
    zlabelcolortable.cpp \
    zdocplayer.cpp \
    zlinesegmentobject.cpp \
    zopenvdbobject.cpp \
    zstroke2dobjsmodel.cpp \
    zdocplayerobjsmodel.cpp \
    zabstractmodelpresenter.cpp \
    zstackobjectarray.cpp \
    zwindowfactory.cpp \
    dvid/zdvidwriter.cpp \
    zdialogfactory.cpp \
    zwidgetfactory.cpp \
    zlabelededitwidget.cpp \
    zlabeledcombowidget.cpp \
    zbuttonbox.cpp \
    newprojectmainwindow.cpp \
    zmouseeventmapper.cpp \
    zframefactory.cpp \
    zactionbutton.cpp \
    dvid/zdvidbufferreader.cpp \
    zmouseevent.cpp \
    zmouseeventrecorder.cpp \
    zmouseeventprocessor.cpp \
    zstackoperator.cpp \
    zsleeper.cpp \
    z3dwindowfactory.cpp \
    zthreadfuturemap.cpp \
    zstackball.cpp \
    zstackdochittest.cpp \
    zkeyeventmapper.cpp \
    zuserinputevent.cpp \
    zstackobjectmanager.cpp \
    zstackobjectgroup.cpp \
    zcolorscheme.cpp \
    zpunctumcolorscheme.cpp \
    zstackpatch.cpp \
    zrect2d.cpp \
    zobjectcolorscheme.cpp \
    zstackdvidgrayscalefactory.cpp \
    zstackdocreader.cpp \
    zsegmentationproject.cpp \
    zsegmentationprojectmodel.cpp \
    zlabeledspinboxwidget.cpp \
    zstackviewmanager.cpp \
    zstackviewparam.cpp \
    zflyemdataloader.cpp \
    zprogressmanager.cpp \
    zmessagemanager.cpp \
    zmessageprocessor.cpp \
    zmessage.cpp \
    zmainwindowmessageprocessor.cpp \
    zstackdocloader.cpp \
    zstackwidget.cpp \
    zbodysplitbutton.cpp \
    zmessagefactory.cpp \
    zmessagemanagermodel.cpp \
    zflyemcontrolform.cpp \
    zpixmap.cpp \
    dvid/zdvidsynapsecommand.cpp \
    dvid/zdvidannotationcommand.cpp \
    dvid/zflyembookmarkcommand.cpp \
    dvid/zdvidannotation.cpp \
    dvid/zdvidsynapse.cpp \
    dvid/zdvidsynapseensenmble.cpp \
    dvid/zdvidbodyhelper.cpp \
    dvid/zdviddataslicetaskfactory.cpp \
    dvid/zdviddataslicetask.cpp \
    dvid/zdvidstackblockfactory.cpp \
    dvid/zdvidblockstream.cpp \
    dvid/zdvidgraysliceensemble.cpp \
    dvid/zdvidenv.cpp \
    dvid/zdvidpath.cpp \
    dvid/zdvidtile.cpp \
    dvid/zdvidresolution.cpp \
    dvid/zdvidtileinfo.cpp \
    dvid/zdvidversionmodel.cpp \
    dvid/zdvidversiondag.cpp \
    dvid/zdvidversion.cpp \
    dvid/zdvidversionnode.cpp \
    dvid/zdvidtileensemble.cpp \
    dvid/zdvidlabelslice.cpp \
    dvid/zdvidgrayslice.cpp \
    dvid/zdvidsparsestack.cpp \
    zwidgetmessage.cpp \
    zprogresssignal.cpp \
    zkeyeventstrokemapper.cpp \
    zkeyoperation.cpp \
    zkeyoperationmap.cpp \
    zkeyoperationconfig.cpp \
    dvid/zdvidsparsevolslice.cpp \
    ztextmessage.cpp \
    ztextmessagefactory.cpp \
    z3dgraphfactory.cpp \
    zkeyeventmapperfactory.cpp \
    zkeyoperationmapsequence.cpp \
    zpuncta.cpp \
    biocytin/zbiocytinprojectiondoc.cpp \
    zstackdocfactory.cpp \
    zintcuboidobj.cpp \
    ztask.cpp \
    zstackobjectpainter.cpp \
    zslicedpuncta.cpp \
    zmultiscalepixmap.cpp \
    biocytin/zbiocytinprojmaskfactory.cpp \
    zpunctumselector.cpp \
    zgraphobjsmodel.cpp \
    zsurfaceobjsmodel.cpp \
    zcubearray.cpp \
    zroiwidget.cpp \
    zstackdocselector.cpp \
    protocols/protocolswitcher.cpp \
    protocols/protocolchooser.cpp \
    protocols/protocolmetadata.cpp \
    protocols/protocoldialog.cpp \
    protocols/doNthingsprotocol.cpp \
    protocols/synapsepredictionprotocol.cpp \
    protocols/synapsepredictioninputdialog.cpp \
    protocols/synapsepredictionbodyinputdialog.cpp \
    protocols/synapsereviewprotocol.cpp \
    protocols/synapsereviewinputdialog.cpp \
    protocols/connectionvalidationprotocol.cpp \
    protocols/todoreviewprotocol.cpp \
    protocols/todoreviewinputdialog.cpp \
    protocols/orphanlinkprotocol.cpp \
    protocols/todosearcher.cpp \
    zactionlibrary.cpp \
    zmenufactory.cpp \
    zcrosshair.cpp \
    zapplication.cpp \
    zdvidutil.cpp \
    dvid/zdvidpatchdatafetcher.cpp \
    dvid/zdvidpatchdataupdater.cpp \
    dvid/zdviddatafetcher.cpp \
    dvid/zdviddataupdater.cpp \
    zcubearraymovieactor.cpp \
    dvid/zdvidroi.cpp \
    z3dmainwindow.cpp \
    dvid/zdvidgrayscale.cpp \
    zscrollslicestrategy.cpp \
    dvid/zdvidgrayslicescrollstrategy.cpp \
    zviewproj.cpp \
    dvid/zdvidneurontracer.cpp \
    zorthoviewhelper.cpp \
    dvid/zdvidstore.cpp \
    zglobal.cpp \
    dvid/zdvidresultservice.cpp \
    zstackgarbagecollector.cpp \
    zxmldoc.cpp \
    neu3window.cpp \
    zstackdockeyprocessor.cpp \
    zspinboxwithscrollbar.cpp \
    zsysteminfo.cpp \
    zvertexbufferobject.cpp \
    zvertexarrayobject.cpp \
    zmesh.cpp \
    zmeshio.cpp \
    zmeshutils.cpp \
    zstringutils.cpp \
    ztakescreenshotwidget.cpp \
    zioutils.cpp \
    zmeshobjsmodel.cpp \
    zopenglwidget.cpp \
    zstackreader.cpp \
    misc/zvtkutil.cpp \
    zstackdocaccessor.cpp \
    zcontrastprotocol.cpp \
    zmeshfactory.cpp \
    zstackdoc3dhelper.cpp \
    zstackobjectinfo.cpp \
    zstackdocproxy.cpp \
    zroiobjsmodel.cpp \
    zstackobjectaccessor.cpp \
    zgraphptr.cpp \
    misc/zmarchingcube.cpp \
    ilastik/marching_cubes.cpp \
    ilastik/laplacian_smoothing.cpp \
    zarbsliceviewparam.cpp \
    zstackviewhelper.cpp \
    dvid/zdviddataslicehelper.cpp \
    zstackdocnullmenufactory.cpp \
    zmenuconfig.cpp \
    zobjsmodelmanager.cpp \
    zobjsmodelfactory.cpp \
    concurrent/zworkthread.cpp \
    concurrent/zworker.cpp \
    concurrent/ztaskqueue.cpp \
    z3dwindowcontroller.cpp \
    zstackblockfactory.cpp \    
    zstackblocksource.cpp \
    protocols/protocolassignment.cpp \
    protocols/protocolassignmentdialog.cpp \
    protocols/protocolassignmentclient.cpp \
    protocols/protocolassignmenttask.cpp \
    protocols/protocolassignmenturl.cpp \
    protocols/protocolchooseassignmentdialog.cpp \
    protocols/taskprotocolmocktask.cpp \
    protocols/protocoltaskfactory.cpp \
    protocols/protocoltaskconfig.cpp \
    protocols/taskfalsesplitreview.cpp \
    protocols/taskprotocoltaskfactory.cpp \
    protocols/taskmergereview.cpp \
    protocols/taskutils.cpp \
    protocols/taskprotocoltask.cpp \
    protocols/taskbodyreview.cpp \
    protocols/tasktesttask.cpp \
    protocols/tasksplitseeds.cpp \
    protocols/bodyprefetchqueue.cpp \
    protocols/taskbodyhistory.cpp \
    protocols/taskbodycleave.cpp \
    protocols/taskbodymerge.cpp \
    imgproc/zstackmultiscalewatershed.cpp

DISTFILES += \
    Resources/shader/wblended_final.frag \
    Resources/shader/wblended_init.frag

#debugging
message("[[ DEFINE ]]: $${DEFINES}")
message("[[ QMAKE_CXXFLAGS ]]: $${QMAKE_CXXFLAGS}")
message("[[ CONDA_ENV ]]: $${CONDA_ENV}")
message("[[ LIBS ]]: $${LIBS}")
message("[[ TARGET ]]: $$app_name")
message("[[ OUT_PWD ]]: $${OUT_PWD}")
macx {
  message("[[ Mac Target ]]: $$QMAKE_MACOSX_DEPLOYMENT_TARGET")
}
