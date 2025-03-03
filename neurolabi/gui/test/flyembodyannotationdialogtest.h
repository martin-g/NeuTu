#ifndef FLYEMBODYANNOTATIONDIALOGTEST_H
#define FLYEMBODYANNOTATIONDIALOGTEST_H

#include <cstdlib>

#include "ztestheader.h"
#include "flyem/dialogs/flyembodyannotationdialog.h"
#include "flyem/zflyembodyannotation.h"

#ifdef _USE_GTEST_

TEST(FlyEmBodyAnnotationDialog, Basic)
{
  FlyEmBodyAnnotationDialog dlg(true, nullptr);
//  dlg.updateStatusBox();

  ZFlyEmBodyAnnotation annotation;
  annotation.loadJsonString(
        "{"
           "\"body ID\": 5901214961,"
           "\"instance\": \"test3\","
           "\"naming user\": \"zhaot\","
           "\"status\": \"Traced\","
           "\"user\": \"zhaot\""
        "}");
  dlg.loadBodyAnnotation(annotation);
  ASSERT_EQ(annotation, dlg.getBodyAnnotation())
      << annotation.brief()
      << dlg.getBodyAnnotation().brief();
  ASSERT_TRUE(annotation.hasSameUserStatus(dlg.getBodyAnnotation()));

  annotation.loadJsonString(
        "{"
           "\"body ID\": 5901214961,"
           "\"instance\": \"test3\","
           "\"status\": \"Traced\","
           "\"user\": \"" + std::string(std::getenv("USER")) +
        "\"}");
  dlg.loadBodyAnnotation(annotation);
  ASSERT_EQ(annotation, dlg.getBodyAnnotation());
  ASSERT_TRUE(annotation.hasSameUserStatus(dlg.getBodyAnnotation()));

  annotation.loadJsonString(
        "{"
           "\"body ID\": 5901214961,"
           "\"instance\": \"test3\","
           "\"status\": \"Traced\","
           "\"user\": \"test\""
        "}");
  dlg.loadBodyAnnotation(annotation);
  ASSERT_EQ(annotation, dlg.getBodyAnnotation());
  ASSERT_FALSE(annotation.hasSameUserStatus(dlg.getBodyAnnotation()));

  dlg.updateStatusBox();
  annotation.loadJsonString(
        "{"
           "\"body ID\": 5901214961,"
           "\"instance\": \"test3\","
           "\"naming user\": \"zhaot\","
           "\"status\": \"Traced\","
           "\"user\": \"zhaot\""
        "}");
  dlg.loadBodyAnnotation(annotation);
  ASSERT_EQ(annotation, dlg.getBodyAnnotation());
  ASSERT_TRUE(annotation.hasSameUserStatus(dlg.getBodyAnnotation()));

  annotation.loadJsonString(
        "{"
           "\"body ID\": 5901214961,"
           "\"instance\": \"test3\","
           "\"naming user\": \"zhaot\","
           "\"status\": \"Traced\","
           "\"user\": \"zhaot\","
           "\"clonal unit\": \"cu test\","
           "\"auto-type\": \"at test\""
        "}");
//  std::cout << annotation.getAutoType() << std::endl;
  dlg.loadBodyAnnotation(annotation);
  ASSERT_EQ("cu test", dlg.getClonalUnit());
  ASSERT_EQ("at test", dlg.getAutoType());
  ASSERT_TRUE(dlg.getProperty().empty());

  annotation.loadJsonString(
        "{"
           "\"body ID\": 5901214961,"
           "\"instance\": \"test3\","
           "\"naming user\": \"zhaot\","
           "\"status\": \"Traced\","
           "\"property\": \"Distinct\","
           "\"user\": \"zhaot\","
           "\"clonal unit\": \"cu test\","
           "\"auto-type\": \"at test\""
        "}");
  std::cout << annotation.getProperty() << std::endl;
  dlg.loadBodyAnnotation(annotation);
  ASSERT_EQ("Distinct", dlg.getProperty());

  ZFlyEmBodyAnnotation annot2 = dlg.getBodyAnnotation();
  ASSERT_EQ("cu test", annot2.getClonalUnit());
  ASSERT_EQ("at test", annot2.getAutoType());
  ASSERT_EQ("Distinct", annot2.getProperty());

  annotation.loadJsonString(
        "{"
           "\"body ID\": 5901214961,"
           "\"instance\": \"test3\","
           "\"naming user\": \"zhaot\","
           "\"status\": \"Traced\","
           "\"property\": \"testprop\","
           "\"user\": \"zhaot\","
           "\"clonal unit\": \"cu test\","
           "\"auto-type\": \"at test\""
        "}");
  dlg.loadBodyAnnotation(annotation);
  ASSERT_EQ("testprop", dlg.getProperty());
}

TEST(FlyEmBodyAnnotationDialog, manage)
{
  FlyEmBodyAnnotationDialog dlg(true, nullptr);
  dlg.setValue(FlyEmBodyAnnotationDialog::KEY_TYPE, "test type");
  ASSERT_EQ("test type",
            dlg.getValue(FlyEmBodyAnnotationDialog::KEY_TYPE).toString());

  dlg.setValue(FlyEmBodyAnnotationDialog::KEY_STATUS, "Roughly Traced");
  ASSERT_EQ("Roughly Traced",
            dlg.getValue(FlyEmBodyAnnotationDialog::KEY_STATUS).toString());
  ASSERT_EQ("Roughly Traced", dlg.getStatus());

  dlg.setValue(FlyEmBodyAnnotationDialog::KEY_INSTANCE, "test instance");
  ASSERT_EQ("test instance",
            dlg.getValue(FlyEmBodyAnnotationDialog::KEY_INSTANCE).toString());
  ASSERT_EQ("test instance", dlg.getInstance());

  dlg.setValue(FlyEmBodyAnnotationDialog::KEY_COMMENT, "test comment");
  ASSERT_EQ("test comment",
            dlg.getValue(FlyEmBodyAnnotationDialog::KEY_COMMENT).toString());
  ASSERT_EQ("test comment", dlg.getComment());
}

#endif

#endif // FLYEMBODYANNOTATIONDIALOGTEST_H
