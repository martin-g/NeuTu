#include "zstackobject.h"

#if _QT_APPLICATION_2
#include <QThread>
#include <QCoreApplication>
#endif

#include <chrono>

#if defined(_QT_GUI_USED_)
#include "zpainter.h"
#endif

#include "common/math.h"
#include "common/utilities.h"
#include "geometry/zintcuboid.h"
#include "geometry/zcuboid.h"

//const char* ZStackObject::m_nodeAdapterId = "!NodeAdapter";
double ZStackObject::m_defaultPenWidth = 0.5;
//std::atomic<uint64_t> ZStackObject::m_currentHandle{0};

ZStackObject::ZStackObject()
{
  setSliceAxis(neutu::EAxis::Z);
  m_basePenWidth = m_defaultPenWidth;
//  m_handle = GetNextHandle();
}

ZStackObject::ZStackObject(const ZStackObject &obj)
{
  *this = obj;
}

ZStackObject::ZStackObject(ZStackObject &&obj)
{
  *this = std::move(obj);
}

ZStackObject::~ZStackObject()
{
#if _QT_APPLICATION_2
  if (QCoreApplication::instance()) {
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
      if (getType() != ZStackObject::EType::DVID_SYNAPSE &&
          getType() != ZStackObject::EType::OBJECT3D_SCAN) {
        std::cout << "Deteleted in separate threads!!!" << std::endl;
        std::cout << "Deconstructing " << this << ": " << getTypeName() << ", "
                  << getSource() << std::endl;
      }
    }
  }
#endif

#ifdef _DEBUG_2
  std::cout << "Deconstructing " << this << ": " << getTypeName() << ", "
            << getSource() << std::endl;
#endif
}

/*
uint64_t ZStackObject::GetNextHandle()
{
//  std::lock_guard<std::mutex> guard(m_handleMutex);
  return ++m_currentHandle;
}
*/

#define RETURN_TYPE_NAME(v, t) \
  if (v == EType::t) return NT_STR(t)

std::string ZStackObject::GetTypeName(EType type)
{
  RETURN_TYPE_NAME(type, UNIDENTIFIED);
  RETURN_TYPE_NAME(type, SWC);
  RETURN_TYPE_NAME(type, PUNCTUM);
  RETURN_TYPE_NAME(type, MESH);
  RETURN_TYPE_NAME(type, OBJ3D);
  RETURN_TYPE_NAME(type, STROKE);
  RETURN_TYPE_NAME(type, LOCSEG_CHAIN);
  RETURN_TYPE_NAME(type, CONN);
  RETURN_TYPE_NAME(type, OBJECT3D_SCAN);
  RETURN_TYPE_NAME(type, SPARSE_OBJECT);
  RETURN_TYPE_NAME(type, CIRCLE);
  RETURN_TYPE_NAME(type, STACK_BALL);
  RETURN_TYPE_NAME(type, STACK_PATCH);
  RETURN_TYPE_NAME(type, RECT2D);
  RETURN_TYPE_NAME(type, DVID_TILE);
  RETURN_TYPE_NAME(type, DVID_GRAY_SLICE);
  RETURN_TYPE_NAME(type, DVID_GRAY_SLICE_ENSEMBLE);
  RETURN_TYPE_NAME(type, DVID_TILE_ENSEMBLE);
  RETURN_TYPE_NAME(type, DVID_LABEL_SLICE);
  RETURN_TYPE_NAME(type, DVID_SPARSE_STACK);
  RETURN_TYPE_NAME(type, DVID_SPARSEVOL_SLICE);
  RETURN_TYPE_NAME(type, STACK);
  RETURN_TYPE_NAME(type, SWC_NODE);
  RETURN_TYPE_NAME(type, GRAPH_3D);
  RETURN_TYPE_NAME(type, PUNCTA);
  RETURN_TYPE_NAME(type, FLYEM_BOOKMARK);
  RETURN_TYPE_NAME(type, INT_CUBOID);
  RETURN_TYPE_NAME(type, LINE_SEGMENT);
  RETURN_TYPE_NAME(type, SLICED_PUNCTA);
  RETURN_TYPE_NAME(type, DVID_SYNAPSE);
  RETURN_TYPE_NAME(type, DVID_SYNAPE_ENSEMBLE);
  RETURN_TYPE_NAME(type, CUBE);
  RETURN_TYPE_NAME(type, DVID_ANNOTATION);
  RETURN_TYPE_NAME(type, FLYEM_TODO_ITEM);
  RETURN_TYPE_NAME(type, FLYEM_TODO_LIST);
  RETURN_TYPE_NAME(type, CROSS_HAIR);

  return std::to_string(neutu::EnumValue(type));
}

