#ifndef ZSTACKOBJECTINFO_H
#define ZSTACKOBJECTINFO_H

#include <QHash>

#include "zstackobject.h"

class ZStackObject;

class ZStackObjectInfo
{
public:
  ZStackObjectInfo();


  typedef uint64_t TState;

  static const TState STATE_UNKNOWN = 0;
  static const TState STATE_MODIFIED = BIT_FLAG(1);
  static const TState STATE_ADDED = BIT_FLAG(2);
  static const TState STATE_REMOVED = BIT_FLAG(3);
  static const TState STATE_VISIBITLITY_CHANGED = BIT_FLAG(4);
  static const TState STATE_COLOR_CHANGED = BIT_FLAG(5);
  static const TState STATE_ROLE_CHANGED = BIT_FLAG(6);

  void setType(ZStackObject::EType type) {
    m_type = type;
  }

  void setTarget(ZStackObject::ETarget target) {
    m_target = target;
  }

  void setRole(const ZStackObjectRole &role) {
    m_role = role;
  }

  void set(const ZStackObject &obj);

  ZStackObject::EType getType() const {
    return m_type;
  }

  ZStackObject::ETarget getTarget() const {
    return m_target;
  }

  const ZStackObjectRole& getRole() const{
    return m_role;
  }

  bool operator == (const ZStackObjectInfo &info) const;

  void print() const;
  std::string toString() const;

private:
  ZStackObject::EType m_type = ZStackObject::TYPE_UNIDENTIFIED;
  ZStackObject::ETarget m_target = ZStackObject::TARGET_NULL;
  ZStackObjectRole m_role;
};

uint qHash(const ZStackObjectInfo &info);

class ZStackObjectInfoSet : public QHash<ZStackObjectInfo, ZStackObjectInfo::TState>
{
public:
  ZStackObjectInfoSet();

  bool contains(ZStackObject::EType type) const;
  bool contains(ZStackObject::ETarget target) const;
  bool contains(ZStackObjectRole::TRole role) const;
  bool contains(const ZStackObjectInfo &info) const;

  QSet<ZStackObject::EType> getType() const;
  QSet<ZStackObject::ETarget> getTarget() const;


  void add(const ZStackObject &obj);

  void add(ZStackObject::ETarget target);
  void add(ZStackObject::EType type,
           ZStackObjectInfo::TState state = ZStackObjectInfo::STATE_UNKNOWN);
  void add(ZStackObjectRole::TRole role);
  void add(const ZStackObjectInfo &info);
  void add(const QSet<ZStackObject::ETarget> &targetSet);
  void add(const ZStackObjectInfo &info, ZStackObjectInfo::TState state);

  void print() const;
};


#endif // ZSTACKOBJECTINFO_H
