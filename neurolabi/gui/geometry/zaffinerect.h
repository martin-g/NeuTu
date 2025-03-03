#ifndef ZAFFINERECT_H
#define ZAFFINERECT_H

#include "zaffineplane.h"
#include "zlinesegment.h"

class ZAffineRect
{
public:
  ZAffineRect();

  void set(const ZPoint &offset, const ZPoint &v1, const ZPoint &v2,
           double width, double height);

  double getWidth() const;
  double getHeight() const;
  ZPoint getV1() const;
  ZPoint getV2() const;
  ZPoint getCenter() const;

  /*!
   * \brief Get a corner of the rectangle
   *
   * ^ Y
   * |
   * 1----0
   * |    |
   * |    |
   * 2----3 -> X
   */
  ZPoint getCorner(int index) const;
  ZLineSegment getSide(int index) const;

  void setCenter(const ZPoint &offset);
  void setCenter(double x, double y, double z);
  void setPlane(const ZPoint &v1, const ZPoint &v2);
  void setSize(double width, double height);

  void translate(double dx, double dy, double dz);
  void translate(const ZPoint &dv);

  /*!
   * \brief Scale a rectangle without moving its center
   */
  void scale(double su, double sv);

  ZAffinePlane getAffinePlane() const;

  friend std::ostream& operator<<(std::ostream& stream, const ZAffineRect &r);

private:
  ZAffinePlane m_ap;
  double m_width = 0;
  double m_height = 0;
};

struct ZAffineRectBuilder
{
  ZAffineRectBuilder();
  ZAffineRectBuilder(int width, int height);
  operator ZAffineRect() const;

  ZAffineRectBuilder& at(const ZPoint &center);
  ZAffineRectBuilder& on(const ZPoint &v1, const ZPoint &v2);
  ZAffineRectBuilder& withSize(int width, int height);

private:
  ZAffineRect m_ar;
};

#endif // ZAFFINERECT_H
