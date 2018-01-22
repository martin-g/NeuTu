#ifndef ZFLYEMBODY3DDOC_H
#define ZFLYEMBODY3DDOC_H

#include "zstackdoc.h"

#include <QSet>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QColor>
#include <QList>
#include <QTime>

#include "neutube_def.h"
#include "dvid/zdvidtarget.h"
#include "dvid/zdvidreader.h"
#include "dvid/zdvidinfo.h"
#include "dvid/zdvidwriter.h"
#include "zthreadfuturemap.h"
#include "zsharedpointer.h"
//#include "flyem/zflyemtodoitem.h"

class ZFlyEmProofDoc;
class ZFlyEmBodyMerger;
class ZFlyEmBodyComparisonDialog;
class ZFlyEmBody3dDocKeyProcessor;
class ZMesh;
//class ZFlyEmToDoItem;

class ZFlyEmBody3dDoc : public ZStackDoc
{
  Q_OBJECT
public:
  explicit ZFlyEmBody3dDoc(QObject *parent = 0);
  ~ZFlyEmBody3dDoc();


  void setDataDoc(ZSharedPointer<ZStackDoc> doc);

public:
  class BodyEvent {
  public:
    enum EAction {
      ACTION_NULL, ACTION_REMOVE, ACTION_ADD, ACTION_FORCE_ADD,
      ACTION_UPDATE, ACTION_CACHE
    };

    typedef uint64_t TUpdateFlag;

  public:
    BodyEvent() : m_action(ACTION_NULL), m_bodyId(0), /*m_refreshing(false),*/
    m_updateFlag(0), m_resLevel(0) {}
    BodyEvent(BodyEvent::EAction action, uint64_t bodyId) :
      m_action(action), m_bodyId(bodyId), m_updateFlag(0), m_resLevel(0) {}

    EAction getAction() const { return m_action; }
    uint64_t getBodyId() const { return m_bodyId; }
    const QColor& getBodyColor() const { return m_bodyColor; }
//    bool isRefreshing() const { return m_refreshing; }

    void setAction(EAction action) { m_action = action; }
    void setBodyColor(const QColor &color) { m_bodyColor = color; }

    void mergeEvent(const BodyEvent &event, neutube::EBiDirection direction);

//    void syncBodySelection();

    bool updating(TUpdateFlag flag) const {
      return (m_updateFlag & flag) > 0;
    }

    void addUpdateFlag(TUpdateFlag flag) {
      m_updateFlag |= flag;
    }

    void removeUpdateFlag(TUpdateFlag flag) {
      m_updateFlag &= ~flag;
    }

    TUpdateFlag getUpdateFlag() const {
      return m_updateFlag;
    }

    bool hasUpdateFlag(TUpdateFlag flag) {
      return (m_updateFlag & flag) > 0;
    }

    int getResLevel() const {
      return m_resLevel;
    }

    void setResLevel(int level) {
      m_resLevel = level;
    }

    void decResLevel() {
      --m_resLevel;
    }

    void print() const;

  public:
    static const TUpdateFlag UPDATE_CHANGE_COLOR;
    static const TUpdateFlag UPDATE_ADD_SYNAPSE;
    static const TUpdateFlag UPDATE_ADD_TODO_ITEM;
    static const TUpdateFlag UPDATE_MULTIRES;
    static const TUpdateFlag UPDATE_SEGMENTATION;

  private:
    EAction m_action;
    uint64_t m_bodyId;
    QColor m_bodyColor;
//    bool m_refreshing;
    TUpdateFlag m_updateFlag;
    int m_resLevel;
  };

  class ObjectStatus {
  public:
    explicit ObjectStatus(int timeStamp = 0);
    void setRecycable(bool on);
    void setTimeStamp(int t);
    void setResLevel(int level);

    bool isRecycable() const;
    int getTimeStamp() const;
    int getResLevel() const;

  private:
    void init(int timeStatus);

  private:
    bool m_recycable;
    int m_timeStamp;
    int m_resLevel;
  };

  void setBodyType(flyem::EBodyType type);
  flyem::EBodyType getBodyType() const { return m_bodyType; }

  QSet<uint64_t> getBodySet() const { return m_bodySet; }

