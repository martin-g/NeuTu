#ifndef ZDVIDGRAYSLICE_H
#define ZDVIDGRAYSLICE_H

#include <memory>
#include <unordered_map>

#include "zstackobject.h"
#include "zimage.h"
//#include "zdvidreader.h"
#include "zstackviewparam.h"
#include "zarbsliceviewparam.h"
#include "zpixmap.h"
#include "zcontrastprotocol.h"
#include "zuncopyable.h"
#include "vis2d/zslicecanvas.h"

//#include "zdvidtarget.h"

class ZRect2d;
class ZDvidReader;
class ZStack;
class ZDvidDataSliceHelper;
class ZDvidTarget;
class ZTask;
class ZStackDoc;
//class ZStackViewParam;

class ZDvidGraySlice : public ZStackObject, ZUncopyable
{
public:
  ZDvidGraySlice();
  ~ZDvidGraySlice();

  static ZStackObject::EType GetType() {
    return ZStackObject::EType::DVID_GRAY_SLICE;
  }

  bool display_inner(
      QPainter *painter, const DisplayConfig &config) const override;

  void setDvidTarget(const ZDvidTarget &target);

  const ZDvidReader& getDvidReader() const;
  const ZDvidReader& getWorkDvidReader() const;
  const ZDvidTarget& getDvidTarget() const;

  /*!
   * \brief Update the view parameter
   *
   * This function will trigger data update if the current data is out of sync.
   */
  bool update(const ZStackViewParam &viewParam);

  void printInfo() const;

//  int getWidth(int viewId) const;
//  int getHeight(int viewId) const;
//  int getZoom() const;

  int getActiveScale(int viewId) const;

  ZCuboid getBoundBox(int viewId) const override;

//  ZStackViewParam getViewParam(int viewId) const;
  ZIntCuboid getDataRange() const;

  void forEachViewParam(std::function<void(const ZStackViewParam &param)> f);

//  void setZoom(int zoom);
  void setContrastProtocol(const ZContrastProtocol &cp);
  void updateContrast(bool highContrast);
  void updateContrast(const ZJsonObject &obj);

  /*!
   * \brief Check if the slice has any low-resolution region.
   */
  bool hasLowresRegion(int viewId) const;

  void setCenterCut(int width, int height);

  /*
  const ZPixmap& getPixmap() const {
    return m_pixmap;
  }
  */
  /*
  const QImage getImage() const {
    return m_imageCanvas.toImage();
  }
  */

//  void setArbitraryAxis(const ZPoint &v1, const ZPoint &v2);

  bool highResUpdateNeeded(int viewId) const;
  void processHighResParam(
      int viewId,
      std::function<void(
        const ZStackViewParam &/*viewParam*/, int /*zoom*/,
        int /*centerCutX*/, int /*centerCutY*/,
        bool /*usingCenterCut*/)> f) const;

  bool consume(ZStack *stack, const ZStackViewParam &viewParam,
               int zoom, int centerCutX, int centerCutY, bool usingCenterCut);
  bool containedIn(const ZStackViewParam &viewParam, int zoom,
                   int centerCutX, int centerCutY, bool centerCut) const;
  ZTask* makeFutureTask(ZStackDoc *doc, int viewId);
  void trackViewParam(const ZStackViewParam &viewParam);

public: //for testing
  void saveImage(const std::string &path, int viewId);
  void savePixmap(const std::string &path, int viewId);
  void _forceUpdate(const ZStackViewParam &viewParam);
//  void test();

private:
  void clear();

//  void updateImage(
//      const ZStack *stack, neutu::EAxis axis,
//      const ZAffinePlane &ap, int zoom, int viewId);
  void updateImage(
      const ZStack *stack, const ZStackViewParam &viewParam,
      int zoom, int centerCutX, int centerCutY, bool usingCenterCut);
  void forceUpdate(const ZStackViewParam &viewParam);
  void updateContrast();

  template<typename T>
  int updateParam(T *param);

  /*!
   * \brief Check if the regions of the image and the slice are consistent.
   */
//  bool isRegionConsistent() const;

  const ZDvidDataSliceHelper* getHelper() const {
    return m_helper.get();
  }
  ZDvidDataSliceHelper* getHelper() {
    return m_helper.get();
  }

  struct DisplayBuffer {
//    DisplayBuffer() {}
//    DisplayBuffer(bool valid) {
//      m_isValid = valid;
//    }

    ZSliceCanvas m_imageCanvas;
    ZImage m_image;
//    bool m_isValid = true;
  };

  std::shared_ptr<DisplayBuffer> getDisplayBuffer(int viewId) const;
  ZSliceCanvas& getImageCanvas(int viewId) const;
  ZImage& getImage(int viewId) const;

private:
//  ZSliceCanvas m_imageCanvas;
//  ZImage m_image; //Todo: need to replace ZImage type with a more concise one
  mutable std::unordered_map<int, std::shared_ptr<DisplayBuffer>> m_displayBufferMap;
  mutable std::mutex m_displayBufferMutex;

  bool m_usingContrastProtocol = false;
  ZContrastProtocol m_contrastProtocol;

  std::unique_ptr<ZDvidDataSliceHelper> m_helper;
};

#endif // ZDVIDGRAYSLICE_H
