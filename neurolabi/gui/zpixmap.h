#ifndef ZPIXMAP_H
#define ZPIXMAP_H

#include <QPixmap>
#include <QRect>
#include <QBitmap>
#include <QFutureWatcher>

#include "zsttransform.h"
#include "neutube.h"

class ZPixmap : public QPixmap
{
public:
  ZPixmap();
  ZPixmap(const QSize &size);
  ZPixmap(int width, int height);
  ~ZPixmap();

  const ZStTransform& getTransform() const;
  void setTransform(const ZStTransform &transform);
  void setScale(double sx, double sy);
  void setOffset(double dx, double dy);

  const ZStTransform& getProjTransform() const;
  ZStTransform& getProjTransform();
  void updateProjTransform(const QRect &viewPort, const QRectF &newProjRegion);

  void cleanUp();
  void clean(const QRect &rect);


  inline bool isVisible() const {
    return m_isVisible;
  }

  void setVisible(bool visible) {
    m_isVisible = visible;
  }

  QRectF getActiveArea(NeuTube::ECoordinateSystem coord) const;
  bool isFullyActive() const;
  void matchProj();

private:
  void cleanFunc(QPixmap *pixmap);

private:
  ZStTransform m_objTransform; //Transform from world coordinates to image coordinates
  ZStTransform m_projTransform; //Transform from image coordinates to screen coordinates
  QRectF m_activeArea; //Active area in the world coordiantes
  bool m_isVisible;

  QPixmap m_cleanBuffer;
  QBitmap m_cleanMask;

//  QFuture<void> m_cleanFuture;
  QFutureWatcher<void> *m_cleanWatcher;
};

#endif // ZPIXMAP_H
