#include "zflyembodysplitproject.h"

#include <QProcess>
#include <QByteArray>
#include <QtConcurrentRun>
#include <QWidget>
#include <QUndoStack>
#include <QMutexLocker>

#include "zstackframe.h"
#include "z3dwindow.h"
#include "zstackdoclabelstackfactory.h"
#include "zstackobject.h"
#include "zstackball.h"
#include "zsparsestack.h"
#include "z3dvolumesource.h"
#include "zswctree.h"
#include "zwindowfactory.h"
#include "dvid/zdvidreader.h"
#include "zwindowfactory.h"
#include "zstackskeletonizer.h"
#include "neutubeconfig.h"
#include "zswcgenerator.h"
#include "z3dswcfilter.h"
#include "zobject3dscan.h"
#include "zstroke2d.h"
#include "dvid/zdvidwriter.h"
#include "dvid/zdviddata.h"
#include "zstring.h"
#include "zflyemcoordinateconverter.h"
#include "flyem/zflyemneuron.h"
#include "zstackview.h"
#include "zstackpatch.h"
#include "zstackobjectsource.h"
#include "neutubeconfig.h"
#include "zarray.h"
#include "dvid/zdvidgrayslice.h"
#include "zstackobjectsourcefactory.h"
#include "zflyemproofdoc.h"
#include "dvid/zdvidsparsestack.h"
#include "zwidgetmessage.h"
#include "zflyemmisc.h"
#include "zstackdochelper.h"

ZFlyEmBodySplitProject::ZFlyEmBodySplitProject(QObject *parent) :
  QObject(parent), m_bodyId(0), m_dataFrame(NULL),
  m_resultWindow(NULL), m_quickResultWindow(NULL),
  m_quickViewWindow(NULL), m_bookmarkArray(NULL),
  m_isBookmarkVisible(true), m_showingBodyMask(false)
{
  m_progressSignal = new ZProgressSignal(this);

  connect(this, SIGNAL(bodyQuickViewReady()), this, SLOT(startBodyQuickView()));
  connect(this, SIGNAL(result3dQuickViewReady()),
          this, SLOT(startResultQuickView()));
  connect(this, SIGNAL(rasingBodyQuickView()), this, SLOT(raiseBodyQuickView()));
  connect(this, SIGNAL(rasingResultQuickView()), this, SLOT(raiseResultQuickView()));
}

ZFlyEmBodySplitProject::~ZFlyEmBodySplitProject()
{
  clear();
}

void ZFlyEmBodySplitProject::clear(QWidget *widget)
{
  if (widget != NULL) {
    widget->hide();
    delete widget;
    widget = NULL;
  }
}

void ZFlyEmBodySplitProject::clear()
{
  if (m_resultWindow != NULL) {
    m_resultWindow->hide();
    delete m_resultWindow;
    m_resultWindow = NULL;
  }

  clear(m_quickResultWindow);

  if (m_dataFrame != NULL) {
//    m_dataFrame->close3DWindow();
    m_dataFrame->hide();
    delete m_dataFrame;
    m_dataFrame = NULL;
  }

  if (m_quickViewWindow != NULL) {
    m_quickViewWindow->hide();
    delete m_quickViewWindow;
    m_quickViewWindow = NULL;
  }

  shallowClear();
}

void ZFlyEmBodySplitProject::shallowClearDataFrame()
{
  if (m_resultWindow != NULL) {
    m_resultWindow->hide();
    delete m_resultWindow;
    m_resultWindow = NULL;
  }

  clear(m_quickResultWindow);

  if (m_quickViewWindow != NULL) {
    m_quickViewWindow->hide();
    delete m_quickViewWindow;
    m_quickViewWindow = NULL;
  }

  shallowClear();
}

void ZFlyEmBodySplitProject::shallowClear()
{
  m_resultWindow = NULL;
  m_quickResultWindow = NULL;
  m_quickViewWindow = NULL;
  m_dataFrame = NULL;

  m_bodyId = 0;

  m_bookmarkDecoration.clear();
}

void ZFlyEmBodySplitProject::shallowClearResultWindow()
{
  m_resultWindow = NULL;
}

void ZFlyEmBodySplitProject::shallowClearQuickResultWindow()
{
  m_quickResultWindow = NULL;
}

void ZFlyEmBodySplitProject::shallowClearQuickViewWindow()
{
  m_quickViewWindow = NULL;
}

/*
void ZFlyEmBodySplitProject::shallowClearBodyWindow()
{
  m_bodyWindow = NULL;
}
*/
void ZFlyEmBodySplitProject::setDvidTarget(const ZDvidTarget &target)
{
  m_dvidTarget = target;
  ZDvidReader reader;
  if (reader.open(target)) {
    m_dvidInfo = reader.readGrayScaleInfo();
  }
}

void ZFlyEmBodySplitProject::showDataFrame() const
{
  if (m_dataFrame != NULL) {
    m_dataFrame->show();
    m_dataFrame->raise();
    m_dataFrame->activateWindow();
  }
}

ZProgressSignal* ZFlyEmBodySplitProject::getProgressSignal() const
{
  return m_progressSignal;
}

void ZFlyEmBodySplitProject::showDataFrame3d()
{
  if (getDocument() != NULL) {
    Z3DWindow *window = getDocument()->getParent3DWindow();

    if (window == NULL) {
      /*
      if (getMainWindow() != NULL) {
        getMainWindow()->startProgress("Opening 3D View ...", 0);
      }
      */

      ZWindowFactory factory;
      //factory.setParentWidget(parent);
      window = factory.make3DWindow(getSharedDocument(), Z3DWindow::NORMAL_INIT);
      window->setWindowTitle(getDocument()->getTitle());

      //getDocument()->registerUser(window);

      if (getDocument()->getParentFrame() != NULL) {
        connect(getDocument()->getParentFrame(), SIGNAL(closed(ZStackFrame*)),
                window, SLOT(close()));
      }

      /*
      if (getMainWindow() != NULL) {
        getMainWindow()->endProgress();
      }
      */
    }

    if (window != NULL) {
      window->show();
      window->raise();
    } else {
      emitError("3D functions are disabled");
//      emit messageGenerated("3D functions are disabled");
    }

  }

#if 0
  if (m_dataFrame != NULL) {
    if (m_bodyWindow == NULL) {
    //emit messageGenerated("Showing data in 3D ...");
      m_bodyWindow = m_dataFrame->open3DWindow();

    } else {
      m_bodyWindow->show();
      m_bodyWindow->raise();
    }
    //emit messageGenerated("Done.");
  }
#endif
}

ZObject3dScan* ZFlyEmBodySplitProject::readBody(ZObject3dScan *out) const
{
  if (out != NULL) {
    out->clear();
  }
  ZDvidReader reader;
  if (reader.open(getDvidTarget())) {
    if (out == NULL) {
      out = new ZObject3dScan;
    }

    reader.readBody(getBodyId(), out);
  }

  return out;
}

