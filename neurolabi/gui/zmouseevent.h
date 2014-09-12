#ifndef ZMOUSEEVENT_H
#define ZMOUSEEVENT_H

#include <qnamespace.h>
#include "zintpoint.h"
#include "zpoint.h"

class QMouseEvent;

class ZMouseEvent
{
public:
  ZMouseEvent();

  enum EAction {
    ACTION_NONE, ACTION_PRESS, ACTION_RELEASE, ACTION_MOVE, ACTION_DOUBLE_CLICK
  };

  bool isNull() const;

  void set(QMouseEvent *event, int z);
  void set(QMouseEvent *event, EAction action, int z);

  //void setStackPosition(const ZPoint &pt);
  //void setStackPosition(double x, double y, double z);

  void setRawStackPosition(const ZPoint &pt);
  void setRawStackPosition(double x, double y, double z);

  void setPressEvent(QMouseEvent *event, int z);
  void setReleaseEvent(QMouseEvent *event, int z);
  void setMoveEvent(QMouseEvent *event, int z);

  inline EAction getAction() const {
    return m_action;
  }

  /*
  inline Qt::MouseButton getButton() const {
    return m_button;
  }
  */

  /*!
   * \brief Get the buttons causing the event
   */
  inline Qt::MouseButtons getButtons() const {
    return m_buttons;
  }

  inline Qt::KeyboardModifiers getModifiers() const {
    return m_modifiers;
  }

  inline const ZIntPoint& getPosition() const {
    return m_position;
  }

  inline int getX() const {
    return getPosition().getX();
  }

  inline int getY() const {
    return getPosition().getY();
  }

  inline int getZ() const {
    return getPosition().getZ();
  }

  inline const ZPoint& getRawStackPosition() const {
    return m_rawStackPosition;
  }

private:
  //Qt::MouseButton m_button;
  Qt::MouseButtons m_buttons;
  EAction m_action;
  Qt::KeyboardModifiers m_modifiers;
  ZIntPoint m_position;
  ZPoint m_rawStackPosition;
  //ZPoint m_stackPosition;
};

#endif // ZMOUSEEVENT_H