std::string ZStackObject::getTypeName() const
{
  return GetTypeName(getType());
}

ZStackObject* ZStackObject::clone() const
{
  return nullptr;
}

const ZStackObjectHandle &ZStackObject::getHandle() const
{
  return m_handle;
}

ZStackObject& ZStackObject::operator=(ZStackObject &&obj)
{
  *this = obj;
//  m_handle = std::move(obj.m_handle);
  m_selectionCallbacks = std::move(obj.m_selectionCallbacks);
  m_deselectionCallbacks = std::move(obj.m_deselectionCallbacks);

  return *this;
}

ZStackObject& ZStackObject::operator=(const ZStackObject &obj)
{
  m_hitProtocal = obj.m_hitProtocal;
  m_hitMap = obj.m_hitMap;
  _hit = obj._hit;
  m_color = obj.m_color;
  m_target = obj.m_target;
  m_type = obj.m_type;
  m_coordSpace = obj.m_coordSpace;
  m_basePenWidth = obj.m_basePenWidth;
  m_zScale = obj.m_zScale;
  m_source = obj.m_source;
  m_objectClass = obj.m_objectClass;
  m_uLabel = obj.m_uLabel;
  m_zOrder = obj.m_zOrder;
  m_role = obj.m_role;
  m_hitPoint = obj.m_hitPoint;
  m_sliceAxis = obj.m_sliceAxis;
  m_timeStamp = obj.m_timeStamp;

  m_visualEffect = obj.m_visualEffect;

  m_selected = obj.m_selected;
  m_isSelectable = obj.m_isSelectable;
  m_hittable = obj.m_hittable;
  m_isVisible = obj.m_isVisible;
  m_projectionVisible = obj.m_projectionVisible;
  m_usingCosmeticPen = obj.m_usingCosmeticPen;

  return *this;
}

#if 0
bool ZStackObject::display(
    QPainter * /*painter*/, int /*z*/,
    EDisplayStyle /*option*/, EDisplaySliceMode /*sliceMode*/,
    neutu::EAxis /*sliceAxis*/) const
{
  return false;
}
#endif

void ZStackObject::setLabel(uint64_t label)
{
  m_uLabel = label;
}

void ZStackObject::setSelected(bool selected)
{
  m_selected = selected;

  if(m_selected) {
    for(auto callback: m_selectionCallbacks) {
      callback(this);
    }
  } else {
    for(auto callback: m_deselectionCallbacks) {
      callback(this);
    }
  }
}

void ZStackObject::processHit(ESelection s)
{
  switch (s) {
  case ESelection::SELECT_SINGLE:
  case ESelection::SELECT_MULTIPLE:
    setSelected(true);
    break;
  case ESelection::SELECT_TOGGLE_SINGLE:
  case ESelection::SELECT_TOGGLE:
    setSelected(!isSelected());
    break;
  case ESelection::DESELECT:
    setSelected(false);
    break;
  }
}

void ZStackObject::deselect(bool recursive)
{
  setSelected(false);

  if (recursive) {
    deselectSub();
  }
}


void ZStackObject::setColor(int red, int green, int blue)
{
#if defined(_QT_GUI_USED_)
  setColor(QColor(red, green, blue, m_color.alpha()));
#else
  UNUSED_PARAMETER(red);
  UNUSED_PARAMETER(green);
  UNUSED_PARAMETER(blue);
#endif

//#if defined(_QT_GUI_USED_)
//  m_color.setRed(red);
//  m_color.setGreen(green);
//  m_color.setBlue(blue);
//#else
//  UNUSED_PARAMETER(red);
//  UNUSED_PARAMETER(green);
//  UNUSED_PARAMETER(blue);
//#endif
}