void ZFlyEmBodySplitProject::quickViewFunc()
{
  if (m_quickViewWindow != NULL) {
    getProgressSignal()->startProgress("Showing quick view ...");
    emitMessage("Generating quick view ...");

    ZStackDoc *doc = m_quickViewWindow->getDocument();

    ZObject3dScan obj;
    if (getDocument() != NULL) {
      const ZObject3dScan *objMask = NULL;
      const ZStackDoc *doc = getDocument();
      if (doc->hasSparseStack()) {
        objMask = doc->getConstSparseStack()->getObjectMask();
      } else {
        if (getDocument<ZFlyEmProofDoc>() != NULL) {
          objMask = getDocument<ZFlyEmProofDoc>()->getDvidSparseStack()->getObjectMask();
        }
      }
      if (objMask != NULL) {
        obj = *objMask;
      }
    }

    getProgressSignal()->advanceProgress(0.1);

    ZDvidReader reader;
    if (reader.open(getDvidTarget())) {
      if (obj.isEmpty()) {
        int bodyId = getBodyId();
        obj = reader.readBody(bodyId);
        if (!obj.isEmpty()) {
          obj.canonize();
        }
      }
    }

    getProgressSignal()->advanceProgress(0.1);

    ZSwcTree *tree = ZSwcGenerator::createSurfaceSwc(obj, 2);
    doc->addObject(tree);
    getProgressSignal()->advanceProgress(0.3);

    m_quickViewWindow->getSwcFilter()->setRenderingPrimitive("Sphere");

    connect(m_quickViewWindow, SIGNAL(destroyed()),
            this, SLOT(shallowClearQuickViewWindow()));
    if (m_dataFrame != NULL) {
      connect(m_quickViewWindow, SIGNAL(locating2DViewTriggered(ZStackViewParam)),
              m_dataFrame, SLOT(setView(ZStackViewParam)));
    }
    connect(m_quickViewWindow, SIGNAL(locating2DViewTriggered(ZStackViewParam)),
            this, SIGNAL(locating2DViewTriggered(ZStackViewParam)));

    if (m_dvidInfo.isValid()) {
      //      ZDvidInfo dvidInfo = reader.readGrayScaleInfo();
      doc->addObject(ZFlyEmMisc::MakeBoundBoxGraph(m_dvidInfo), true);
      doc->addObject(ZFlyEmMisc::MakePlaneGraph(getDocument(), m_dvidInfo), true);
      //ZFlyEmMisc::Decorate3DWindow(m_quickViewWindow, reader);
    }

    m_quickViewWindow->setYZView();
    getProgressSignal()->advanceProgress(0.1);

    ZIntCuboid box = obj.getBoundBox();
    m_quickViewWindow->notifyUser(
          QString("Size: %1; Bounding box: %2 x %3 x %4").
          arg(obj.getVoxelNumber()).arg(box.getWidth()).arg(box.getHeight()).
          arg(box.getDepth()));

    emitMessage("Done.");

    emit bodyQuickViewReady();
    getProgressSignal()->endProgress();
    emit rasingBodyQuickView();
  }
}

void ZFlyEmBodySplitProject::raiseBodyQuickView()
{
  if (m_quickViewWindow != NULL) {
    m_quickViewWindow->raise();
  }
}

void ZFlyEmBodySplitProject::raiseResultQuickView()
{
  if (m_quickResultWindow != NULL) {
    m_quickResultWindow->raise();
  }
}

void ZFlyEmBodySplitProject::showQuickView(Z3DWindow *window)
{
  if (window != NULL) {
    window->show();
    window->raise();
  }
}

/*
void ZFlyEmBodySplitProject::showBodyQuickView()
{
  showQuickView(m_quickViewWindow);
}

void ZFlyEmBodySplitProject::showResultQuickView()
{
  showQuickView(m_quickResultWindow);
}
*/
void ZFlyEmBodySplitProject::startQuickView(Z3DWindow *window)
{
  if (window != NULL) {
    std::cout << "Starting quick view ..." << std::endl;
    window->setYZView();
    std::cout << "Estimating body bound box ..." << std::endl;
    const TStackObjectList &objList =
        window->getDocument()->getObjectList(ZStackObject::TYPE_SWC);

    ZCuboid boundBox;
    for (TStackObjectList::const_iterator iter = objList.begin();
         iter != objList.end(); ++iter) {
      ZSwcTree *tree = dynamic_cast<ZSwcTree*>(*iter);
      if (tree != NULL) {
        if (boundBox.isValid()) {
          boundBox.bind(tree->getBoundBox());
        } else {
          boundBox = tree->getBoundBox();
        }
      }
    }

    std::cout << "Zooming in" << std::endl;
    window->gotoPosition(boundBox.toCornerVector(), 0);
//    m_quickViewWindow->setYZView();

    std::cout << "Showing quick view ..." << std::endl;
    showQuickView(window);
  }
}

void ZFlyEmBodySplitProject::startResultQuickView()
{
  startQuickView(m_quickResultWindow);
}

void ZFlyEmBodySplitProject::startBodyQuickView()
{
  startQuickView(m_quickViewWindow);
#if 0
  if (m_quickViewWindow != NULL) {
    m_quickViewWindow->setYZView();
    const TStackObjectList &objList =
        m_quickViewWindow->getDocument()->getObjectList(ZStackObject::TYPE_SWC);

    ZCuboid boundBox;
    for (TStackObjectList::const_iterator iter = objList.begin();
         iter != objList.end(); ++iter) {
      ZSwcTree *tree = dynamic_cast<ZSwcTree*>(*iter);
      if (tree != NULL) {
        if (boundBox.isValid()) {
          boundBox.bind(tree->getBoundBox());
        } else {
          boundBox = tree->getBoundBox();
        }
      }
    }
    m_quickViewWindow->gotoPosition(boundBox.toCornerVector(), 0);
//    m_quickViewWindow->setYZView();
    showBodyQuickView();
  }
#endif
}

void ZFlyEmBodySplitProject::showBodyQuickView()
{
  if (m_quickViewWindow == NULL) {
    ZWindowFactory factory;
    factory.setWindowTitle("Quick View");

    ZStackDoc *doc = new ZStackDoc(NULL, NULL);
    doc->setTag(NeuTube::Document::FLYEM_BODY_DISPLAY);
    m_quickViewWindow = factory.make3DWindow(doc);

    const QString threadId = "quickViewFunc";
    if (!m_futureMap.isAlive(threadId)) {
      m_futureMap.removeDeadThread();
      QFuture<void> future =
          QtConcurrent::run(this, &ZFlyEmBodySplitProject::quickViewFunc);
      m_futureMap[threadId] = future;
    }
  } else {
//    showBodyQuickView();
    showQuickView(m_quickViewWindow);
  }
}

void ZFlyEmBodySplitProject::showSkeleton(ZSwcTree *tree)
{
  if (tree != NULL) {
    if (m_quickViewWindow == NULL) {
      ZWindowFactory factory;
      factory.setWindowTitle("Quick View");
      ZStackDoc *doc = new ZStackDoc(NULL, NULL);
      doc->addObject(tree);
      m_quickViewWindow = factory.make3DWindow(doc);
      connect(m_quickViewWindow, SIGNAL(destroyed()),
              this, SLOT(shallowClearQuickViewWindow()));
    } else {
      m_quickViewWindow->getDocument()->beginObjectModifiedMode(
            ZStackDoc::OBJECT_MODIFIED_CACHE);
      m_quickViewWindow->getDocument()->removeAllSwcTree();
      m_quickViewWindow->getDocument()->addObject(tree);
      m_quickViewWindow->getDocument()->endObjectModifiedMode();
      m_quickViewWindow->getDocument()->notifyObjectModified();
    }

    m_quickViewWindow->show();
    m_quickViewWindow->raise();
  }
}