  void addBody(uint64_t bodyId, const QColor &color);
  void removeBody(uint64_t bodyId);
  void updateBody(uint64_t bodyId, const QColor &color);
  void updateBody(uint64_t bodyId, const QColor &color, flyem::EBodyType type);

  void addSynapse(uint64_t bodyId);
  void addTodo(uint64_t bodyId);
  void addTodo(int x, int y, int z, bool checked, uint64_t bodyId);
  void addTodo(const ZFlyEmToDoItem &item, uint64_t bodyId);
  void addTodoSliently(const ZFlyEmToDoItem &item);
  void addTodo(const QList<ZFlyEmToDoItem> &itemList);
  void updateSegmentation();
  void removeTodo(ZFlyEmToDoItem &item, uint64_t bodyId);
  void removeTodo(const QList<ZFlyEmToDoItem> &itemList);
  void removeTodo(int x, int y, int z);
  void removeTodoSliently(const ZFlyEmToDoItem &item);
  ZFlyEmToDoItem makeTodoItem(
      int x, int y, int z, bool checked, uint64_t bodyId);
  ZFlyEmToDoItem readTodoItem(int x, int y, int z) const;
  void loadSplitTask(uint64_t bodyId);

  void addEvent(BodyEvent::EAction action, uint64_t bodyId,
                BodyEvent::TUpdateFlag flag = 0, QMutex *mutex = NULL);
  void addEvent(const BodyEvent &event, QMutex *mutex = NULL);

  template <typename InputIterator>
  void addBodyChangeEvent(const InputIterator &first, const InputIterator &last);

  bool hasBody(uint64_t bodyId);

  inline const ZDvidTarget& getDvidTarget() const {
    return m_dvidTarget;
  }

  void setDvidTarget(const ZDvidTarget &target);

  const ZDvidInfo& getDvidInfo() const;

//  void updateFrame();

  ZFlyEmProofDoc* getDataDocument() const;

  void printEventQueue() const;

  void dumpAllBody(bool recycable);

  void dumpGarbage(ZStackObject *obj, bool recycable);
  void dumpGarbageUnsync(ZStackObject *obj, bool recycable);

  template<typename InputIterator>
  void dumpGarbage(const InputIterator &first, const InputIterator &last,
                   bool recycable);
  void mergeBodyModel(const ZFlyEmBodyMerger &merger);

  void processEventFunc();
  void cancelEventThread();

  void setTodoItemSelected(ZFlyEmToDoItem *item, bool select);
//  void setTodoVisible(ZFlyEmToDoItem::EToDoAction action, bool visible);

  bool hasTodoItemSelected() const;

  ZFlyEmToDoItem* getOneSelectedTodoItem() const;

  void forceBodyUpdate();
  void compareBody();
  void compareBody(const std::string &uuid);
  void compareBody(const ZFlyEmBodyComparisonDialog *dlg);
  void compareBody(ZDvidReader &diffReader);
  void compareBody(ZDvidReader &diffReader, const ZIntPoint &pt);

  std::vector<std::string> getParentUuidList() const;
  std::vector<std::string> getAncestorUuidList() const;

  void waitForAllEvent();

  bool updating() const;

  void enableNodeSeeding(bool on);
  void enableBodySelectionSync(bool on);

  void enableGarbageLifetimeLimit(bool on);

  ZMesh *readMesh(const ZDvidReader &reader, uint64_t bodyId, int zoom);

  // The instances referred to by ZDvidUrl::getMeshesTarsUrl() represent data that
  // uses the body's identifier in multiple ways: for multiple meshes, at different
  // levels in the agglomeration history, and as a key whose associated value is a
  // tar file of meshes.  These distinct identifiers are created by encoding a
  // raw body identifier.

  static uint64_t encode(uint64_t rawId, unsigned int level = 0, bool tar = true);
  static bool encodesTar(uint64_t id);
  static unsigned int encodedLevel(uint64_t id);

public:
  void executeAddTodoCommand(int x, int y, int z, bool checked, uint64_t bodyId);
  void executeRemoveTodoCommand();

public slots:
  void showSynapse(bool on);// { m_showingSynapse = on; }
  void addSynapse(bool on);
  void showTodo(bool on);
  void addTodo(bool on);
  void updateTodo(uint64_t bodyId);
  void setUnrecycable(const QSet<uint64_t> &bodySet);
  void setNormalTodoVisible(bool visible);
  void setSelectedTodoItemChecked(bool on);
  void checkSelectedTodoItem();
  void uncheckSelectedTodoItem();

