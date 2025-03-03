#ifndef ZFLYEMBOOKMARKLISTMODEL_H
#define ZFLYEMBOOKMARKLISTMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>

#include "qt/core/zsortfiltertablemodel.h"

#include "zflyembookmark.h"
#include "zflyembookmarkpresenter.h"
#include "zflyembookmarkptrarray.h"

class ZSortFilterProxyModel;


class ZFlyEmBookmarkListModel : public ZSortFilterTableModel
{
  Q_OBJECT
public:
  explicit ZFlyEmBookmarkListModel(QObject *parent = 0);

  int rowCount( const QModelIndex & parent = QModelIndex() ) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data( const QModelIndex & index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
  bool insertColumns(int col, int count, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  bool removeColumns(int col, int count, const QModelIndex &parent = QModelIndex());

//  QModelIndex index(int row, int column,
//                    const QModelIndex &parent = QModelIndex()) const;

  void clear();
  void append(const ZFlyEmBookmark *bookmark);
  void appendSliently(const ZFlyEmBookmark *bookmark);
  void append(const QList<ZFlyEmBookmark*> &bookmarkList);
//  void update(int row);

  const ZFlyEmBookmark* getBookmark(int index) const;
  ZFlyEmBookmark* getBookmark(int index);
  const ZFlyEmBookmarkPtrArray& getBookmarkArray() const;

  void removeBookmark(ZFlyEmBookmark *bookmark);

//  QSortFilterProxyModel *getProxy() const {
//    return m_proxy;
//  }

//  void sortBookmark();

  bool isUsed() const {
    return m_isUsed;
  }
  void setUsed(bool on) {
    m_isUsed = on;
  }

//  void appendBookmark(const ZFlyEmBookmark &bookmark);

  //void load(const QString &filePath);

signals:

public slots:

private:
  void init();

private:
  ZFlyEmBookmarkPtrArray m_bookmarkArray;
//  ZSortFilterProxyModel *m_proxy;
//  ZFlyEmBookmarkArray m_bookmarkArray;
  ZFlyEmBookmarkPresenter *m_presenter;
  ZFlyEmBookmarkPresenter *m_defaultPresenter;
  bool m_isUsed = false;
};

#endif // ZFLYEMBOOKMARKLISTMODEL_H
