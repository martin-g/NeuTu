#ifndef ZDVIDLABELSLICE_H
#define ZDVIDLABELSLICE_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include <QCache>
#include <QMutex>

#include "zuncopyable.h"
#include "neutube.h"
#include "zstackobject.h"
#include "zdvidtarget.h"
#include "zobject3dscan.h"
#include "zobject3dscanarray.h"
#include "zimage.h"
#include "zselector.h"
#include "vis2d/zslicecanvas.h"

#include "flyem/zflyembodycolorscheme.h"
#include "flyem/zflyembodymerger.h"

class QColor;
class ZArray;
class ZPixmap;
class ZDvidDataSliceHelper;
class ZStackViewParam;
class ZArbSliceViewParam;
class ZTask;
class ZStackDoc;
class ZDvidDataSliceTaskFactory;
class ZDvidReader;
class ZFlyEmBodyIdColorScheme;

class ZDvidLabelSlice : public ZStackObject, ZUncopyable
{
public:
  ZDvidLabelSlice();
  ZDvidLabelSlice(int maxWidth, int maxHeight);
  ~ZDvidLabelSlice();

  static ZStackObject::EType GetType() {
    return ZStackObject::EType::DVID_LABEL_SLICE;
  }

  void setOpacity(double opacity);
  double getOpacity() const;
  void setMaxSize(const ZStackViewParam &viewParam, int maxWidth, int maxHeight);

  bool update(const ZStackViewParam &viewParam);

  void setUpdatePolicy(neutu::EDataSliceUpdatePolicy policy);

  void updateFullView(const ZStackViewParam &viewParam);

  void setSliceAxis(neutu::EAxis sliceAxis);

  bool display_inner(
      QPainter *painter, const DisplayConfig &config) const override;
  bool isVisible_inner(const DisplayConfig &) const override;

  void trackViewParam(const ZStackViewParam &viewParam);

  void setDvidTarget(const ZDvidTarget &target);

  bool hit(double x, double y, double z, int viewId) override;
  void processHit(ESelection s) override;

  void selectHit(bool appending = false);
//  void selectLabel(uint64_t bodyId, bool appending = false);

  void deselectAll();
  void toggleHitSelection(bool appending = false);
  void clearSelection();
  void clearSelectionWithExclusion(const std::set<uint64_t> excluded);

  bool isSelectionFrozen() const { return m_selectionFrozen; }
  void freezeSelection(bool on) { m_selectionFrozen = on; }

  bool isSupervoxel() const;
  bool hasType(neutu::EBodyLabelType type) const;

  void setSelection(
      const std::set<uint64_t> &selected, neutu::ELabelSource labelType);
  void addSelection(uint64_t bodyId, neutu::ELabelSource labelType);
  void xorSelection(uint64_t bodyId, neutu::ELabelSource labelType);
  void removeSelection(uint64_t bodyId, neutu::ELabelSource labelType);

  template <typename InputIterator>
  void addSelection(const InputIterator &begin, const InputIterator &end,
                    neutu::ELabelSource labelType);

  template<template<class...> class Container>
  void addSelection(
      const Container<uint64_t> &bodyList, neutu::ELabelSource labelType);

  template <typename InputIterator>
  void setSelection(const InputIterator &begin, const InputIterator &end,
                    neutu::ELabelSource labelType);
  template<template<class...> class Container>
  void setSelection(
      const Container<uint64_t> &bodyList, neutu::ELabelSource labelType);

  /*! xor selection on a group of bodies
   *
   * Note that when \a labelType is ORIGINAL, the labels will be mapped to the
   * final labels and then apply xor selection on the mapped labels in the
   * MAPPED mode in order to avoid partial selection of a merged body.
   */
  template <typename InputIterator>
  void xorSelection(const InputIterator &begin, const InputIterator &end,
                    neutu::ELabelSource labelType);

  template<template<class...> class Container>
  void xorSelection(
      const Container<uint64_t> &bodyList, neutu::ELabelSource labelType);

  template <typename InputIterator>
  void xorSelectionGroup(const InputIterator &begin, const InputIterator &end,
                         neutu::ELabelSource labelType);

  // The difference between xorSelection and xorSelectionGroup is that
  // xorSelection updates selection on the input group of bodies one by one
  // independently, while xorSelectionGroup treat them as whole. In
  // xorSelectionGroup, if any body in the input is not selected, then the whole
  // group is treated as unselected, meaning that xor leads to seleting all
  // the bodies in the group. Only when all the bodies are selected in the group,
  // xorSelectionGroup deselects all of them. Another difference between them
  // is how they treat original labels. xorSelection will map every original
  // label to its final label and operate on the final label to keep the integrity
  // of body merges, while xorSelectionGroup treats a original label as it is.
  template<template<class...> class Container>
  void xorSelectionGroup(
      const Container<uint64_t> &bodyList, neutu::ELabelSource labelType);