void ZFlyEmBodySplitProject::loadResult3dQuick(ZStackDoc *doc)
{
  if (doc != NULL && getDocument() != NULL) {
    doc->beginObjectModifiedMode(ZStackDoc::OBJECT_MODIFIED_CACHE);
    doc->removeAllSwcTree();
    TStackObjectList objList =
        getDocument()->getObjectList(ZStackObject::TYPE_OBJECT3D_SCAN);
    const int maxSwcNodeNumber = 100000;
    const int maxScale = 50;
    const int minScale = 1;
    getProgressSignal()->advanceProgress(0.1);

    double dp = 0.0;
    if (!objList.isEmpty()) {
      dp =  0.9 / objList.size();
    }
    for (TStackObjectList::const_iterator iter = objList.begin();
         iter != objList.end(); ++iter) {
      ZObject3dScan *splitObj = dynamic_cast<ZObject3dScan*>(*iter);
      if (splitObj != NULL) {
        if (splitObj->hasRole(ZStackObjectRole::ROLE_TMP_RESULT)) {
          ZObject3d *obj = splitObj->toObject3d();
          if (obj != NULL) {
            int ds = obj->size() / maxSwcNodeNumber + 1;
            if (ds < minScale) {
              ds = minScale;
            }
            if (ds > maxScale) {
              ds = maxScale;
            }

            ZSwcTree *tree = ZSwcGenerator::createSwc(
                  *obj, dmin2(5.0, ds / 2.0), ds);
            if (tree != NULL) {
              tree->setAlpha(255);
              doc->addObject(tree);
            }
            delete obj;
          }
        }
      }
      getProgressSignal()->advanceProgress(dp);
    }
    doc->endObjectModifiedMode();
    doc->notifyObjectModified();
  }
}

void ZFlyEmBodySplitProject::updateResult3dQuickFunc()
{
  QMutexLocker locker(&m_splitWindowMutex);

  if (m_quickResultWindow != NULL) {
    ZStackDoc *doc = m_quickResultWindow->getDocument();
    bool resetCamera = true;
    if (doc->hasSwc()) {
      resetCamera = false;
    }

    loadResult3dQuick(doc);
    if (resetCamera) {
      m_quickResultWindow->resetCamera();
    }

    emit messageGenerated(ZWidgetMessage("3D split view updated."));
  }
}

void ZFlyEmBodySplitProject::updateResult3dQuick()
{
  emit messageGenerated(ZWidgetMessage("Updating 3D split view ..."));
  updateResult3dQuickFunc();
//  QtConcurrent::run(this, &ZFlyEmBodySplitProject::updateResult3dQuickFunc);
}

void ZFlyEmBodySplitProject::result3dQuickFunc()
{
  ZStackDoc *mainDoc = getDocument();

  if (mainDoc != NULL) {
    if (m_quickResultWindow != NULL) {
      getProgressSignal()->startProgress("Showing result quick view ...");

      ZStackDoc *doc = m_quickResultWindow->getDocument();

      getProgressSignal()->advanceProgress(0.1);

      getProgressSignal()->startProgress(0.5);
      loadResult3dQuick(doc);
      getProgressSignal()->endProgress();

      m_quickResultWindow->getSwcFilter()->setColorMode("Intrinsic");
      m_quickResultWindow->getSwcFilter()->setRenderingPrimitive("Sphere");
      m_quickResultWindow->setYZView();

      ZDvidReader reader;
      if (reader.open(getDvidTarget())) {
//        ZDvidInfo dvidInfo = reader.readGrayScaleInfo();
        doc->addObject(ZFlyEmMisc::MakeBoundBoxGraph(m_dvidInfo), true);
        doc->addObject(ZFlyEmMisc::MakePlaneGraph(getDocument(), m_dvidInfo), true);
      }

      connect(m_quickResultWindow, SIGNAL(destroyed()),
              this, SLOT(shallowClearQuickResultWindow()));
      connect(mainDoc, SIGNAL(labelFieldModified()),
              this, SLOT(updateResult3dQuick()));
      if (m_dataFrame != NULL) {
        connect(m_quickResultWindow, SIGNAL(locating2DViewTriggered(ZStackViewParam)),
                m_dataFrame, SLOT(setView(ZStackViewParam)));
      }
      connect(m_quickResultWindow, SIGNAL(locating2DViewTriggered(ZStackViewParam)),
              this, SIGNAL(locating2DViewTriggered(ZStackViewParam)));

      emit result3dQuickViewReady();
      getProgressSignal()->endProgress();

      emit rasingResultQuickView();
    }
  }
}

void ZFlyEmBodySplitProject::showResultQuickView()
{
  ZStackDoc *mainDoc = getDocument();

  if (mainDoc != NULL) {
    if (m_quickResultWindow == NULL) {
      ZWindowFactory windowFactory;
      windowFactory.setWindowTitle("Splitting Result");
      ZStackDoc *doc = new ZStackDoc(NULL, NULL);
      doc->setTag(NeuTube::Document::FLYEM_BODY_DISPLAY);
      m_quickResultWindow = windowFactory.make3DWindow(doc);

      const QString threadId = "result3dQuickFunc";
      if (!m_futureMap.isAlive(threadId)) {
        m_futureMap.removeDeadThread();
        QFuture<void> future =
            QtConcurrent::run(this, &ZFlyEmBodySplitProject::result3dQuickFunc);
        m_futureMap[threadId] = future;
      }
//      result3dQuickFunc();
/*
      loadResult3dQuick(doc);

      m_quickResultWindow->getSwcFilter()->setColorMode("Intrinsic");
      m_quickResultWindow->getSwcFilter()->setRenderingPrimitive("Sphere");
      m_quickResultWindow->setYZView();

      ZDvidReader reader;
      if (reader.open(getDvidTarget())) {
//        ZDvidInfo dvidInfo = reader.readGrayScaleInfo();
        doc->addObject(ZFlyEmMisc::MakeBoundBoxGraph(m_dvidInfo), true);
        doc->addObject(ZFlyEmMisc::MakePlaneGraph(getDocument(), m_dvidInfo), true);
      }

      connect(m_quickResultWindow, SIGNAL(destroyed()),
              this, SLOT(shallowClearQuickResultWindow()));
      connect(mainDoc, SIGNAL(labelFieldModified()),
              this, SLOT(updateResult3dQuick()));
      if (m_dataFrame != NULL) {
        connect(m_quickResultWindow, SIGNAL(locating2DViewTriggered(ZStackViewParam)),
                m_dataFrame, SLOT(setView(ZStackViewParam)));
      }
      connect(m_quickResultWindow, SIGNAL(locating2DViewTriggered(ZStackViewParam)),
              this, SIGNAL(locating2DViewTriggered(ZStackViewParam)));
              */
    } else {
      showQuickView(m_quickResultWindow);
//      showResultQuickView();
      /*
      m_quickResultWindow->show();
      m_quickResultWindow->raise();
      */
    }
  }
}