void ZStackObject::setColor(int red, int green, int blue, int alpha)
{
#if defined(_QT_GUI_USED_)
  setColor(QColor(red, green, blue, alpha));
#endif

//#if defined(_QT_GUI_USED_)
//  m_color.setRed(red);
//  m_color.setGreen(green);
//  m_color.setBlue(blue);
//  m_color.setAlpha(alpha);
//#else
//  UNUSED_PARAMETER(red);
//  UNUSED_PARAMETER(green);
//  UNUSED_PARAMETER(blue);
//  UNUSED_PARAMETER(alpha);
//#endif
}

void ZStackObject::setColor(const QColor &n) {
#if defined(_QT_GUI_USED_)
  m_color = n;
#else
  UNUSED_PARAMETER(&n);
#endif
}

void ZStackObject::setAlpha(int alpha) {
#if defined(_QT_GUI_USED_)
  setColor(QColor(m_color.red(), m_color.green(), m_color.blue(), alpha));
//  m_color.setAlpha(alpha);
#else
  UNUSED_PARAMETER(alpha);
#endif
}

int ZStackObject::getAlpha() {
#if defined(_QT_GUI_USED_)
  return m_color.alpha();
#else
  return 0;
#endif
}

double ZStackObject::getAlphaF() {
#if defined(_QT_GUI_USED_)
  return m_color.alphaF();
#else
  return 0.0;
#endif
}

double ZStackObject::getRedF() {
#if defined(_QT_GUI_USED_)
  return m_color.redF();
#else
  return 0.0;
#endif
}

double ZStackObject::getGreenF() {
#if defined(_QT_GUI_USED_)
  return m_color.greenF();
#else
  return 0.0;
#endif
}

double ZStackObject::getBlueF() {
#if defined(_QT_GUI_USED_)
  return m_color.blueF();
#else
  return 0.0;
#endif
}

bool ZStackObject::isOpaque() {
  return getAlpha() == 255;
}

const QColor& ZStackObject::getColor() const
{
  return m_color;
}

double ZStackObject::getPenWidth() const
{
  double width = m_basePenWidth;

  if (m_usingCosmeticPen) {
    width += 1.0;
  }

  return width;
}

bool ZStackObject::isSliceVisible(int /*z*/, neutu::EAxis /*axis*/) const
{
  return isVisible();
}

//void ZStackObject::viewSpaceAlignedDisplay(
//      QPainter */*painter*/, const ViewSpaceAlignedDisplayConfig &/*config*/) const
//{

//}

ZStackObject *ZStackObject::aligned(
    const ZAffinePlane &/*plane*/, neutu::EAxis /*sliceAxis*/) const
{
  return nullptr;
}

bool ZStackObject::isSliceVisible(
    int z, neutu::EAxis axis, const ZAffinePlane &/*plane*/) const
{
  return isSliceVisible(z, axis);
}

bool ZStackObject::isSliceVisible(
    const DisplayConfig &/*config*/, int /*canvasWidth*/, int /*canvasHeight*/) const
{
  return true;
}

bool ZStackObject::isSliceVisible(const DisplayConfig &/*config*/) const
{
  return true;
}

void ZStackObject::setHittable(bool on)
{
  m_hittable = on;
}

bool ZStackObject::hit(double /*x*/, double /*y*/, neutu::EAxis /*axis*/)
{
  return false;
}

bool ZStackObject::hit(double x, double y, neutu::EAxis axis, int /*viewId*/)
{
  return hit(x, y, axis);
}

bool ZStackObject::hit(double x, double y, double z, int viewId)
{
  if (m_hitMap.count(viewId) > 0) {
    return m_hitMap.at(viewId)(this, x, y, z);
  }

  return hit(x, y, z);
}

bool ZStackObject::hit(double x, double y, double z)
{
  if (m_hittable) {
    return _hit(this, x, y, z);
  }

  return false;
}

bool ZStackObject::hit(const ZIntPoint &pt)
{
  return hit(pt.getX(), pt.getY(), pt.getZ());
}

bool ZStackObject::hit(const ZIntPoint &pt, int viewId)
{
  return hit(pt.getX(), pt.getY(), pt.getZ(), viewId);
}

