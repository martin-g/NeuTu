#ifndef ZACTIONFACTORY_H
#define ZACTIONFACTORY_H

#include <QAction>

class ZStackDoc;
class QWidget;
//class ZActionActivator;
//class ZStackPresenter;
class QUndoStack;

/*!
 * \brief The class of producing Qt actions
 *
 * The ZActionFactory class is a factory class of producing actions so that they
 * can be shown consistently in different widgets or windows. Each action is
 * associated with a key of \a EAction type.
 */
class ZActionFactory
{
public:
  ZActionFactory();
  virtual ~ZActionFactory() {}

  enum EAction {
    ACTION_NULL,
    ACTION_TEST, ACTION_ABOUT, ACTION_SAVE_OBJECT_AS,
    ACTION_EXTEND_SWC_NODE, ACTION_SMART_EXTEND_SWC_NODE,
    ACTION_CONNECT_TO_SWC_NODE, ACTION_ADD_SWC_NODE,
    ACTION_TOGGLE_SWC_SKELETON,
    ACTION_LOCK_SWC_NODE_FOCUS, ACTION_CHANGE_SWC_NODE_FOCUS,
    ACTION_MOVE_SWC_NODE,
    ACTION_ESTIMATE_SWC_NODE_RADIUS,
    ACTION_PAINT_STROKE, ACTION_ERASE_STROKE,
    ACTION_LOCATE_SELECTED_SWC_NODES_IN_3D,
    ACTION_ACTIVATE_LOCATE,
    ACTION_SPLIT_DATA, ACTION_SHOW_BODY_IN_3D,
    ACTION_BODY_SPLIT_START, ACTION_ADD_SPLIT_SEED,
    ACTION_BODY_MERGE, ACTION_BODY_UNMERGE,
    ACTION_BODY_ANNOTATION, ACTION_BODY_CHECKIN, ACTION_BODY_CHECKOUT,
    ACTION_BODY_FORCE_CHECKIN, ACTION_BODY_DECOMPOSE,
    ACTION_BODY_CROP, /*ACTION_BODY_CHOP_Z,*/
    ACTION_BODY_CHOP,
    ACTION_BODY_CONNECTION,
    ACTION_BODY_PROFILE,
    ACTION_BODY_EXPERT_STATUS,
    ACTION_BOOKMARK_CHECK, ACTION_BOOKMARK_UNCHECK,
    ACTION_MEASURE_SWC_NODE_LENGTH, ACTION_MEASURE_SCALED_SWC_NODE_LENGTH,
    ACTION_MEASURE_SWC_NODE_DIST,
    ACTION_SWC_SUMMARIZE,
    ACTION_CHNAGE_SWC_NODE_SIZE, ACTION_TRANSLATE_SWC_NODE,
    ACTION_SET_SWC_ROOT, ACTION_INSERT_SWC_NODE,
    ACTION_RESET_BRANCH_POINT, ACTION_SET_BRANCH_POINT,
    ACTION_CONNECTED_ISOLATED_SWC,
    ACTION_DELETE_SWC_NODE, ACTION_CONNECT_SWC_NODE,
    ACTION_DELETE_UNSELECTED_SWC_NODE,
    ACTION_MERGE_SWC_NODE, ACTION_BREAK_SWC_NODE,
    ACTION_SELECT_DOWNSTREAM, ACTION_SELECT_UPSTREAM,
    ACTION_SELECT_NEIGHBOR_SWC_NODE,
    ACTION_SELECT_SWC_BRANCH, ACTION_SELECT_CONNECTED_SWC_NODE,
    ACTION_SELECT_ALL_SWC_NODE,
    ACTION_CHANGE_SWC_TYPE, ACTION_CHANGE_SWC_SIZE, ACTION_REMOVE_TURN,
    ACTION_RESOLVE_CROSSOVER, ACTION_SWC_Z_INTERPOLATION,
    ACTION_SWC_RADIUS_INTERPOLATION, ACTION_SWC_POSITION_INTERPOLATION,
    ACTION_SWC_INTERPOLATION,
    ACTION_SYNAPSE_ADD_PRE, ACTION_SYNAPSE_ADD_POST, ACTION_SYNAPSE_MOVE,
    ACTION_SYNAPSE_DELETE, ACTION_SYNAPSE_LINK, ACTION_SYNAPSE_UNLINK,
    ACTION_SYNAPSE_VERIFY, ACTION_SYNAPSE_UNVERIFY,
    ACTION_SYNAPSE_FILTER, ACTION_SYNAPSE_HLPSD,
    ACTION_SYNAPSE_REPAIR,
    ACTION_TRACE, ACTION_FITSEG, ACTION_DROPSEG, ACTION_FIT_ELLIPSE,
    ACTION_PUNCTA_MARK, ACTION_PUNCTA_ENLARGE, ACTION_PUNCTA_NARROW,
    ACTION_PUNCTA_MEANSHIFT, ACTION_PUNCTA_MEANSHIFT_ALL,
    ACTION_PUNCTA_CHANGE_COLOR, ACTION_PUNCTA_HIDE_SELECTED,
    ACTION_PUNCTA_SHOW_SELECTED, ACTION_PUNCTA_ADD_SELECTION,
    ACTION_SHOW_SYNAPSE, ACTION_SHOW_TODO,
    ACTION_SAVE_SPLIT_TASK, ACTION_DELETE_SPLIT_SEED,
    ACTION_DELETE_SELECTED_SPLIT_SEED,
    ACTION_DELETE_SELECTED,
    ACTION_VIEW_DATA_EXTERNALLY,
    ACTION_UNDO, ACTION_REDO,
    ACTION_SHOW_ORTHO, ACTION_SHOW_ORTHO_BIG,
    ACTION_ADD_TODO_ITEM, ACTION_ADD_TODO_MERGE,
    ACTION_ADD_TODO_SPLIT, ACTION_ADD_TODO_SVSPLIT,
    ACTION_ADD_TODO_TRACE_TO_SOMA, ACTION_ADD_TODO_NO_SOMA,
    ACTION_ADD_TODO_DIAGNOSTIC,
    ACTION_CHECK_TODO_ITEM, ACTION_ACTIVATE_TODO_ITEM,
    ACTION_ACTIVATE_TOSPLIT_ITEM,
    ACTION_ADD_TODO_ITEM_CHECKED, ACTION_TODO_ITEM_ANNOT_NORMAL,
    ACTION_TODO_ITEM_ANNOT_MERGE, ACTION_TODO_ITEM_ANNOT_SPLIT,
    ACTION_TODO_ITEM_ANNOT_IRRELEVANT, ACTION_TODO_ITEM_ANNOT_TRACE_TO_SOMA,
    ACTION_TODO_ITEM_ANNOT_NO_SOMA,
    ACTION_SHOW_NORMAL_TODO,
    ACTION_REMOVE_TODO_BATCH,
    ACTION_UNCHECK_TODO_ITEM, ACTION_REMOVE_TODO_ITEM,
    ACTION_ENTER_RECT_ROI_MODE, ACTION_CANCEL_RECT_ROI,
    ACTION_SELECT_BODY_IN_RECT, ACTION_ZOOM_TO_RECT,
    ACTION_DESELECT_BODY,
    ACTION_REWRITE_SEGMENTATION, ACTION_REFRESH_SEGMENTATION,
    ACTION_REFRESH_DATA,
    ACTION_FLYEM_UPDATE_BODY,
    ACTION_FLYEM_COMPARE_BODY,
    ACTION_SHOW_SPLIT_MESH_ONLY,
    ACTION_TOGGLE_SUPERVOXEL_VIEW,
    ACTION_SAVE_STACK, ACTION_COPY_POSITION, ACTION_COPY_BODY_ID,
    ACTION_COPY_SUPERVOXEL_ID, ACTION_COPY_NEUROGLANCER_LINK,
    ACTION_SHOW_SUPERVOXEL_LIST,
    ACTION_EXIT_SPLIT, ACTION_START_SPLIT, ACTION_COMMIT_SPLIT,
    ACTION_GO_TO_BODY, ACTION_GO_TO_POSITION, ACTION_SELECT_BODY,
    ACTION_INFORMATION, ACTION_BODY_QUERY, ACTION_BODY_FIND_SIMILIAR,
    ACTION_BODY_QUERY_BY_NAME, ACTION_BODY_QUERY_BY_STATUS,
    ACTION_BODY_QUERY_ALL_NAMED,
    ACTION_BODY_EXPORT_SELECTED, ACTION_BODY_EXPORT_SELECTED_LEVEL,
    ACTION_BODY_SKELETONIZE_TOP, ACTION_BODY_SKELETONIZE_LIST,
    ACTION_BODY_SKELETONIZE_SELECTED,
    ACTION_BODY_UPDATE_MESH, ACTION_BODY_EXPORT_STACK,
    ACTION_CLEAR_ALL_MERGE, ACTION_BODY_REPORT_CORRUPUTION,
    ACTION_BODY_COLOR_NORMAL, ACTION_BODY_COLOR_SEQUENCER,
    ACTION_BODY_COLOR_NAME, ACTION_BODY_COLOR_PROTOCOL,
    ACTION_3DWINDOW_TOGGLE_SETTING, ACTION_3DWINDOW_TOGGLE_OBJECTS,
    ACTION_COPY_3DCAMERA, ACTION_PASTE_3DCAMERA,
    ACTION_SAVE_ALL_MESH, ACTION_VIEW_SCREENSHOT,
    ACTION_RUN_TIP_DETECTION,
    ACTION_SEPARATOR
  };

#if 0
  static QAction* makeAction(
      EAction item, const ZStackDoc *doc, QWidget *parent,
      ZActionActivator *activator = NULL, bool positive = true);
#endif

/*
  static QAction* makeAction(
      EAction item, const ZStackPresenter *presenter, QWidget *parent,
      ZActionActivator *activator = NULL, bool positive = true);
*/
  static QAction *MakeAction(EAction actionKey, QObject *parent);

  virtual QAction* makeAction(EAction actionKey, QObject *parent) const;

  void setUndoStack(QUndoStack *undoStack);

  static bool IsRegularAction(EAction actionKey);

private:
  QUndoStack *m_undoStack = NULL;
};

#endif // ZACTIONFACTORY_H