void ZFlyEmBodySplitProject::showResult3d()
{
  if (getDocument() != NULL) {
    if (m_resultWindow == NULL) {
      //emit messageGenerated("Showing results in 3D ...");
      //ZStackDocReader docReader;
      ZStackDocLabelStackFactory *factory = new ZStackDocLabelStackFactory;
      factory->setDocument(getDocument());
      ZStack *labeled = factory->makeStack();
      if (labeled != NULL) {
        //docReader.setStack(labeled);
        ZStackDoc *doc = new ZStackDoc(labeled, NULL);
        doc->setTag(NeuTube::Document::FLYEM_SPLIT);
        doc->setStackFactory(factory);
        ZWindowFactory windowFactory;
        windowFactory.setWindowTitle("Splitting Result");
        m_resultWindow = windowFactory.make3DWindow(doc);

        //ZStackFrame *newFrame = new ZStackFrame;
        //newFrame->addDocData(docReader);
        //newFrame->document()->setTag(NeuTube::Document::FLYEM_SPLIT);
        //newFrame->document()->setStackFactory(factory);

        connect(getDocument(), SIGNAL(labelFieldModified()),
                doc, SLOT(reloadStack()));
        /*
        m_dataFrame->connect(
              m_dataFrame->document().get(), SIGNAL(labelFieldModified()),
              doc, SLOT(reloadStack()));
              */
        //m_resultWindow = newFrame->open3DWindow(NULL);
        if (getDocument()->hasVisibleSparseStack()) {
          ZIntPoint dsIntv =
              getDocument()->getConstSparseStack()->getDownsampleInterval();
          if (dsIntv.getX() != dsIntv.getZ()) {
            m_resultWindow->getVolumeSource()->setZScale(
                  ((float) (dsIntv.getZ() + 1)) / (dsIntv.getX() + 1));
            m_resultWindow->resetCamera();
          }
        }
        connect(m_resultWindow, SIGNAL(destroyed()),
                this, SLOT(shallowClearResultWindow()));
        //delete newFrame;
      }
    }
    m_resultWindow->show();
    m_resultWindow->raise();

    //emit messageGenerated("Done.");
  }
}

bool ZFlyEmBodySplitProject::hasDataFrame() const
{
  return m_dataFrame != NULL;
}

void ZFlyEmBodySplitProject::setDataFrame(ZStackFrame *frame)
{
  if (m_dataFrame != NULL) {
    clear();
  }

  m_dataFrame = frame;
  connect(m_dataFrame, SIGNAL(splitStarted()), this, SLOT(backupSeed()));

//  updateBookDecoration();
}

void ZFlyEmBodySplitProject::removeAllBookmark()
{
  if (m_bookmarkArray != NULL) {
    m_bookmarkArray->clear();
  }
  clearBookmarkDecoration();
}

void ZFlyEmBodySplitProject::loadBookmark(const QString &filePath)
{
  if (m_bookmarkArray != NULL) {
    ZDvidReader reader;
    ZFlyEmCoordinateConverter converter;
    if (reader.open(m_dvidTarget)) {
      //    ZDvidInfo info = reader.readGrayScaleInfo();
      converter.configure(m_dvidInfo);
      m_bookmarkArray->importJsonFile(filePath.toStdString(), NULL/*&converter*/);
    }
  }
}

bool ZFlyEmBodySplitProject::hasBookmark() const
{
  if (m_bookmarkArray != NULL) {
    return !m_bookmarkArray->isEmpty();
  }

  return false;
}

void ZFlyEmBodySplitProject::locateBookmark(const ZFlyEmBookmark &bookmark)
{
  if (m_dataFrame != NULL) {
    m_dataFrame->viewRoi(bookmark.getLocation().getX(),
                         bookmark.getLocation().getY(),
                         bookmark.getLocation().getZ(), 5);
  }
}

void ZFlyEmBodySplitProject::clearBookmarkDecoration()
{
  if (getDocument() != NULL) {
    for (std::vector<ZStackObject*>::iterator iter = m_bookmarkDecoration.begin();
         iter != m_bookmarkDecoration.end(); ++iter) {
      ZStackObject *obj = *iter;
      getDocument()->removeObject(obj, false);
      delete obj;
    }
  } else {
    for (std::vector<ZStackObject*>::iterator iter = m_bookmarkDecoration.begin();
         iter != m_bookmarkDecoration.end(); ++iter) {
      delete *iter;
    }
  }
  m_bookmarkDecoration.clear();
}

void ZFlyEmBodySplitProject::addBookmarkDecoration(
    const ZFlyEmBookmarkArray &bookmarkArray)
{
  if (getDocument() != NULL) {
    getDocument()->beginObjectModifiedMode(ZStackDoc::OBJECT_MODIFIED_CACHE);
    for (ZFlyEmBookmarkArray::const_iterator iter = bookmarkArray.begin();
         iter != bookmarkArray.end(); ++iter) {
      const ZFlyEmBookmark &bookmark = *iter;
      ZPunctum *circle = new ZPunctum;
      circle->set(bookmark.getLocation(), 5);

//      ZStackBall *circle = new ZStackBall;
//      circle->set(bookmark.getLocation(), 5);
      circle->setColor(255, 0, 0);
      circle->setVisible(m_isBookmarkVisible);
      circle->setHittable(false);
//      circle->setRole(ZStackObjectRole::ROLE_3DGRAPH_DECORATOR);
      getDocument()->addObject(circle);
      m_bookmarkDecoration.push_back(circle);
    }
    getDocument()->endObjectModifiedMode();
    getDocument()->notifyObjectModified();
  }
}

/*
void ZFlyEmBodySplitProject::updateBookmarkDecoration(
    const ZFlyEmBookmarkArray &bookmarkArray)
{
  clearBookmarkDecoration();

  if (getDocument() != NULL) {
    ZFlyEmBookmarkArray filteredBookmarkArray;
    foreach (ZFlyEmBookmark bookmark, bookmarkArray) {
      if (bookmark.getBodyId() == getBodyId() &&
          bookmark.getType() == ZFlyEmBookmark::TYPE_FALSE_MERGE) {
        filteredBookmarkArray.append(bookmark);
      }
    }
    addBookmarkDecoration(filteredBookmarkArray);
  }
}
*/


void ZFlyEmBodySplitProject::updateBookmarkDecoration()
{
  clearBookmarkDecoration();

  if (getDocument() != NULL) {
    ZFlyEmBookmarkArray bookmarkArray;

    //    foreach (ZFlyEmBookmark bookmark, m_bookmarkArray) {
    if (m_bookmarkArray != NULL) {
      for (ZFlyEmBookmarkArray::const_iterator iter = m_bookmarkArray->begin();
           iter != m_bookmarkArray->end(); ++iter) {
        const ZFlyEmBookmark &bookmark = *iter;
        if (bookmark.getBodyId() == getBodyId() &&
            bookmark.getType() == ZFlyEmBookmark::TYPE_FALSE_MERGE) {
          bookmarkArray.append(bookmark);
        }
      }
    }
    addBookmarkDecoration(bookmarkArray);
  }
}

void ZFlyEmBodySplitProject::showBookmark(bool visible)
{
  m_isBookmarkVisible = visible;
  for (std::vector<ZStackObject*>::iterator iter = m_bookmarkDecoration.begin();
       iter != m_bookmarkDecoration.end(); ++iter) {
    ZStackObject *obj = *iter;
    obj->setVisible(visible);
  }
  if (m_dataFrame != NULL && !m_bookmarkDecoration.empty()) {
    m_dataFrame->updateView();
  }
}

std::set<int> ZFlyEmBodySplitProject::getBookmarkBodySet() const
{
  std::set<int> bodySet;
  if (m_bookmarkArray != NULL) {
    for (ZFlyEmBookmarkArray::const_iterator iter = m_bookmarkArray->begin();
         iter != m_bookmarkArray->end(); ++iter) {
      const ZFlyEmBookmark &bookmark = *iter;
      bodySet.insert(bookmark.getBodyId());
    }
  }

  return bodySet;
}