bool ZStackObject::hit(const ZIntPoint &dataPos, const ZIntPoint &widgetPos,
                       neutu::EAxis axis)
{
  switch (m_hitProtocal) {
  case EHitProtocol::HIT_DATA_POS:
    return hit(dataPos);
  case EHitProtocol::HIT_WIDGET_POS:
    return hitWidgetPos(widgetPos, axis);
  default:
    break;
  }

  return false;
}

bool ZStackObject::hitWidgetPos(
    const ZIntPoint &/*widgetPos*/, neutu::EAxis /*axis*/)
{
  return false;
}

void ZStackObject::setHitPoint(const ZIntPoint &pt)
{
  m_hitPoint = pt;
}

void ZStackObject::setHitFunc(
    std::function<bool(const ZStackObject*, double, double, double)> f)
{
  this->_hit = f;
}

bool ZStackObject::fromSameSource(const ZStackObject *obj) const
{
  bool sameSource = false;
  if (obj != NULL) {
    if ((getType() == obj->getType()) &&
        !getSource().empty() && !obj->getSource().empty()) {
      if (getSource() == obj->getSource()) {
        sameSource = true;
      }
    }
  }

  return sameSource;
}

bool ZStackObject::hasSameId(const ZStackObject *obj) const
{
  bool same = false;
  if (obj != NULL) {
    if (!getObjectId().empty() && !obj->getObjectId().empty()) {
      if (getObjectId() == obj->getObjectId()) {
        same = true;
      }
    }
  }

  return same;
}

bool ZStackObject::fromSameSource(
    const ZStackObject *obj1, const ZStackObject *obj2)
{
  bool sameSource = false;
  if (obj1 != NULL) {
    sameSource = obj1->fromSameSource(obj2);
  }

  return sameSource;
}

bool ZStackObject::hasSameId(const ZStackObject *obj1, const ZStackObject *obj2)
{
  bool same = false;
  if (obj1 != NULL) {
    same = obj1->hasSameId(obj2);
  }

  return same;
}

bool ZStackObject::IsSameSource(const std::string &s1, const std::string &s2)
{
  return (!s1.empty() && !s2.empty() && s1 == s2);
}

bool ZStackObject::IsSameClass(const std::string &s1, const std::string &s2)
{
  return IsSameSource(s1, s2);
}


bool ZStackObject::IsSelected(const ZStackObject *obj)
{
  if (obj != NULL) {
    return obj->isSelected();
  }

  return false;
}

void ZStackObject::removeRole(ZStackObjectRole::TRole role)
{
  m_role.removeRole(role);
}

void ZStackObject::boundBox(ZIntCuboid *box) const
{
  if (box != NULL) {
    *box = ZIntCuboid();
  }
}

ZCuboid ZStackObject::getBoundBox() const
{
  return ZCuboid();
}

ZCuboid ZStackObject::getBoundBox(int /*viewId*/) const
{
  return getBoundBox();
}

void ZStackObject::addVisualEffect(neutu::display::TVisualEffect ve)
{
  m_visualEffect |= ve;
}

void ZStackObject::removeVisualEffect(neutu::display::TVisualEffect ve)
{
  m_visualEffect &= ~ve;
}

void ZStackObject::setVisualEffect(neutu::display::TVisualEffect ve)
{
  m_visualEffect = ve;
}

bool ZStackObject::hasVisualEffect(neutu::display::TVisualEffect ve) const
{
  return m_visualEffect & ve;
}

void ZStackObject::setPrevZ(int z) const
{
  m_displayTrace.prevZ = z;
  m_displayTrace.isValid = true;
}

bool ZStackObject::display(
    QPainter *painter, const DisplayConfig &config) const
{
  if (isVisible(config)) {
    neutu::ApplyOnce ao([&]() {painter->save();}, [&]() {painter->restore();});
    painter->setRenderHint(QPainter::Antialiasing, m_usingCosmeticPen);
    return display_inner(painter, config);
  }

  return false;
}

bool ZStackObject::display_inner(
    QPainter */*painter*/, const DisplayConfig &/*config*/) const
{
  return false;
}

void ZStackObject::updateTimestamp()
{
  setTimestamp(
        std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch()).count());
}