  void recycleObject(ZStackObject *obj) override;
  void killObject(ZStackObject *obj) override;

  void setSeedType(int type);

  void setBodyModelSelected(const QSet<uint64_t> &bodySet);
  void saveSplitTask();
  void deleteSplitSeed();
  void deleteSelectedSplitSeed();

  void processObjectModifiedFromDataDoc(const ZStackObjectInfoSet &objInfo);

  void cacheObject(ZStackObject *obj);

signals:
  void bodyRemoved(uint64_t bodyId);

protected:
  void autoSave() override {}
  void makeKeyProcessor() override;

private:
  ZStackObject* retriveBodyObject(
      uint64_t bodyId, int zoom,
      flyem::EBodyType bodyType, ZStackObject::EType objType);
  ZStackObject* retriveBodyObject(uint64_t bodyId, int zoom);
  ZSwcTree* retrieveBodyModel(uint64_t bodyId, int zoom, flyem::EBodyType bodyType);
  ZSwcTree* getBodyModel(uint64_t bodyId, int zoom, flyem::EBodyType bodyType);
  ZMesh* getBodyMesh(uint64_t bodyId, int zoom);
  ZMesh* retrieveBodyMesh(uint64_t bodyId, int zoom);

  ZMesh *readMesh(uint64_t bodyId, int zoom);

//  ZSwcTree* makeBodyModel(uint64_t bodyId, int zoom);
  ZSwcTree* makeBodyModel(uint64_t bodyId, int zoom, flyem::EBodyType bodyType);

  bool getCachedMeshes(uint64_t bodyId, int zoom, std::map<uint64_t, ZMesh*> &result);
  void makeBodyMeshModels(uint64_t id, int zoom, std::map<uint64_t, ZMesh*> &result);

  std::vector<ZSwcTree*> makeDiffBodyModel(
      uint64_t bodyId1, ZDvidReader &diffReader, int zoom,
      flyem::EBodyType bodyType);

  std::vector<ZSwcTree*> makeDiffBodyModel(
      uint64_t bodyId1, uint64_t bodyId2, ZDvidReader &diffReader, int zoom,
      flyem::EBodyType bodyType);

  std::vector<ZSwcTree*> makeDiffBodyModel(
      const ZIntPoint &pt, ZDvidReader &diffReader,
      int zoom, flyem::EBodyType bodyType);

  void updateDvidInfo();

  void addBodyFunc(uint64_t bodyId, const QColor &color, int resLevel);
  void addBodyMeshFunc(uint64_t bodyId, const QColor &color, int resLevel);

  void removeBodyFunc(uint64_t bodyId, bool removingAnnotation);
  void updateBodyFunc(uint64_t bodyId, ZStackObject *bodyObject);
//  void updateBodyMeshFunc(uint64_t bodyId, ZMesh *mesh);

  void connectSignalSlot();
  void updateBodyFunc();

  void processBodySetBuffer();

  QMap<uint64_t, BodyEvent> makeEventMap(bool synced, QSet<uint64_t> &bodySet);
  QMap<uint64_t, BodyEvent> makeEventMapUnsync(QSet<uint64_t> &bodySet);

  bool synapseLoaded(uint64_t bodyId) const;
  void addSynapse(
      std::vector<ZPunctum*> &puncta,
      uint64_t bodyId, const std::string &source, double radius,
      const QColor &color);

  template<typename T>
  T* recoverFromGarbage(const std::string &source);

  ZSwcTree *getBodyQuickly(uint64_t bodyId);
  BodyEvent makeMultresBodyEvent(
      uint64_t bodyId, int resLevel, const QColor &color);

  ZDvidReader& getBodyReader();
  void updateBodyModelSelection();

  ZStackObject::EType getBodyObjectType() const;

signals:
  void todoVisibleChanged();
  void bodyMeshLoaded();
  void bodyMeshesAdded();

