#ifndef ZFLYEMBODYMERGEDOC_H
#define ZFLYEMBODYMERGEDOC_H

#include "zstackdoc.h"
#include <QList>
#include <QSet>

#include "flyem/zflyembodymerger.h"
#include "zstackdoccommand.h"
#include "dvid/zdvidtarget.h"

class ZArray;

class ZFlyEmBodyMergeDoc : public ZStackDoc
{
  Q_OBJECT
public:
  explicit ZFlyEmBodyMergeDoc(ZStack *stack, QObject *parent = 0);
  ~ZFlyEmBodyMergeDoc();

public:
  void updateBodyObject();
  void mergeSelected();

  void setOriginalLabel(const ZStack *stack);

  ZFlyEmBodyMerger* getBodyMerger() {
    return &m_bodyMerger;
  }

  int64_t getSelectedBodyId() const;

  inline void setDvidTarget(const ZDvidTarget &target) {
    m_dvidTarget = target;
  }

  const ZDvidTarget& getDvidTarget() const {
    return m_dvidTarget;
  }

private:
  QList<ZObject3dScan*> extractAllObject();

signals:

public slots:
  void updateOriginalLabel(ZArray *array);
  void updateOriginalLabel(ZArray *array, QSet<uint64_t> *selected);

private:
  ZArray *m_originalLabel;
  ZFlyEmBodyMerger m_bodyMerger;
  ZDvidTarget m_dvidTarget;
};

namespace ZFlyEmBodyMergerDocCommand {
class MergeBody : public ZUndoCommand
{
public:
  MergeBody(ZStackDoc *doc, QUndoCommand *parent = NULL);
  void undo();
  void redo();

  ZFlyEmBodyMergeDoc* getCompleteDocument();

private:
  ZStackDoc *m_doc;
};
}

#endif // ZFLYEMBODYMERGEDOC_H