void ZFlyEmBodySplitProject::exportSplits()
{
  //ZObject3dScan body = *(getDataFrame()->document()->getSparseStack()->getObjectMask());

}

void ZFlyEmBodySplitProject::commitResult()
{
  getProgressSignal()->startProgress("Saving splits");

  getProgressSignal()->startProgress(0.8);
  ZFlyEmBodySplitProject::commitResultFunc(
        getDocument()->getConstSparseStack()->getObjectMask(),
        getDocument()->getLabelField(),
        getDocument()->getConstSparseStack()->getDownsampleInterval());
  getProgressSignal()->endProgress();

  deleteSavedSeed();
  getDocument()->undoStack()->clear();
  removeAllSeed();
//  getDocument()->removeObject(ZStackObject::TYPE_OBJ3D);
//  removeAllSideSeed();
  downloadBodyMask();

  getDocument()->setSegmentationReady(false);

  getProgressSignal()->endProgress();

  /*
  QtConcurrent::run(this, &ZFlyEmBodySplitProject::commitResultFunc,
                    getDataFrame()->document()->getSparseStack()->getObjectMask(),
                    getDataFrame()->document()->getLabelField(),
                    getDataFrame()->document()->getSparseStack()->getDownsampleInterval());
                    */
}

void ZFlyEmBodySplitProject::commitResultFunc(
    const ZObject3dScan *wholeBody, const ZStack *stack, const ZIntPoint &dsIntv)
{
  getProgressSignal()->startProgress("Uploading splitted bodies");

  emitMessage("Uploading results ...");

//  const ZObject3dScan *wholeBody =
//      getDataFrame()->document()->getSparseStack()->getObjectMask();

  ZObject3dScan body = *wholeBody;

  emitMessage(QString("Backup ... %1").arg(getBodyId()));

  std::string backupDir =
      NeutubeConfig::getInstance().getPath(NeutubeConfig::AUTO_SAVE);
  body.save(backupDir + "/" + getSeedKey(getBodyId()) + ".sobj");

//  const ZStack *stack = getDataFrame()->document()->getLabelField();
  QStringList filePathList;
  QList<uint64_t> oldBodyIdList;
  int maxNum = 1;

  if (stack != NULL) { //Process splits
//    const ZIntPoint &dsIntv =
//        getDataFrame()->document()->getSparseStack()->getDownsampleInterval();
    std::vector<ZObject3dScan*> objArray =
        ZObject3dScan::extractAllObject(*stack);

//    emit progressAdvanced(0.1);
    getProgressSignal()->advanceProgress(0.1);

    double dp = 0.3;

    if (!objArray.empty()) {
      dp = 0.3 / objArray.size();
    }

    for (std::vector<ZObject3dScan*>::iterator iter = objArray.begin();
         iter != objArray.end(); ++iter) {
      ZObject3dScan *obj = *iter;
      obj->upSample(dsIntv.getX(), dsIntv.getY(), dsIntv.getZ());

      ZObject3dScan currentBody = body.subtract(*obj);

      if (!currentBody.isEmpty() && obj->getLabel() > 1) {
        std::vector<ZObject3dScan> objArray =
            currentBody.getConnectedComponent();
        for (std::vector<ZObject3dScan>::iterator iter = objArray.begin();
             iter != objArray.end(); ++iter) {
          ZString output = QDir::tempPath() + "/body_";
          output.appendNumber(getBodyId());
          output += "_";
          output.appendNumber(obj->getLabel());
          output += "_";
          output.appendNumber(maxNum++);
          iter->save(output + ".sobj");
          filePathList << (output + ".sobj").c_str();
          oldBodyIdList << obj->getLabel();
        }
      }
      delete obj;

      getProgressSignal()->advanceProgress(dp);
//      emit progressAdvanced(dp);
    }
  }

  if (!body.isEmpty()) {
    std::vector<ZObject3dScan> objArray = body.getConnectedComponent();

#ifdef _DEBUG_2
    body.save(GET_TEST_DATA_DIR + "/test.sobj");
#endif

    double dp = 0.3;

    if (!objArray.empty()) {
      dp = 0.3 / objArray.size();
    }

    const size_t sizeThreshold = 0;
    //if (objArray.size() > 1 || !filePathList.isEmpty()) {
    for (std::vector<ZObject3dScan>::const_iterator iter = objArray.begin();
         iter != objArray.end(); ++iter) {
      const ZObject3dScan &obj = *iter;
      if (obj.getVoxelNumber() > sizeThreshold) {
        ZString output = QDir::tempPath() + "/body_";
        output.appendNumber(getBodyId());
        output += "_";
        output.appendNumber(body.getLabel());
        output += "_";
        output.appendNumber(maxNum++);
        obj.save(output + ".sobj");
        filePathList << (output + ".sobj").c_str();
        oldBodyIdList << 0;
      }

      getProgressSignal()->advanceProgress(dp);
//      emit progressAdvanced(dp);
    }
    //}
  }

  ZDvidReader reader;
  reader.open(m_dvidTarget);
//  int bodyId = reader.readMaxBodyId();

  int bodyIndex = 0;

  double dp = 0.3;

  if (!filePathList.empty()) {
    dp = 0.3 / filePathList.size();
  }

  QList<uint64_t> newBodyIdList;

  ZDvidWriter writer;
  writer.open(getDvidTarget());
  foreach (QString objFile, filePathList) {
    ZObject3dScan obj;
    obj.load(objFile.toStdString());
    uint64_t newBodyId = writer.writeSplit(
          getDvidTarget().getBodyLabelName(), obj, getBodyId(), ++bodyIndex);

#if 0
    QString command = buildemPath +
        QString("/bin/dvid_load_sparse http://emdata2:8000 %1 %2 %3 %4").
        arg(m_dvidTarget.getUuid().c_str()).
        arg(m_dvidTarget.getBodyLabelName().c_str()).
        arg(objFile).arg(++bodyId);

    qDebug() << command;

    QProcess::execute(command);
#endif

    uint64_t oldBodyId = oldBodyIdList[bodyIndex - 1];
    QString msg;
    if (newBodyId > 0) {
      if (oldBodyId > 0) {
        msg = QString("Label %1 uploaded as %2 (%3 voxels).").
            arg(oldBodyId).arg(newBodyId).arg(obj.getVoxelNumber());
      } else {
        msg = QString("Isolated object uploaded as %1 (%2 voxels) .").
            arg(newBodyId).arg(obj.getVoxelNumber());
      }
      newBodyIdList.append(newBodyId);

      emitMessage(msg);
    } else {
      emitError("Warning: Something wrong happened during uploading! "
                "Please contact the developer as soon as possible.");
    }


    getProgressSignal()->advanceProgress(dp);
//    emit progressAdvanced(dp);
  }

  if (!newBodyIdList.isEmpty()) {
    QString bodyMessage = QString("Body %1 splitted: ").arg(wholeBody->getLabel());
    bodyMessage += "<font color=#007700>";
    foreach (uint64_t bodyId, newBodyIdList) {
      bodyMessage.append(QString("%1 ").arg(bodyId));
    }
    bodyMessage += "</font>";
    emitMessage(bodyMessage);
  }

  getProgressSignal()->endProgress();
  //writer.writeMaxBodyId(bodyId);

//  emit progressDone();
  emitMessage("Done.");
  emit resultCommitted();
}

