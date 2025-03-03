#ifndef ZFLYEMBODYSTATUSTEST_H
#define ZFLYEMBODYSTATUSTEST_H

#include "ztestheader.h"

#include <QColor>

#include "flyem/zflyembodystatus.h"
#include "zjsonparser.h"
#include "zjsonobjectparser.h"

#ifdef _USE_GTEST_

TEST(ZFlyEmBodyStatus, Property)
{
  ZFlyEmBodyStatus status("Test");
  ASSERT_FALSE(status.isExpertStatus());
  ASSERT_FALSE(status.isFinal());
  ASSERT_EQ(999, status.getPriority());
  ASSERT_EQ("Test", status.getName());

  status.setExpert(true);
  status.setFinal(true);
  status.setPriority(1);

  ASSERT_TRUE(status.isExpertStatus());
  ASSERT_TRUE(status.isFinal());
  ASSERT_EQ(1, status.getPriority());

  ASSERT_TRUE(status.isMergable());
  status.setMergable(false);
  ASSERT_FALSE(status.isMergable());

  ASSERT_FALSE(status.presevingId());
  status.preseveId(true);
  ASSERT_TRUE(status.presevingId());
}

TEST(ZFlyEmBodyStatus, Json)
{
  ZFlyEmBodyStatus status;

  ZJsonObject json;
  json.setEntry(ZFlyEmBodyStatus::KEY_EXPERT, true);
  json.setEntry(ZFlyEmBodyStatus::KEY_FINAL, true);
  json.setEntry(ZFlyEmBodyStatus::KEY_PRIORITY, 1);
  json.setEntry(ZFlyEmBodyStatus::KEY_MERGABLE, false);
  json.setEntry(ZFlyEmBodyStatus::KEY_PRESERVING_ID, true);
  json.setEntry(ZFlyEmBodyStatus::KEY_COLOR, "#FF00FF00");

  status.loadJsonObject(json);
  ASSERT_TRUE(status.isExpertStatus());
  ASSERT_TRUE(status.isFinal());
  ASSERT_EQ(1, status.getPriority());
  ASSERT_FALSE(status.isMergable());
  ASSERT_TRUE(status.presevingId());
  ASSERT_EQ("#FF00FF00", status.getColorCode());

  QColor color(status.getColorCode().c_str());
  ASSERT_EQ(QColor(0, 255, 0, 255), color);

  status.reset();
  ASSERT_FALSE(status.isExpertStatus());
  ASSERT_FALSE(status.isFinal());
  ASSERT_EQ(999, status.getPriority());
  ASSERT_EQ("", status.getName());
  ASSERT_TRUE(status.isMergable());
  ASSERT_FALSE(status.presevingId());
  ASSERT_TRUE(status.getColorCode().empty());
}

#endif


#endif // ZFLYEMBODYSTATUSTEST_H