  inline const std::set<uint64_t>& getSelectedOriginal() const {
    return m_selectedOriginal;
  }

  std::set<uint64_t> getSelected(
      neutu::ELabelSource labelType, bool decoded = false) const;

  bool isBodySelected(uint64_t bodyId, neutu::ELabelSource labelType) const;

  void setBodyMerger(ZFlyEmBodyMerger *bodyMerger);
  void updateLabelColor();

  std::shared_ptr<ZFlyEmBodyColorScheme> getColorScheme() const;

  QColor getLabelColor(uint64_t label, neutu::ELabelSource labelType) const;
  QColor getLabelColor(int64_t label, neutu::ELabelSource labelType) const;

  bool setLabelColor(uint64_t label, const QColor &color, size_t rank);

  /*!
   * \brief Set the label color by specifing a string code.
   *
   * When the code is non-empty, it is taken as the name of color passed to
   * QColor directly; otherwise it means no special color for the label and
   * if it has one, the existing one will be removed.
   */
  bool setLabelColor(uint64_t label, const QString &colorCode, size_t rank);
  bool setLabelColor(uint64_t label, const char *colorCode, size_t rank);
  bool setLabelColor(uint64_t label, const std::string &colorCode, size_t rank);

  bool removeLabelColor(uint64_t label, size_t rank);

  bool resetLabelColor(size_t rank);

  uint64_t getMappedLabel(const ZObject3dScan &obj) const;
  uint64_t getMappedLabel(uint64_t label) const;
  uint64_t getMappedLabel(
      uint64_t label, neutu::ELabelSource labelType) const;

  std::set<uint64_t> getOriginalLabelSet(uint64_t mappedLabel) const;

  bool setSelectedLabelColor(const QColor &color);
  bool resetSelectedLabelColor();

  uint64_t getHitLabel() const;
  std::set<uint64_t> getHitLabelSet() const;
  uint64_t getLabelAt(int x, int y, int z, neutu::ELabelSource source) const;
  std::set<uint64_t> getLabelIdSet(
      const std::vector<ZIntPoint> &ptArray, neutu::ELabelSource source) const;

//  const ZStackViewParam& getViewParam() const;
  int getCurrentZ() const;
//  QRect getDataRect() const;
  ZIntCuboid getDataRange() const;

  void mapSelection();

  void forceUpdate(bool ignoringHidden);

  void setCenterCut(int width, int height);

  //Selection events
  void startSelection();
  void endSelection();

  const ZSelector<uint64_t>& getSelector() const {
    return m_selector;
  }

  ZSelector<uint64_t>& getSelector() {
    return m_selector;
  }

  std::set<uint64_t> getRecentSelected(neutu::ELabelSource labelType) const;
  std::set<uint64_t> getRecentDeselected(neutu::ELabelSource labelType) const;

  void setCustomColorMap(const std::shared_ptr<ZFlyEmBodyColorScheme> &colorMap);
  void removeCustomColorMap();
  bool hasCustomColorMap() const;
  void assignColorMap();

  /*
  ZImage* getPaintBuffer() {
    return m_paintBuffer;
  }
  */

  int64_t getReadingTime() const;

  bool refreshReaderBuffer();

  void paintBuffer(int viewId);
  void invalidatePaintBuffer(int viewId);
  bool isPaintBufferValid(int viewId) const;

  void paintAllBuffer();
  void invalidateAllPaintBuffer();

//  QRect getDataRect(const ZStackViewParam &viewParam) const;

  bool consume(ZArray *array, const ZStackViewParam &viewParam,
               int zoom, int centerCutX, int centerCutY, bool usingCenterCut);
  bool containedIn(const ZStackViewParam &viewParam, int zoom,
                   int centerCutX, int centerCutY, bool usingCenterCut) const;
//  bool needsUpdate(const ZStackViewParam &viewParam, int zoom,
//                   int centerCutX, int centerCutY, bool usingCenterCut) const;
  bool highResUpdateNeeded(int viewId) const;
  void processHighResParam(
      int viewId,
      std::function<void(
        const ZStackViewParam &/*viewParam*/, int /*zoom*/,
        int /*centerCutX*/, int /*centerCutY*/,
        bool /*usingCenterCut*/, bool /*sv*/)> f) const;
  ZTask* makeFutureTask(ZStackDoc *doc, int viewId);
  void setTaskFactory(std::unique_ptr<ZDvidDataSliceTaskFactory> &&factory);

