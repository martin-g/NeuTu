#include "zstackdocmenufactory.h"

#include <QMenu>

#include "neutubeconfig.h"
#include "zstackdoc.h"
#include "zstackpresenter.h"

ZStackDocMenuFactory::ZStackDocMenuFactory()
{
}

QMenu* ZStackDocMenuFactory::makeSwcNodeContextMenu(
    const ZStackDoc *doc, QMenu *menu)
{
  if (menu == NULL) {
    menu = new QMenu(NULL);
  }

  menu->addAction(doc->getAction(ZStackDoc::ACTION_DELETE_SWC_NODE));
  menu->addAction(doc->getAction(ZStackDoc::ACTION_BREAK_SWC_NODE));
  menu->addAction(doc->getAction(ZStackDoc::ACTION_CONNECT_SWC_NODE));
  menu->addAction(doc->getAction(ZStackDoc::ACTION_MERGE_SWC_NODE));

  QMenu *submenu = new QMenu("Select", menu);
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_SELECT_DOWNSTREAM));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_SELECT_UPSTREAM));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_SELECT_SWC_BRANCH));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_SELECT_CONNECTED_SWC_NODE));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_SELECT_ALL_SWC_NODE));
  menu->addMenu(submenu);

  submenu = new QMenu("Advanced Editing", menu);
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_REMOVE_TURN));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_RESOLVE_CROSSOVER));
  menu->addMenu(submenu);

  submenu = new QMenu("Change property", menu);
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_TRANSLATE_SWC_NODE));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_CHANGE_SWC_SIZE));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_SET_SWC_ROOT));
  menu->addMenu(submenu);

  submenu = new QMenu("Information", menu);
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_SWC_SUMMARIZE));
  submenu->addAction(doc->getAction(ZStackDoc::ACTION_MEASURE_SWC_NODE_LENGTH));
  menu->addMenu(submenu);

  return menu;
}

QMenu* ZStackDocMenuFactory::makeSwcNodeContextMenu(
    const ZStackPresenter *presenter, QMenu *menu)
{
  if (menu == NULL) {
    menu = new QMenu(NULL);
  }

  menu->addAction(presenter->getAction(ZStackPresenter::ACTION_SMART_EXTEND_SWC_NODE));
  menu->addAction(presenter->getAction(ZStackPresenter::ACTION_EXTEND_SWC_NODE));
  menu->addAction(presenter->getAction(ZStackPresenter::ACTION_CONNECT_TO_SWC_NODE));

  if (GET_APPLICATION_NAME == "Biocytin") {
    menu->addAction(presenter->getAction(
                      ZStackPresenter::ACTION_LOCK_SWC_NODE_FOCUS));
    menu->addAction(presenter->getAction(
                      ZStackPresenter::ACTION_ESTIMATE_SWC_NODE_RADIUS));
  }

  return menu;
}