  void meshArchiveLoadingStarted();
  void meshArchiveLoadingProgress(float fraction);
  void meshArchiveLoadingEnded();

private slots:
//  void updateBody();
  void processEvent();
  void processEvent(const BodyEvent &event);
  void clearGarbage();

private:
  void processEventFunc(const BodyEvent &event);
  ZSwcTree* recoverFullBodyFromGarbage(
      uint64_t bodyId, int resLevel);
  ZMesh* recoverMeshFromGarbage(uint64_t bodyId, int resLevel);
  int getMinResLevel() const;
  void removeDiffBody();

  ZStackObject* takeObjectFromCache(
      ZStackObject::EType type, const std::string &source);

private:
  QSet<uint64_t> m_bodySet;
  flyem::EBodyType m_bodyType = flyem::BODY_FULL;
  QSet<uint64_t> m_selectedBodySet;

  bool m_quitting = false;
  bool m_showingSynapse = true;
  bool m_showingTodo = true;
  bool m_nodeSeeding = false;
  bool m_syncyingBodySelection = false;
//  QSet<uint64_t> m_bodySetBuffer;
//  bool m_isBodySetBufferProcessed;

  ZDvidTarget m_dvidTarget;
  ZDvidReader m_dvidReader;
  ZDvidWriter m_dvidWriter;
  ZDvidReader m_bodyReader;

  ZDvidInfo m_dvidInfo;

  ZThreadFutureMap m_futureMap;
  QTimer *m_timer;
  QTimer *m_garbageTimer;
  QTime m_objectTime;

  ZSharedPointer<ZStackDoc> m_dataDoc;

//  QList<ZStackObject*> m_garbageList;
  QMap<ZStackObject*, ObjectStatus> m_garbageMap;
  ZStackObjectGroup m_objCache;
  int m_objCacheCapacity;
  QMap<uint64_t, int> m_bodyUpdateMap;
//  QSet<uint64_t> m_unrecycableSet;

  bool m_garbageJustDumped = false;

  QQueue<BodyEvent> m_eventQueue;

  QMutex m_eventQueueMutex;
  QMutex m_garbageMutex;

  std::map<uint64_t, std::vector<uint64_t>> m_tarIdToMeshIds;

  bool m_limitGarbageLifetime = true;

  const static int OBJECT_GARBAGE_LIFE;
  const static int OBJECT_ACTIVE_LIFE;
  const static int MAX_RES_LEVEL;
};

template <typename InputIterator>
void ZFlyEmBody3dDoc::addBodyChangeEvent(
    const InputIterator &first, const InputIterator &last)
{
  std::cout << "Locking mutex ..." << std::endl;
  QMutexLocker locker(&m_eventQueueMutex);

  QSet<uint64_t> bodySet = m_bodySet;
  QMap<uint64_t, ZFlyEmBody3dDoc::BodyEvent> actionMap = makeEventMap(
        false, bodySet);

//  m_eventQueue.clear();

  QSet<uint64_t> newBodySet;
  for (InputIterator iter = first; iter != last; ++iter) {
    uint64_t bodyId = *iter;
    newBodySet.insert(bodyId);
  }

  for (QSet<uint64_t>::const_iterator iter = m_bodySet.begin();
       iter != m_bodySet.end(); ++iter) {
    uint64_t bodyId = *iter;
    if (!newBodySet.contains(bodyId)) {
      addEvent(BodyEvent::ACTION_REMOVE, bodyId, 0, NULL);
    }
  }

//  QList<BodyEvent> oldEventList;
  for (QMap<uint64_t, ZFlyEmBody3dDoc::BodyEvent>::iterator
       iter = actionMap.begin(); iter != actionMap.end(); ++iter) {
    if (newBodySet.contains(iter.key())) {
      if (iter.value().getAction() != BodyEvent::ACTION_REMOVE) {
        //In the new body set had the bodyID and not remove, add event
        addEvent(iter.value());
      } else {
        addEvent(BodyEvent::ACTION_ADD, iter.key(), 0, NULL);
      }
    }
  }

  for (InputIterator iter = first; iter != last; ++iter) {
    uint64_t bodyId = *iter;
    if (!actionMap.contains(bodyId)) { //If the action map has no such body id
      addEvent(BodyEvent::ACTION_ADD, bodyId, 0, NULL);
    }
  }
}

#endif // ZFLYEMBODY3DDOC_H