  void allowBlinking(bool on);

  void saveCanvas(const std::string &path, int viewId);

  const ZDvidReader& getDvidReader() const;
  const ZDvidReader& getWorkDvidReader() const;

public:
  void _forceUpdate(
      const ZStackViewParam &viewParam, bool ignoringHidden) {
    forceUpdate(viewParam, ignoringHidden);
  }

private:
  const ZDvidTarget& getDvidTarget() const;// { return m_dvidTarget; }

  void forceUpdate(
      const ZStackViewParam &viewParam, bool ignoringHidden);
//  void forceUpdate(const QRect &viewPort, int z, int zoom);
//  void forceUpdate(const ZArbSliceViewParam &viewParam, int zoom);
//  void forceUpdate(bool ignoringHidden);
  //void updateLabel(const ZFlyEmBodyMerger &merger);
  void init(int maxWidth, int maxHeight,
            neutu::EAxis sliceAxis = neutu::EAxis::Z);
  QColor getCustomColor(uint64_t label) const;

  void paintBufferUnsync(int viewId);
//  void remapId(ZArray *label);
//  void remapId();

//  void remapId(uint64_t *array, const uint64_t *originalArray, uint64_t v);
//  void remapId(uint64_t *array, const uint64_t *originalArray, uint64_t v,
//               std::set<uint64_t> &selected);
//  void remapId(uint64_t *array, const uint64_t *originalArray, uint64_t v,
//               const ZFlyEmBodyMerger::TLabelMap &bodyMap);
//  void remapId(uint64_t *array, const uint64_t *originalArray, uint64_t v,
//               std::set<uint64_t> &selected,
//               const ZFlyEmBodyMerger::TLabelMap &bodyMap);

  void updateRgbTable();

  ZFlyEmBodyMerger::TLabelMap getLabelMap() const;
  void clearLabelData(int viewId);

//  void updatePixmap(ZPixmap *pixmap) const;
  void updatePaintBuffer(int viewId);
  void updateCanvas(int viewId);
//  void setTransform(ZImage *image) const;

  const ZDvidDataSliceHelper* getHelper() const {
    return m_helper.get();
  }
  ZDvidDataSliceHelper* getHelper() {
    return m_helper.get();
  }

  void setPreferredUpdatePolicy(neutu::EDataSliceUpdatePolicy policy);

  bool isPaintBufferAllocNeeded(int width, int height, int viewId) const;
  bool isPaintBufferUpdateNeeded(int viewId) const;

  int getFirstZoom(const ZStackViewParam &viewParam) const;

//  bool hasValidPaintBuffer() const;

  std::shared_ptr<ZFlyEmBodyColorScheme> getBaseColorScheme() const;
  void updateColorField(int viewId);

  ZFlyEmBodyIdColorScheme* getIndividualColorScheme(size_t rank) const;

  struct DisplayBuffer {
    std::shared_ptr<ZArray> m_labelArray;
//    std::shared_ptr<ZArray> m_mappedLabelArray;
    std::vector<uint32_t> m_colorField;
    std::shared_ptr<ZImage> m_paintBuffer;
    ZSliceCanvas m_imageCanvas;
//    std::shared_ptr<ZDvidDataSliceHelper> m_helper;
    bool m_isPaintBufferValid = false;
  };

  std::shared_ptr<DisplayBuffer> getDisplayBuffer(int viewId) const;
  ZSliceCanvas& getImageCanvas(int viewId) const;
  std::shared_ptr<ZArray> getLabelArray(int viewId) const;
//  std::shared_ptr<ZArray> getMappedLabelArray(int viewId) const;
  std::shared_ptr<ZImage> getPaintBuffer(int viewId) const;
  const std::vector<uint32_t>& getColorField(int viewId) const;
  std::vector<uint32_t>& getColorField(int viewId);

//  std::shared_ptr<ZDvidDataSliceHelper> getHelper(int viewId) const;

private:
  mutable std::unordered_map<int, std::shared_ptr<DisplayBuffer>> m_displayBufferMap;
  mutable std::mutex m_displayBufferMutex;

  QMutex m_updateMutex; //Review-TZ: does not seem used properly

  ZObject3dScanArray m_objArray;

  double m_opacity = 0.3;

  std::shared_ptr<ZFlyEmBodyColorScheme> m_defaultColorSheme;
  std::vector<std::shared_ptr<ZFlyEmBodyIdColorScheme>> m_individualColorScheme;
  std::shared_ptr<ZFlyEmBodyColorScheme> m_customColorScheme;

  std::vector<int> m_rgbTable;