int ZFlyEmBodySplitProject::selectAllSeed()
{
  int nSelected = 0;
  if (getDocument() != NULL) {
    QList<ZDocPlayer*> playerList =
        getDocument()->getPlayerList(ZStackObjectRole::ROLE_SEED);
//    getDocument()->deselectAllObject();
    foreach (const ZDocPlayer *player, playerList) {
      getDocument()->setSelected(player->getData(), true);
      ++nSelected;
    }
    if (m_dataFrame != NULL) {
      m_dataFrame->view()->paintObject();
    }
  }

  return nSelected;
}

int ZFlyEmBodySplitProject::selectSeed(int label)
{
  int nSelected = 0;
  if (getDocument() != NULL) {
    QList<ZDocPlayer*> playerList =
        getDocument()->getPlayerList(ZStackObjectRole::ROLE_SEED);
    getDocument()->deselectAllObject();
    foreach (const ZDocPlayer *player, playerList) {
      if (player->getLabel() == label) {
       getDocument()->setSelected(player->getData(), true);
       ++nSelected;
      }
    }
    if (m_dataFrame != NULL) {
      m_dataFrame->view()->paintObject();
    }
  }

  return nSelected;
}

void ZFlyEmBodySplitProject::backupSeed()
{
  if (getBodyId() == 0) {
    return;
  }

  ZDvidReader reader;
  if (reader.open(getDvidTarget())) {
    QList<ZDocPlayer*> playerList;
    if (getDocument() != NULL) {
      playerList = getDocument()->getPlayerList(ZStackObjectRole::ROLE_SEED);
    }
    ZJsonArray jsonArray;
    foreach (const ZDocPlayer *player, playerList) {
      ZJsonObject jsonObj = player->toJsonObject();
      if (!jsonObj.isEmpty()) {
        jsonArray.append(jsonObj);
      }
    }

    ZDvidWriter writer;
    if (writer.open(getDvidTarget())) {
      if (!jsonArray.isEmpty()) {
        ZJsonObject rootObj;
        rootObj.setEntry("seeds", jsonArray);
        writer.writeJson(getSplitLabelName(), getBackupSeedKey(getBodyId()),
                         rootObj);
      }
    }
  }
}

void ZFlyEmBodySplitProject::deleteSavedSeed()
{
  ZDvidReader reader;
  if (reader.open(getDvidTarget())) {
    if (!reader.hasData(getSplitLabelName())) {
      emitError(
            ("Failed to delete seed: " + getSplitLabelName() +
            " has not been created on the server.").c_str());

      return;
    }
  }

  ZDvidWriter writer;
  if (writer.open(getDvidTarget())) {
    writer.deleteKey(getSplitLabelName(), getSeedKey(getBodyId()));
    emit messageGenerated(QString("All seeds of %1 have been deleted").
                          arg(getBodyId()));
  }
}

void ZFlyEmBodySplitProject::saveSeed(bool emphasizingMessage)
{
  ZDvidReader reader;
  if (reader.open(getDvidTarget())) {
    if (!reader.hasData(getSplitLabelName())) {
      emitError(
            ("Failed to save seed: " + getSplitLabelName() +
            " has not been created on the server.").c_str());

      return;
    }
  }

  QList<ZDocPlayer*> playerList =
      getDocument()->getPlayerList(ZStackObjectRole::ROLE_SEED);
  ZJsonArray jsonArray;
  foreach (const ZDocPlayer *player, playerList) {
    ZJsonObject jsonObj = player->toJsonObject();
    if (!jsonObj.isEmpty()) {
      jsonArray.append(jsonObj);
    }
  }


  ZDvidWriter writer;
  if (writer.open(getDvidTarget())) {
    if (jsonArray.isEmpty()) {
      writer.deleteKey(getSplitLabelName(), getSeedKey(getBodyId()));
      if (emphasizingMessage) {
        emitPopoupMessage("All seeds deleted");
      } else {
        emitMessage("All seeds deleted");
      }
    } else {
      ZJsonObject rootObj;
      rootObj.setEntry("seeds", jsonArray);
      writer.writeJson(getSplitLabelName(), getSeedKey(getBodyId()), rootObj);
      if (emphasizingMessage) {
        emitPopoupMessage("All seeds have been saved");
      }
      emitMessage(ZWidgetMessage::appendTime("All seeds saved"));
    }
  }
}

void ZFlyEmBodySplitProject::recoverSeed()
{
  downloadSeed(getBackupSeedKey(getBodyId()));
}

void ZFlyEmBodySplitProject::loadSeed(const ZJsonObject &obj)
{
  if (obj.hasKey("seeds")) {
    ZLabelColorTable colorTable;

    ZJsonArray jsonArray(obj["seeds"], ZJsonValue::SET_INCREASE_REF_COUNT);
    for (size_t i = 0; i < jsonArray.size(); ++i) {
      ZJsonObject seedJson(
            jsonArray.at(i), ZJsonValue::SET_INCREASE_REF_COUNT);
      if (seedJson.hasKey("stroke")) {
        ZStroke2d *stroke = new ZStroke2d;
        stroke->loadJsonObject(seedJson);
        if (!stroke->isEmpty()) {
          stroke->setRole(ZStackObjectRole::ROLE_SEED |
                          ZStackObjectRole::ROLE_3DGRAPH_DECORATOR);
          stroke->setPenetrating(false);
          getDocument()->addObject(stroke);
        } else {
          delete stroke;
        }
      } else if (seedJson.hasKey("obj3d")) {
        ZObject3d *obj3d = new ZObject3d;
        obj3d->loadJsonObject(seedJson);
        obj3d->setColor(colorTable.getColor(obj3d->getLabel()));

        if (!obj3d->isEmpty()) {
          obj3d->setRole(ZStackObjectRole::ROLE_SEED |
                         ZStackObjectRole::ROLE_3DGRAPH_DECORATOR);
          getDocument()->addObject(obj3d);
        } else {
          delete obj3d;
        }
      }
    }
  }
}

void ZFlyEmBodySplitProject::importSeed(const QString &fileName)
{
  if (!fileName.isEmpty()) {
    ZJsonObject obj;
    obj.load(fileName.toStdString());
    if (obj.hasKey("seeds")) {
#ifdef _DEBUG_
      std::cout << getDocument()->getPlayerList(
                     ZStackObjectRole::ROLE_SEED).size() << " seeds" <<  std::endl;
#endif

      getDocument()->undoStack()->clear();
      removeAllSeed();
      loadSeed(obj);
    } else {
      emit messageGenerated(
            ZWidgetMessage(
              QString("Invalid seed file: %1. Seeds remain unchanged").
              arg(fileName),
              NeuTube::MSG_ERROR));
    }
  }
}

void ZFlyEmBodySplitProject::exportSeed(const QString &fileName)
{
  if (!fileName.isEmpty()) {
    QList<ZDocPlayer*> playerList =
        getDocument()->getPlayerList(ZStackObjectRole::ROLE_SEED);
    ZJsonArray jsonArray;
    foreach (const ZDocPlayer *player, playerList) {
      ZJsonObject jsonObj = player->toJsonObject();
      if (!jsonObj.isEmpty()) {
        jsonArray.append(jsonObj);
      }
    }

    ZDvidWriter writer;
    if (writer.open(getDvidTarget())) {
      if (!jsonArray.isEmpty()) {
        ZJsonObject rootObj;
        rootObj.setEntry("seeds", jsonArray);
        rootObj.dump(fileName.toStdString());
      }
    }
  }
}

