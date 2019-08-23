#include "connectionvalidationprotocol.h"
#include "ui_connectionvalidationprotocol.h"

#include <iostream>
#include <stdlib.h>

#include <QFileDialog>
#include <QMessageBox>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>


#include "zjsonobject.h"
#include "zjsonparser.h"

ConnectionValidationProtocol::ConnectionValidationProtocol(QWidget *parent) :
    ProtocolDialog(parent),
    ui(new Ui::ConnectionValidationProtocol)
{
    ui->setupUi(this);

    // sites table
    m_sitesModel = new QStandardItemModel(0, 5, ui->sitesTableView);
    setSitesHeaders(m_sitesModel);
    ui->sitesTableView->setModel(m_sitesModel);

    // UI connections
    connect(ui->firstButton, SIGNAL(clicked(bool)), this, SLOT(onFirstButton()));
    connect(ui->nextButton, SIGNAL(clicked(bool)), this, SLOT(onNextButton()));
    connect(ui->markButton, SIGNAL(clicked(bool)), this, SLOT(onMarkButton()));
    connect(ui->markAndNextButton, SIGNAL(clicked(bool)), this, SLOT(onMarkAndNextButton()));
    connect(ui->gotoButton, SIGNAL(clicked(bool)), this, SLOT(onGotoButton()));

    connect(ui->exitButton, SIGNAL(clicked(bool)), this, SLOT(onExitButton()));
    connect(ui->completeButton, SIGNAL(clicked(bool)), this, SLOT(onCompleteButton()));

    connect(ui->tbarCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onTbarGoodChanged()));
    connect(ui->tbarSegCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onTbarSegGoodChanged()));
    connect(ui->psdCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onPSDGoodCanged()));
    connect(ui->psdSegCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onPSDSegGoodChanged()));

    connect(ui->sitesTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(onClickedTable(QModelIndex)));

    m_currentIndex = -1;
}

// constants
const std::string ConnectionValidationProtocol::KEY_VERSION = "version";
const int ConnectionValidationProtocol::fileVersion = 1;