  std::unordered_map<ZIntPoint, uint64_t> m_cachedLabel;
  uint64_t m_hitLabel; //Mapped label
  std::set<uint64_t> m_selectedOriginal;
  ZFlyEmBodyMerger *m_bodyMerger;

  std::set<uint64_t> m_prevSelectedOriginal;
  ZSelector<uint64_t> m_selector; //original labels


  std::unique_ptr<ZDvidDataSliceHelper> m_helper;
  std::unique_ptr<ZDvidDataSliceTaskFactory> m_taskFactory;

  bool m_selectionFrozen;
//  bool m_multiResUpdate = true;
};

template <typename InputIterator>
void ZDvidLabelSlice::xorSelection(
    const InputIterator &begin, const InputIterator &end,
    neutu::ELabelSource labelType)
{
  std::set<uint64_t> labelSet;

  for (InputIterator iter = begin; iter != end; ++iter) {
    labelSet.insert(getMappedLabel(*iter, labelType));
  }

  for (std::set<uint64_t>::const_iterator iter  = labelSet.begin();
       iter != labelSet.end(); ++iter) {
    xorSelection(*iter, neutu::ELabelSource::MAPPED);
  }
}

template<template<class...> class Container>
void ZDvidLabelSlice::xorSelection(
    const Container<uint64_t> &bodyList, neutu::ELabelSource labelType)
{
  xorSelection(bodyList.begin(), bodyList.end(), labelType);
}

template <typename InputIterator>
void ZDvidLabelSlice::addSelection(
    const InputIterator &begin, const InputIterator &end,
    neutu::ELabelSource labelType)
{
  std::set<uint64_t> labelSet;

  for (InputIterator iter = begin; iter != end; ++iter) {
    labelSet.insert(getMappedLabel(*iter, labelType));
  }

  for (std::set<uint64_t>::const_iterator iter  = labelSet.begin();
       iter != labelSet.end(); ++iter) {
    addSelection(*iter, neutu::ELabelSource::MAPPED);
  }
}

template<template<class...> class Container>
void ZDvidLabelSlice::addSelection(
    const Container<uint64_t> &bodyList, neutu::ELabelSource labelType)
{
  addSelection(bodyList.begin(), bodyList.end(), labelType);
}

template <typename InputIterator>
void ZDvidLabelSlice::setSelection(
    const InputIterator &begin, const InputIterator &end,
    neutu::ELabelSource labelType)
{
  clearSelection();
  addSelection(begin, end, labelType);
  paintAllBuffer();
}

template<template<class...> class Container>
void ZDvidLabelSlice::setSelection(
    const Container<uint64_t> &bodyList, neutu::ELabelSource labelType)
{
  setSelection(bodyList.begin(), bodyList.end(), labelType);
}

template <typename InputIterator>
void ZDvidLabelSlice::xorSelectionGroup(
    const InputIterator &begin, const InputIterator &end,
    neutu::ELabelSource labelType)
{
  std::set<uint64_t> labelSet; //original label set

  switch (labelType) {
  case neutu::ELabelSource::MAPPED:
    for (InputIterator iter = begin; iter != end; ++iter) {
//      uint64_t label = getMappedLabel(*iter, labelType);
      std::set<uint64_t> sourceLabel = getOriginalLabelSet(*iter);
      labelSet.insert(sourceLabel.begin(), sourceLabel.end());
    }
    break;
  case neutu::ELabelSource::ORIGINAL:
    for (InputIterator iter = begin; iter != end; ++iter) {
      uint64_t label = *iter;
      labelSet.insert(label);
    }
    break;
  }

  bool selecting = false;
  for (std::set<uint64_t>::const_iterator iter = labelSet.begin();
       iter != labelSet.end(); ++iter) {
    uint64_t label = *iter;
    if (m_selectedOriginal.count(label) == 0) { //any body has not been selected
      selecting = true;
      break;
    }
  }

  if (selecting) {
    for (std::set<uint64_t>::const_iterator iter  = labelSet.begin();
         iter != labelSet.end(); ++iter) {
      m_selectedOriginal.insert(*iter);
    }
  } else {
    for (std::set<uint64_t>::const_iterator iter  = labelSet.begin();
         iter != labelSet.end(); ++iter) {
      m_selectedOriginal.erase(*iter);
    }
  }
}

template<template<class...> class Container>
void ZDvidLabelSlice::xorSelectionGroup(
    const Container<uint64_t> &bodyList, neutu::ELabelSource labelType)
{
  xorSelectionGroup(bodyList.begin(), bodyList.end(), labelType);
}

#endif // ZDVIDLABELSLICE_H