void ZFlyEmBodySplitProject::downloadSeed()
{
  downloadSeed(getSeedKey(getBodyId()));
}

void ZFlyEmBodySplitProject::removeAllSeed()
{
  getDocument()->removeObject(ZStackObjectRole::ROLE_SEED, true);
}

void ZFlyEmBodySplitProject::removeAllSideSeed()
{
  std::set<ZStackObject*> removeSet;
  ZDocPlayerList &playerList = getDocument()->getPlayerList();
  for (ZDocPlayerList::iterator iter = playerList.begin();
       iter != playerList.end(); /*++iter*/) {
    ZDocPlayer *player = *iter;
    if (player->hasRole(ZStackObjectRole::ROLE_SEED) && player->getLabel() > 1) {
      removeSet.insert(player->getData());
      iter = playerList.erase(iter);
      delete player;
    } else {
      ++iter;
    }
  }

  getDocument()->getObjectGroup().removeObject(
        removeSet.begin(), removeSet.end(), true);

  if (!removeSet.empty()) {
    getDocument()->notifyObjectModified();
    getDocument()->notifyPlayerChanged(ZStackObjectRole::ROLE_SEED);
  }
}

void ZFlyEmBodySplitProject::downloadSeed(const std::string &seedKey)
{
  ZDvidReader reader;
  if (reader.open(getDvidTarget())) {
    getDocument()->undoStack()->clear();
    removeAllSeed();
    QByteArray seedData = reader.readKeyValue(
          getSplitLabelName().c_str(), seedKey.c_str());
    if (!seedData.isEmpty()) {
      ZJsonObject obj;
      obj.decode(seedData.constData());

      loadSeed(obj);
    }
  }
}

ZFlyEmNeuron ZFlyEmBodySplitProject::getFlyEmNeuron() const
{
  ZFlyEmNeuron neuron;
  neuron.setId(getBodyId());
  neuron.setPath(getDvidTarget());

  return neuron;
}

void ZFlyEmBodySplitProject::viewPreviousSlice()
{
  if (getDataFrame() != NULL) {
    getDataFrame()->view()->blockRedraw(true);
    getDataFrame()->view()->stepSlice(-1);
    getDataFrame()->view()->blockRedraw(false);
    viewFullGrayscale();
    updateBodyMask();
  }
}

void ZFlyEmBodySplitProject::viewNextSlice()
{
  if (getDataFrame() != NULL) {
    getDataFrame()->view()->blockRedraw(true);
    getDataFrame()->view()->stepSlice(1);
    getDataFrame()->view()->blockRedraw(false);
    viewFullGrayscale();
    updateBodyMask();
  }
}

void ZFlyEmBodySplitProject::viewFullGrayscale(bool viewing)
{
  ZStackFrame *frame = getDataFrame();
  if (frame != NULL) {
    if (viewing) {
      viewFullGrayscale();
    } else {
      ZStackObject *obj =
          frame->document()->getObjectGroup().findFirstSameSource(
            ZStackObject::TYPE_DVID_GRAY_SLICE,
            ZStackObjectSourceFactory::MakeDvidGraySliceSource());
      if (obj != NULL) {
        obj->setVisible(false);
        frame->updateView();
      }
    }
  }
}

void ZFlyEmBodySplitProject::viewFullGrayscale()
{
  ZDvidReader reader;
  if (reader.open(getDvidTarget())) {
    ZStackFrame *frame = getDataFrame();
    if (frame != NULL) {
      int currentSlice = frame->view()->sliceIndex();

      QRect rect = frame->view()->getViewPort(NeuTube::COORD_STACK);
      ZRect2d rectRoi;
      rectRoi.set(rect.x(), rect.y(), rect.width(), rect.height());
//      ZRect2d rectRoi = frame->document()->getRect2dRoi();
      ZIntPoint offset = frame->document()->getStackOffset();
//      if (!rectRoi.isValid()) {
//        int width = frame->document()->getStackWidth();
//        int height = frame->document()->getStackHeight();
//        rectRoi.set(offset.getX(), offset.getY(), width, height);
//      }

      int z = currentSlice + offset.getZ();
      ZDvidGraySlice *graySlice = dynamic_cast<ZDvidGraySlice*>(
            frame->document()->getObjectGroup().findFirstSameSource(
              ZStackObject::TYPE_DVID_GRAY_SLICE,
              ZStackObjectSourceFactory::MakeDvidGraySliceSource()));

      if (graySlice == NULL) {
        graySlice = new ZDvidGraySlice();
        graySlice->setDvidTarget(getDvidTarget());
        graySlice->setSource(ZStackObjectSourceFactory::MakeDvidGraySliceSource());
        frame->document()->addObject(graySlice, false);
      }

      graySlice->setVisible(true);

      graySlice->setBoundBox(rectRoi);
      graySlice->update(z);

      frame->updateView();


#if 0
      ZStack *stack = reader.readGrayScale(
            rectRoi.getX0(), rectRoi.getY0(), z,
            rectRoi.getWidth(), rectRoi.getHeight(), 1);
      ZStackPatch *patch = new ZStackPatch(stack);
      patch->setZOrder(-1);
      patch->setSource(ZStackObjectSource::getSource(
                         ZStackObjectSource::ID_BODY_GRAYSCALE_PATCH));
      patch->setTarget(ZStackObject::STACK_CANVAS);
      frame->document()->addStackPatch(patch);
#endif
    }
  }
}

void ZFlyEmBodySplitProject::downloadBodyMask()
{
  ZFlyEmProofDoc *doc = getDocument<ZFlyEmProofDoc>();
  if (doc != NULL) {
    getDocument<ZFlyEmProofDoc>()->downloadBodyMask();
  }
}

void ZFlyEmBodySplitProject::updateBodyMask()
{
  ZStackFrame *frame = getDataFrame();
  if (frame != NULL) {
    frame->document()->removeObject(ZStackObjectRole::ROLE_MASK, true);
    if (showingBodyMask()) {
      ZDvidReader reader;
      if (reader.open(getDvidTarget())) {
        int currentSlice = frame->view()->sliceIndex();

        ZRect2d rectRoi = frame->document()->getRect2dRoi();
        ZIntPoint offset = frame->document()->getStackOffset();
        if (!rectRoi.isValid()) {
          int width = frame->document()->getStackWidth();
          int height = frame->document()->getStackHeight();
          rectRoi.set(offset.getX(), offset.getY(), width, height);
        }

        int z = currentSlice + offset.getZ();
        ZArray *array = reader.readLabels64(
              rectRoi.getX0(), rectRoi.getY0(), z,
              rectRoi.getWidth(), rectRoi.getHeight(), 1);
        /*
        ZStack *stack = reader.readBodyLabel(
              rectRoi.getX0(), rectRoi.getY0(), z,
              rectRoi.getWidth(), rectRoi.getHeight(), 1);
              */
        if (array != NULL) {
          /*
          std::map<int, ZObject3dScan*> *bodySet =
              ZObject3dScan::extractAllObject(
                (uint64_t*) stack->array8(), stack->width(), stack->height(), 1,
                stack->getOffset().getZ(), 1, NULL);
                */

          std::map<int, ZObject3dScan*> *bodySet =
              ZObject3dScan::extractAllObject(
                array->getDataPointer<uint64_t>(), array->getDim(0),
                array->getDim(1), 1,
                array->getStartCoordinate(2), 1, NULL);

          frame->document()->blockSignals(true);
          for (std::map<int, ZObject3dScan*>::const_iterator iter = bodySet->begin();
               iter != bodySet->end(); ++iter) {
            int label = iter->first;
            ZObject3dScan *obj = iter->second;
            if (label > 0) {
              obj->translate(
                    array->getStartCoordinate(0),
                    array->getStartCoordinate(1), 0);
              obj->setRole(ZStackObjectRole::ROLE_MASK);
              frame->document()->addObject(obj, false);
            } else {
              delete obj;
            }
          }
          frame->document()->blockSignals(false);
          frame->document()->notifyObject3dScanModified();
          frame->document()->notifyPlayerChanged(ZStackObjectRole::ROLE_MASK);
        }

        delete array;
      }
    }
  }
}

