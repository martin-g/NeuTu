#include "flyemmvcdialogmanager.h"

#include <typeinfo>

#include "common/utilities.h"
#include "logging/zlog.h"
#include "zjsonobject.h"

#include "zdialogfactory.h"

#include "zflyemproofmvc.h"
#include "zflyemproofdoc.h"
#include "service/neuprintreader.h"
#include "dvid/zdvidannotation.h"

#include "dialogs/flyemtododialog.h"
#include "dialogs/zdvidtargetproviderdialog.h"
#include "dialogs/zflyemtodoannotationdialog.h"
#include "dialogs/neuprintquerydialog.h"
#include "dialogs/neuprintsetupdialog.h"
#include "dialogs/zcontrastprotocaldialog.h"
#include "dialogs/zspinboxdialog.h"
#include "dialogs/flyembodyinfodialog.h"
#include "dialogs/zflyemsplitcommitdialog.h"
#include "dialogs/zflyemsplituploadoptiondialog.h"
#include "dialogs/zflyembodychopdialog.h"
#include "dialogs/zinfodialog.h"
#include "dialogs/flyemdialogfactory.h"
#include "dialogs/zflyemskeletonupdatedialog.h"
#include "dialogs/zflyemroitooldialog.h"
#include "dialogs/zflyemgrayscaledialog.h"
#include "dialogs/flyembodyiddialog.h"
#include "dialogs/zflyemmergeuploaddialog.h"
#include "dialogs/flyemdialogfactory.h"
#include "dialogs/zflyemproofsettingdialog.h"
#include "dialogs/tipdetectordialog.h"
#include "dialogs/zsynapsepropertydialog.h"
#include "dialogs/neuroglancerlinkdialog.h"
//#include "dialogs/zstackviewrecorddialog.h"

/** Implementation details
 *
 * This class is to separate dialog-related code from ZFlyEmProofMvc, reducing
 * the size of zflyemproofmvc.cpp. It does not granulate the ZFlyEmProofMvc
 * class, however, because of the mutual dependency of the two classes.
 */

FlyEmMvcDialogManager::FlyEmMvcDialogManager(ZFlyEmProofMvc *parent) :
  QObject(parent),
  m_parent(parent)
{
}

FlyEmMvcDialogManager::~FlyEmMvcDialogManager()
{
}

template <typename T>
bool FlyEmMvcDialogManager::createIfNecessary(T* &dlg)
{
  if (isNull(dlg)) {
    KINFO << "Creating" << typeid(T).name(); //Not necessary works (compiler dependent), but seems good enough for debugging
    dlg = new T(m_parent);

    return true;
  }

  return false;
}

/*
bool FlyEmMvcDialogManager::creationRequired(void *dlg) const
{
  return isNull(dlg) && m_parent->hasWidgetRole();
}
*/

ZDvidTargetProviderDialog* FlyEmMvcDialogManager::getDvidDlg()
{
  if (isNull(m_dvidDlg)) {
    KINFO << "Creating ZDvidTargetProviderDialog";
    m_dvidDlg = ZDialogFactory::makeDvidDialog(m_parent);
  }

  return m_dvidDlg;
}

void FlyEmMvcDialogManager::setDvidDlg(ZDvidTargetProviderDialog *dlg)
{
  m_dvidDlg = dlg;
}

bool FlyEmMvcDialogManager::isDvidDlgReady() const
{
  return !isNull(m_dvidDlg);
}

bool FlyEmMvcDialogManager::isRoiDlgReady() const
{
  return !isNull(m_roiDlg);
}

bool FlyEmMvcDialogManager::isBodyInfoDlgReady() const
{
  return !isNull(m_bodyInfoDlg);
}

bool FlyEmMvcDialogManager::isSplitUploadDlgReady() const
{
  return !isNull(m_splitUploadDlg);
}

ZContrastProtocalDialog* FlyEmMvcDialogManager::getContrastDlg()
{
  if (createIfNecessary(m_contrastDlg)) {
//    KINFO << "ZContrastProtocalDialog created";
    m_contrastDlg = new ZContrastProtocalDialog(m_parent);
    m_parent->configureContrastDlg(m_contrastDlg);
    //    m_contrastDlg->setContrastProtocol(getPresenter()->getHighContrastProtocal());
    QObject::connect(m_contrastDlg, &ZContrastProtocalDialog::protocolChanged,
                     m_parent, &ZFlyEmProofMvc::updateTmpContrast);
    QObject::connect(m_contrastDlg, &ZContrastProtocalDialog::canceled,
                     m_parent, &ZFlyEmProofMvc::resetContrast);
    QObject::connect(m_contrastDlg, &ZContrastProtocalDialog::committing,
                     m_parent, &ZFlyEmProofMvc::saveTmpContrast);
  }

  return m_contrastDlg;
}

