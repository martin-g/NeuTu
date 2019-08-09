#ifndef TIPDETECTORDIALOG_H
#define TIPDETECTORDIALOG_H

#include <QDialog>
#include <QProcess>

#include "dvid/zdvidtarget.h"

namespace Ui {
class TipDetectorDialog;
}

class TipDetectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TipDetectorDialog(QWidget *parent = 0);
    ~TipDetectorDialog();

    void setBodyID(uint64_t bodyID);
    void setRoiList(QStringList roiList);
    void setDvidTarget(ZDvidTarget target);

private slots:
    void onRunButton();    
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void applicationQuitting();

private:
    enum ScriptStatus {
        NOT_RUNNING,
        RUNNING,
        FINISHED,
        ERROR
    };

    Ui::TipDetectorDialog *ui;

    uint64_t m_bodyID;
    ZDvidTarget m_target;
    QProcess m_process;

    void setStatus(ScriptStatus status);
    void setMessage(QString message);
};

#endif // TIPDETECTORDIALOG_H
