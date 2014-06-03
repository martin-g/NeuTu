#include "tilemanager.h"
#include "ui_tilemanager.h"
#include "ztilemanager.h"
#include "zqtbarprogressreporter.h"
#include <QDebug>

TileManager::TileManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TileManager)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    m_progressReporter = new ZQtBarProgressReporter;
    m_progressReporter->setProgressBar(ui->progressBar);

    ui->tileView->setParentWindow(this);
    ui->tileView->setDragMode(QGraphicsView::ScrollHandDrag);
    //ui->tileView->setTransformationAnchor(QGraphicsView::NoAnchor);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(ShowContextMenu(const QPoint&)));


    scaleFactor = 0.1;
}

TileManager::~TileManager()
{
    delete ui;
    delete m_progressReporter;
}


void TileManager::setTileManager(ZTileManager *manager)
{
  manager->setProgressReporter(m_progressReporter);
  manager->setScaleFactor(scaleFactor);
  manager->setParentView(ui->tileView);
  ui->tileView->setScene(manager);
  ui->tileView->setSceneRect(0,0,ui->tileView->viewport()->frameSize().width(),ui->tileView->viewport()->frameSize().height());
}

void TileManager::ShowContextMenu(const QPoint &pos)
{
    // for most widgets
    QPoint globalPos = this->mapToGlobal(pos);
    QMenu myMenu;
    myMenu.addAction("Show SWC Projection", this, SLOT(on_actionShowSWC_triggered()));
    myMenu.addAction("Turn off SWC Projection",this, SLOT(on_actionTurnOffSWC_triggered()));
    myMenu.exec(globalPos);
}

void TileManager::setDocument(std::tr1::shared_ptr<ZStackDoc> p_doc)
{
    m_doc = p_doc;
    connect(getDocument(),SIGNAL(swcModified()),ui->tileView,SLOT(update()));
    connect(getDocument(),SIGNAL(swcModified()),ui->tileView,SLOT(slotTest()));
}

void TileManager::on_actionShowSWC_triggered()
{
    ui->tileView->setSWCVisibility(true);
    ui->tileView->viewport()->update();
}

void TileManager::on_actionTurnOffSWC_triggered()
{
    ui->tileView->setSWCVisibility(false);
    ui->tileView->viewport()->update();
}

void TileManager::keyPressEvent(QKeyEvent *event)
{
    float scaleFactorNew;

    switch (event->key())
    {
        case Qt::Key_Equal:
            scaleFactorNew = scaleFactor + 0.01;
            if (scaleFactorNew > 1.0) scaleFactorNew = 1.0;
            ui->tileView->scale(scaleFactorNew/scaleFactor,scaleFactorNew/scaleFactor);
            //ui->tileView->translate(ui->tileView->matrix().dx(),ui->tileView->matrix().dy());
            ui->tileView->viewport()->update();
            scaleFactor = scaleFactorNew;
            break;
        case Qt::Key_Minus:
            scaleFactorNew = scaleFactor - 0.01;
            if (scaleFactorNew < 0.1) scaleFactorNew = 0.1;
            ui->tileView->scale(scaleFactorNew/scaleFactor,scaleFactorNew/scaleFactor);
            ui->tileView->viewport()->update();
            scaleFactor = scaleFactorNew;
            break;
        default:
            break;
    }
}