ZFlyEmSplitCommitDialog* FlyEmMvcDialogManager::getSplitCommitDlg()
{
  createIfNecessary(m_splitCommitDlg);

  return m_splitCommitDlg;
}

FlyEmTodoDialog* FlyEmMvcDialogManager::getTodoDlg()
{
  if (createIfNecessary(m_todoDlg)) {
    m_parent->configureTodoDlg(m_todoDlg);
  }

  return m_todoDlg;
}

ZFlyEmRoiToolDialog* FlyEmMvcDialogManager::getRoiDlg()
{
  if (createIfNecessary(m_roiDlg)) {
    QObject::connect(m_roiDlg, SIGNAL(projectActivited()),
                     m_parent, SLOT(loadRoiProject()));
    QObject::connect(m_roiDlg, SIGNAL(projectClosed()),
                     m_parent, SLOT(closeRoiProject()));
    QObject::connect(m_roiDlg, SIGNAL(showing3DRoiCurve()),
                     m_parent, SLOT(showRoi3dWindow()));
    QObject::connect(m_roiDlg, SIGNAL(goingToSlice(int)),
                     m_parent, SLOT(goToSlice(int)));
    QObject::connect(m_roiDlg, SIGNAL(steppingSlice(int)),
                     m_parent, SLOT(stepSlice(int)));
    QObject::connect(m_roiDlg, SIGNAL(goingToNearestRoi()),
                     m_parent, SLOT(goToNearestRoi()));
    QObject::connect(m_roiDlg, SIGNAL(estimatingRoi()),
                     m_parent, SLOT(estimateRoi()));
    QObject::connect(m_roiDlg, SIGNAL(movingPlane(double,double)),
                     m_parent, SLOT(movePlaneRoi(double, double)));
    QObject::connect(m_roiDlg, SIGNAL(rotatingPlane(double)),
                     m_parent, SLOT(rotatePlaneRoi(double)));
    QObject::connect(m_roiDlg, SIGNAL(scalingPlane(double,double)),
                     m_parent, SLOT(scalePlaneRoi(double,double)));

    m_roiDlg->updateDvidTarget();
    m_roiDlg->downloadAllProject();
  }

  return m_roiDlg;
}

ZFlyEmSplitUploadOptionDialog* FlyEmMvcDialogManager::getSplitUploadDlg()
{
  if (createIfNecessary(m_splitUploadDlg)) {
    m_parent->configureSplitUploadDlg(m_splitUploadDlg);
  }

  return m_splitUploadDlg;
}

ZFlyEmBodyChopDialog* FlyEmMvcDialogManager::getBodyChopDlg()
{
  createIfNecessary(m_bodyChopDlg);

  return m_bodyChopDlg;
}

ZInfoDialog* FlyEmMvcDialogManager::getInfoDlg()
{
  createIfNecessary(m_infoDlg);

  return m_infoDlg;
}

ZFlyEmSkeletonUpdateDialog* FlyEmMvcDialogManager::getSkeletonUpdateDlg()
{
  createIfNecessary(m_skeletonUpdateDlg);

  return m_skeletonUpdateDlg;
}

ZFlyEmGrayscaleDialog* FlyEmMvcDialogManager::getGrayscaleDlg()
{
  createIfNecessary(m_grayscaleDlg);

  return m_grayscaleDlg;
}

FlyEmBodyIdDialog* FlyEmMvcDialogManager::getBodyIdDialog()
{
  createIfNecessary(m_bodyIdDialog);

  return m_bodyIdDialog;
}

ZFlyEmMergeUploadDialog* FlyEmMvcDialogManager::getMergeUploadDlg()
{
  createIfNecessary(m_mergeUploadDlg);

  return m_mergeUploadDlg;
}

ZFlyEmProofSettingDialog* FlyEmMvcDialogManager::getSettingDlg()
{
  createIfNecessary(m_settingDlg);

  return m_settingDlg;
}

/*
ZStackViewRecordDialog* FlyEmMvcDialogManager::getRecordDlg()
{
  createIfNecessary(m_recordDlg);

  return m_recordDlg;
}
*/