std::string ZFlyEmBodySplitProject::getSplitStatusName() const
{
  return ZDvidData::GetName(
        ZDvidData::ROLE_SPLIT_STATUS, ZDvidData::ROLE_BODY_LABEL,
        getDvidTarget().getBodyLabelName());
}

std::string ZFlyEmBodySplitProject::getSplitLabelName() const
{
  return ZDvidData::GetName(ZDvidData::ROLE_SPLIT_LABEL,
                            ZDvidData::ROLE_BODY_LABEL,
                            getDvidTarget().getBodyLabelName());
}

std::string ZFlyEmBodySplitProject::getSeedKey(uint64_t bodyId) const
{
  return getDvidTarget().getBodyLabelName() + "_seed_" +
      ZString::num2str(bodyId);
}

std::string ZFlyEmBodySplitProject::getBackupSeedKey(uint64_t bodyId) const
{
  return getDvidTarget().getBodyLabelName() + "_backup_seed_" +
      ZString::num2str(bodyId);
}

void ZFlyEmBodySplitProject::runSplit()
{
  if (getDocument() != NULL) {
    backupSeed();
    getDocument()->runSeededWatershed();
  }

  /*
  ZStackFrame *frame = getDataFrame();
  if (frame != NULL) {
    frame->document()->runSeededWatershed();
  }
  */
}

void ZFlyEmBodySplitProject::setSeedProcessed(uint64_t bodyId)
{
  ZDvidWriter writer;
  if (writer.open(getDvidTarget())) {
    ZJsonObject statusJson;
    statusJson.setEntry("processed", true);
    writer.writeJson(getSplitStatusName(),
                     getSeedKey(bodyId), statusJson);
  }
}

bool ZFlyEmBodySplitProject::isSeedProcessed(uint64_t bodyId) const
{
  bool isProcessed = false;

  ZDvidReader reader;
  if (reader.open(getDvidTarget())) {
    QByteArray value = reader.readKeyValue(
          getSplitStatusName().c_str(),
          getSeedKey(bodyId).c_str());

    if (!value.isEmpty()) {
      ZJsonObject statusJson;
      statusJson.decodeString(value.data());
      if (statusJson.hasKey("processed")) {
        isProcessed = ZJsonParser::booleanValue(statusJson["processed"]);
      }
    }
  }

  return isProcessed;
}

ZStackDoc* ZFlyEmBodySplitProject::getDocument() const
{
  if (getDataFrame() != NULL) {
    return getDataFrame()->document().get();
  }

  return m_doc.get();
}

ZSharedPointer<ZStackDoc> ZFlyEmBodySplitProject::getSharedDocument() const
{
  if (getDataFrame() != NULL) {
    return getDataFrame()->document();
  }

  return m_doc;
}

void ZFlyEmBodySplitProject::setDocument(ZSharedPointer<ZStackDoc> doc)
{
  m_doc = doc;
}

bool ZFlyEmBodySplitProject::isReadyForSplit(const ZDvidTarget &target)
{
  bool succ = true;

  ZWidgetMessage message;
//  QStringList infoList;

  ZDvidReader reader;
  if (reader.open(target)) {
    if (!reader.hasSparseVolume()) {
      message.appendMessage(("Incomplete split database: data \"" +
                             target.getBodyLabelName() +
                             "\" missing").c_str());
//      infoList.append();
      succ = false;
    }

    std::string splitLabelName = ZDvidData::GetName(
          ZDvidData::ROLE_SPLIT_LABEL, ZDvidData::ROLE_BODY_LABEL,
          target.getBodyLabelName());

    if (!reader.hasData(splitLabelName)) {
      message.appendMessage(("Incomplete split database: data \"" + splitLabelName +
                             "\" missing").c_str());
//      infoList.append();
      succ = false;
    }

    std::string splitStatusName =  ZDvidData::GetName(
          ZDvidData::ROLE_SPLIT_STATUS, ZDvidData::ROLE_BODY_LABEL,
          target.getBodyLabelName());
    if (!reader.hasData(splitStatusName)) {
      message.appendMessage(("Incomplete split database: data \"" + splitStatusName +
                             "\" missing").c_str());
//      infoList.append();
      succ = false;
    }
  } else {
    message.appendMessage("Cannot connect to database.");
//    infoList.append();
    succ = false;
  }

  message.setType(NeuTube::MSG_ERROR);

  emit messageGenerated(message.toHtmlString(), true);
//  emit messageGenerated(message);

  return succ;
}

void ZFlyEmBodySplitProject::emitMessage(const QString &msg, bool appending)
{
  qDebug() << "Outputting message: " << msg;

  ZWidgetMessage::ETarget target = ZWidgetMessage::TARGET_TEXT;
  if (appending) {
    target = ZWidgetMessage::TARGET_TEXT_APPENDING;
  }

  emit messageGenerated(
        ZWidgetMessage(msg, NeuTube::MSG_INFORMATION, target));
}

void ZFlyEmBodySplitProject::emitPopoupMessage(const QString &msg)
{
  ZWidgetMessage message(msg, NeuTube::MSG_INFORMATION);
  message.setTarget(ZWidgetMessage::TARGET_DIALOG);
  emit messageGenerated(message);
}


void ZFlyEmBodySplitProject::emitError(const QString &msg, bool appending)
{
  ZWidgetMessage::ETarget target = ZWidgetMessage::TARGET_TEXT;
  if (appending) {
    target = ZWidgetMessage::TARGET_TEXT_APPENDING;
  }

  emit messageGenerated(
        ZWidgetMessage(msg, NeuTube::MSG_ERROR, target));
}

void ZFlyEmBodySplitProject::update3DViewPlane()
{
  if (m_quickViewWindow || m_quickResultWindow) {

//    ZDvidReader reader;

//    tic();
    if (m_dvidInfo.isValid()) {
//      ZDvidInfo dvidInfo = reader.readGrayScaleInfo();


      if (m_quickViewWindow != NULL) {
        Z3DGraph *graph = ZFlyEmMisc::MakePlaneGraph(getDocument(), m_dvidInfo);
        m_quickViewWindow->getDocument()->addObject(graph, true);
      }

      if (m_quickResultWindow != NULL) {
        Z3DGraph *graph = ZFlyEmMisc::MakePlaneGraph(getDocument(), m_dvidInfo);
        m_quickResultWindow->getDocument()->addObject(graph, true);
      }
//      std::cout << "3d plane rendering time: " << toc() << std::endl;
    }

  }
}

void ZFlyEmBodySplitProject::attachBookmarkArray(ZFlyEmBookmarkArray *bookmarkArray)
{
  m_bookmarkArray = bookmarkArray;
}
