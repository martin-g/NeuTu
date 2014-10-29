#ifndef ZRECT2D_H
#define ZRECT2D_H

#include "zstackobject.h"

class ZRect2d : public ZStackObject
{
public:
  ZRect2d();
  ZRect2d(int x0, int y0, int width, int height);
  virtual ~ZRect2d() {}

public:
  virtual void display(ZPainter &painter, int slice, Display_Style option) const;
  virtual const std::string& className() const;
  bool isSliceVisible(int z) const;
  inline void setPenetrating(bool p) {
    m_isPenetrating = p;
  }

  bool isValid() const;

  /*!
   * \brief Set the last corner of the rectable
   */
  void setLastCorner(int x, int y);
  void setFirstCorner(int x, int y);

private:
  int m_x0;
  int m_y0;
  int m_width;
  int m_height;
  int m_z;
  bool m_isPenetrating;
};

#endif // ZRECT2D_H