bool ConnectionValidationProtocol::initialize() {

    QString filepath = QFileDialog::getOpenFileName(this, "Choose point file");
    if (filepath.size() == 0) {
        return false;
    }

    // read the file
    QFile file;
    file.setFileName(filepath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString fileData = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(fileData.toUtf8());
    if (doc.isNull()) {
        // error in parsing
        showError("Point file error", "The point file should be a json file!");
        return false;
    }

    // check file type and version
    QJsonObject data = doc.object();
    if (!data.contains("file type") || data["file type"].toString() != "connection validation") {
        showError("File type error", "Input point file should have type 'connection validation'.");
        return false;
    }
    if (!data.contains("file version") || data["file version"].toInt() != 1) {
        showError("File version error", "Input point file should be version 1");
        return false;
    }

    // after this last check, we assume format is right let it crash if not
    if (!data.contains("points")) {
        showError("No points!", "Input point file does not contain 'points' key!");
        return false;
    }

    // get point data into internal data structures
    QJsonArray jsonArray = data["points"].toArray();
    loadPoints(jsonArray);

    updateTable();
    updateLabels();







    // save initial data and get going
    saveState();

    return true;
}

void ConnectionValidationProtocol::gotoCurrentPoint() {
    if (m_currentIndex >= 0) {
        ZIntPoint p = m_points[m_currentIndex];
        emit requestDisplayPoint(p.getX(), p.getY(), p.getZ());
    }
}

void ConnectionValidationProtocol::onFirstButton() {

}

void ConnectionValidationProtocol::onNextButton() {

}

void ConnectionValidationProtocol::onMarkButton() {

}

void ConnectionValidationProtocol::onMarkAndNextButton() {

}

void ConnectionValidationProtocol::onGotoButton() {
    gotoCurrentPoint();
}

void ConnectionValidationProtocol::onTbarGoodChanged() {

}

void ConnectionValidationProtocol::onTbarSegGoodChanged() {

}

void ConnectionValidationProtocol::onPSDGoodCanged() {

}

void ConnectionValidationProtocol::onPSDSegGoodChanged() {

}

void ConnectionValidationProtocol::onClickedTable(QModelIndex tableIndex) {
    // we don't have a sort proxy, so the table index = model index at this point
    setCurrentPoint(tableIndex.row());
}

void ConnectionValidationProtocol::setCurrentPoint(int index) {
    m_currentIndex = index;
    updateCurrentLabel();
    updateCheckBoxes();
}

void ConnectionValidationProtocol::updateLabels() {
    updateCurrentLabel();
    updateProgressLabel();
}

void ConnectionValidationProtocol::updateCurrentLabel() {
    if (m_currentIndex < 0) {
        ui->currentLabel->setText("n/a)");
    } else {
        ui->currentLabel->setText(QString::fromStdString(m_points[m_currentIndex].toString()));
    }
}

void ConnectionValidationProtocol::updateProgressLabel() {
    if (m_currentIndex < 0) {
        ui->progressLabel->setText("0/0 (0%)");
    } else {
        int nReviewed = 0;
        for (ZIntPoint p: m_points) {
            if (m_pointData[p].reviewed) {
                nReviewed++;
            }
        }
        float percent = float(nReviewed) / m_points.size();
        ui->progressLabel->setText(QString("%1/%2 (%3%)").arg(nReviewed).arg(m_points.size()).arg(percent));
    }
}

void ConnectionValidationProtocol::updateCheckBoxes() {

}

void ConnectionValidationProtocol::updateTable() {
    clearSitesTable();

    int row = 0;
    for (ZIntPoint p: m_points) {
        QStandardItem * pointItem = new QStandardItem();
        pointItem->setData(QVariant(QString::fromStdString(p.toString())), Qt::DisplayRole);
        m_sitesModel->setItem(row, POINT_COLUMN, pointItem);

        PointData pd = m_pointData[p];

        // later I'd like these to be graphics/icons; check mark for reviewed,
        //  and white plus on green circle/white minus on red circle for the others
        QStandardItem * revItem = new QStandardItem();
        QStandardItem * tbarGoodItem = new QStandardItem();
        QStandardItem * tbarSegGoodItem = new QStandardItem();
        QStandardItem * psdGoodItem = new QStandardItem();
        QStandardItem * psdSegGoodItem = new QStandardItem();
        revItem->setData(QVariant(pd.reviewed), Qt::DisplayRole);
        tbarGoodItem->setData(QVariant(pd.tbarGood), Qt::DisplayRole);
        tbarSegGoodItem->setData(QVariant(pd.tbarSegGood), Qt::DisplayRole);
        psdGoodItem->setData(QVariant(pd.psdGood), Qt::DisplayRole);
        psdSegGoodItem->setData(QVariant(pd.psdSegGood), Qt::DisplayRole);
        m_sitesModel->setItem(row, REVIEWED_COLUMN, revItem);
        m_sitesModel->setItem(row, TBAR_GOOD_COLUMN, tbarGoodItem);
        m_sitesModel->setItem(row, TBAR_SEG_GOOD_COLUMN, tbarSegGoodItem);
        m_sitesModel->setItem(row, PSD_GOOD_COLUMN, psdGoodItem);
        m_sitesModel->setItem(row, PSD_SEG_GOOD_COLUMN, psdSegGoodItem);

        row++;

    }
#if QT_VERSION >= 0x050000
    ui->sitesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->sitesTableView->horizontalHeader()->setSectionResizeMode(POINT_COLUMN, QHeaderView::Stretch);
#else
    ui->sitesTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->sitesTableView->horizontalHeader()->setResizeMode(POINT_COLUMN, QHeaderView::Stretch);
#endif
}

void ConnectionValidationProtocol::loadPoints(QJsonArray array) { // get point data into internal data structures
    m_points.clear();
    m_pointData.clear();
    for (QJsonValue val: array) {
        QJsonArray pointArray = val.toArray();
        ZIntPoint point;
        point.setX(pointArray[0].toInt());
        point.setY(pointArray[1].toInt());
        point.setZ(pointArray[2].toInt());
        PointData pd;
        m_points << point;
        m_pointData[point] = pd;
    }
}

void ConnectionValidationProtocol::saveState() {
    ZJsonObject data;

    // always version your output files!
    data.setEntry(KEY_VERSION.c_str(), fileVersion);

    // save data; all the keys are hard-coded here for now as I'm
    //  in a hurry; make them constants later

    // describe the arrays we'll be adding:
    ZJsonArray fields;
    fields.append("PSD location x");
    fields.append("PSD location y");
    fields.append("PSD location z");
    fields.append("reviewed");
    fields.append("T-bar good");
    fields.append("T-bar segmentation good");
    fields.append("PSD good");
    fields.append("PSD segmentation good");
    data.setEntry("fields", fields);

    // and the items themselves:
    ZJsonArray connections;
    for (ZIntPoint p: m_points) {
        PointData pd = m_pointData[p];
        ZJsonArray c;
        c.append(p.getX());
        c.append(p.getY());
        c.append(p.getZ());
        c.append(pd.reviewed);
        c.append(pd.tbarGood);
        c.append(pd.tbarSegGood);
        c.append(pd.psdGood);
        c.append(pd.psdSegGood);
        connections.append(c);
    }
    data.setEntry("connections", connections);

    emit requestSaveProtocol(data);
}

void ConnectionValidationProtocol::loadDataRequested(ZJsonObject data) {
    // check version of saved data here
    if (!data.hasKey(KEY_VERSION.c_str())) {

        // report error

        return;
    }
    int version = ZJsonParser::integerValue(data[KEY_VERSION.c_str()]);
    if (version > fileVersion) {


        // report error


        return;
    }

    // convert old versions to current version here, when it becomes necessary


    // load data here






    // if, in the future, you need to update to a new save version,
    //  remember to do a save here


    // start work

}

void ConnectionValidationProtocol::onExitButton() {
    // exit protocol; can be reopened and worked on later
    emit protocolExiting();
}

void ConnectionValidationProtocol::onCompleteButton() {
    QMessageBox mb;
    mb.setText("Complete protocol");
    mb.setInformativeText("When you complete the protocol, it will save and exit immediately.  You will not be able to reopen it.\n\nComplete protocol now?");
    mb.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    mb.setDefaultButton(QMessageBox::Cancel);
    int ans = mb.exec();

    if (ans == QMessageBox::Ok) {
        // save here if needed
        // saveState();
        emit protocolCompleting();
    }
}

void ConnectionValidationProtocol::setSitesHeaders(QStandardItemModel * model) {
    model->setHorizontalHeaderItem(POINT_COLUMN, new QStandardItem(QString("point")));
    model->setHorizontalHeaderItem(REVIEWED_COLUMN, new QStandardItem(QString("rev")));
    model->setHorizontalHeaderItem(TBAR_GOOD_COLUMN, new QStandardItem(QString("T-bar")));
    model->setHorizontalHeaderItem(TBAR_SEG_GOOD_COLUMN, new QStandardItem(QString("T-seg")));
    model->setHorizontalHeaderItem(PSD_GOOD_COLUMN, new QStandardItem(QString("PSD")));
    model->setHorizontalHeaderItem(PSD_SEG_GOOD_COLUMN, new QStandardItem(QString("P-seg")));
}

void ConnectionValidationProtocol::clearSitesTable() {
    m_sitesModel->clear();
    setSitesHeaders(m_sitesModel);
}

void ConnectionValidationProtocol::showError(QString title, QString message) {
    QMessageBox mb;
    mb.setText(title);
    mb.setInformativeText(message);
    mb.setStandardButtons(QMessageBox::Ok);
    mb.setDefaultButton(QMessageBox::Ok);
    mb.exec();
}

ConnectionValidationProtocol::~ConnectionValidationProtocol()
{
    delete ui;
}