FlyEmBodyAnnotationDialog *FlyEmMvcDialogManager::getAnnotationDlg()
{
  if (isNull(m_annotationDlg)) {
    KINFO << "Creating FlyEmBodyAnnotationDialog";
    m_annotationDlg = FlyEmDialogFactory::MakeBodyAnnotationDialog(
          m_parent->getCompleteDocument(), m_parent);
  }

  return m_annotationDlg;
}

ZGenericBodyAnnotationDialog* FlyEmMvcDialogManager::getGenericAnnotationDlg()
{
  if (isNull(m_genericAnnotationDlg)) {
    auto *doc = m_parent->getCompleteDocument();
//    auto reader = doc->getDvidReader();

    ZJsonObject schema = doc->getBodyAnnotationSchema();
    if (!schema.isEmpty()) {
      m_genericAnnotationDlg = FlyEmDialogFactory::MakeBodyAnnotaitonDialog(
            doc, schema, m_parent);
    }
  }

  return m_genericAnnotationDlg;
}

template<typename T>
FlyEmBodyInfoDialog* FlyEmMvcDialogManager::makeBodyInfoDlg(
    const T &flag, bool initTarget)
{
//  KINFO << "Creating FlyEmBodyInfoDialog";
  FlyEmBodyInfoDialog *dlg = new FlyEmBodyInfoDialog(flag, m_parent);
  dlg->setNeuprintDataset(m_neuprintDataset);
  if (initTarget) {
    dlg->dvidTargetChanged(m_parent->getDvidTarget());
  }

  QObject::connect(m_parent, SIGNAL(dvidTargetChanged(ZDvidTarget)),
                   dlg, SLOT(dvidTargetChanged(ZDvidTarget)));
  QObject::connect(dlg, SIGNAL(bodyActivated(uint64_t)),
                   m_parent, SLOT(locateBody(uint64_t)));
  QObject::connect(dlg, SIGNAL(addBodyActivated(uint64_t)),
                   m_parent, SLOT(addLocateBody(uint64_t)));
  QObject::connect(dlg, SIGNAL(bodiesActivated(QList<uint64_t>)),
                   m_parent, SLOT(selectBody(QList<uint64_t>)));
  QObject::connect(m_bodyInfoDlg, SIGNAL(namedBodyChanged(ZJsonValue)),
                   m_parent, SLOT(prepareBodyMap(ZJsonValue)));
  QObject::connect(dlg, SIGNAL(colorMapChanged(ZFlyEmSequencerColorScheme)),
                   m_parent, SLOT(updateSequencerBodyMap(ZFlyEmSequencerColorScheme)));
  QObject::connect(dlg, SIGNAL(pointDisplayRequested(int,int,int)),
                   m_parent, SLOT(zoomTo(int,int,int)));

  return dlg;
}

void FlyEmMvcDialogManager::detachBodyInfoDlg()
{
  m_bodyInfoDlg = nullptr;
}

void FlyEmMvcDialogManager::detachBodyQueryDlg()
{
  m_bodyQueryDlg = nullptr;
}

void FlyEmMvcDialogManager::detachNeuprintBodyDlg()
{
  m_neuprintBodyDlg = nullptr;
}

FlyEmBodyInfoDialog* FlyEmMvcDialogManager::getBodyInfoDlg()
{
  if (isNull(m_bodyInfoDlg)) {
    KINFO << "Creating sequencer dialog";
    m_bodyInfoDlg = makeBodyInfoDlg(FlyEmBodyInfoDialog::EMode::SEQUENCER, false);
    connect(m_bodyInfoDlg, SIGNAL(destroyed()), this, SLOT(detachBodyInfoDlg()));
  }

  return m_bodyInfoDlg;
}

TipDetectorDialog* FlyEmMvcDialogManager::getTipDetectorDlg() {
    if (isNull(m_tipDetectorDlg)) {
      KINFO << "Creating tip detector dialog";
      m_tipDetectorDlg = new TipDetectorDialog(m_parent);
    }

    return m_tipDetectorDlg;
}

ZSynapsePropertyDialog* FlyEmMvcDialogManager::getSynpasePropertyDlg()
{
  if (createIfNecessary(m_synapseDlg)) {
    m_synapseDlg->setRadius(ZDvidAnnotation::DEFAULT_PRE_SYN_RADIUS,
                            ZDvidAnnotation::DEFAULT_POST_SYN_RADIUS);
    connect(m_synapseDlg, SIGNAL(synapseRadiusChanged(double, double)),
            m_parent, SLOT(updateSynapseDefaultRadius(double, double)));
  }

  return m_synapseDlg;
}

NeuroglancerLinkDialog* FlyEmMvcDialogManager::getNeuroglancerLinkDlg()
{
  createIfNecessary(m_neuroglancerLinkDlg);

  return m_neuroglancerLinkDlg;
}

void FlyEmMvcDialogManager::Show(QDialog *dlg)
{
  if (dlg) {
    dlg->show();
    dlg->raise();
  }
}

void FlyEmMvcDialogManager::showSynpasePropertyDlg()
{
  Show(getSynpasePropertyDlg());
}

FlyEmBodyInfoDialog* FlyEmMvcDialogManager::getBodyQueryDlg()
{
  if (isNull(m_bodyQueryDlg)) {
    KINFO << "Creating body query dialog";
    m_bodyQueryDlg = makeBodyInfoDlg(FlyEmBodyInfoDialog::EMode::QUERY, true);
    QObject::connect(m_bodyQueryDlg, SIGNAL(refreshing()),
            m_parent, SLOT(showBodyConnection()));
    connect(m_bodyQueryDlg, SIGNAL(destroyed()),
            this, SLOT(detachBodyQueryDlg()));
  }

  return m_bodyQueryDlg;
}

FlyEmBodyInfoDialog* FlyEmMvcDialogManager::getNeuprintBodyDlg()
{
  if (isNull(m_neuprintBodyDlg)) {
    if (getNeuprintSetupDlg()->exec()) {
      std::unique_ptr<NeuPrintReader> reader =
          getNeuprintSetupDlg()->takeNeuPrintReader();

      if (reader) {
        neutu::EServerStatus status = reader->getStatus();
//      m_neuprintDataset = getNeuprintSetupDlg()->getDataset().toStdString();
        //      neutu::EServerStatus status = m_parent->getNeuPrintStatus();

        switch (status) {
        case neutu::EServerStatus::NORMAL:
          m_neuprintBodyDlg = makeBodyInfoDlg(
                FlyEmBodyInfoDialog::EMode::NEUPRINT, true);
          m_neuprintBodyDlg->setNeuPrintReader(std::move(reader));
          connect(m_neuprintBodyDlg, SIGNAL(destroyed()),
                  this, SLOT(detachNeuprintBodyDlg()));
          //        m_neuprintBodyDlg->setNeuprintDataset(m_neuprintDataset);
          break;
        case neutu::EServerStatus::NOSUPPORT:
          ZDialogFactory::Error(
                "NeuPrint Not Supported",
                "Cannot use NeuPrint because this dataset is not supported by the server.",
                m_parent);
          break;
        case neutu::EServerStatus::NOAUTH:
          ZDialogFactory::Error(
                "NeuPrint Not Authorized",
                "Cannot use NeuPrint because the token is not accepted.",
                m_parent);
          break;
        case neutu::EServerStatus::OFFLINE:
          ZDialogFactory::Error(
                "NeuPrint Not Connected",
                "Cannot use NeuPrint because the server cannot be connected.",
                m_parent);
          break;
        }
      }

    /*
    neutu::EServerStatus status = m_parent->getNeuPrintStatus();
    switch (status) {
    case neutu::EServerStatus::NOSUPPORT:
      ZDialogFactory::Error(
            "NeuPrint Not Supported",
            "Cannot use NeuPrint because this dataset is not supported by the server.",
            m_parent);
      break;
    default:
      if (status != neutu::EServerStatus::NORMAL) {
        getNeuprintSetupDlg()->exec();
        m_neuprintDataset = getNeuprintSetupDlg()->getDataset().toStdString();
        status = m_parent->getNeuPrintStatus();
      }

      if (status == neutu::EServerStatus::NORMAL) {
        KINFO << "Creating NeuPrint dialog";
        m_neuprintBodyDlg = makeBodyInfoDlg(FlyEmBodyInfoDialog::EMode::NEUPRINT);
        m_neuprintBodyDlg->setNeuprintDataset(m_neuprintDataset);
      }
      break;
    }
    */
    }
  }

  return m_neuprintBodyDlg;
}

NeuprintSetupDialog* FlyEmMvcDialogManager::getNeuprintSetupDlg()
{
  createIfNecessary(m_neuprintSetupDlg);
  m_neuprintSetupDlg->setUuid(m_parent->getDvidTargetUuid().c_str());

  return m_neuprintSetupDlg;
}
