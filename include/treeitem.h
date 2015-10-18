#ifndef TREEITEM_H
#define TREEITEM_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>

#include "src/libcandbc/dbcModel.h"
#include "src/libcandbc/dbcReader.h"
#include "src/libcandbc/dbcWriter.h"

class TreeItem;

enum TreeItemType { TREE_ITEM_ROOT, TREE_ITEM_MESSAGES_ROOT, TREE_ITEM_MESSAGE, TREE_ITEM_SIGNAL };
enum SignalDataType { SD_SIGNED=0, SD_UNSIGNED, SD_FLOAT32, SD_FLOAT64 };
enum SignalDataEndianess { SD_BIG_ENDIAN=0, SD_LITTLE_ENDIAN };

/*
struct TreeItemMessage {
    qint32 id;
    QString name;
    qint8 size;
    QString comment;
};

struct TreeItemSignal {
    QString name;
    qint8 start_bit;
    quint8 length;
    SignalDataType type;
    SignalDataEndianess endianess;
    double factor;
    double offset;
    double min;
    double max;
    QString unit;
};
*/

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(QObject *parent = 0);
    explicit TreeModel(QString filename, QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    TreeItem* getItem(const QModelIndex &index) const;

    void addMessage(QString name="New message");
    bool addSignal(QModelIndex index, QString name="New signal");

    TreeItemType getItemType(const QModelIndex index);

    bool isChanged() { return m_changed; }
    void setChanged(bool changed) { m_changed = changed; }
    bool SaveToFile(QString filename);

    bool deleteItem(QModelIndex &index);
    bool deleteItemItem(TreeItem *item);


private:
    void setupModelData(const QStringList &lines, TreeItem *parent);
    void NewEmptyTree();

    TreeItem *rootItem;
    TreeItem *messagesItem;

    bool m_changed;
    dbc_t *m_dbc;
};


class TreeItem
{
public:
    explicit TreeItem(const TreeItemType type, TreeItem *parentItem = 0);
    explicit TreeItem(const TreeItemType type, void *ptr, TreeItem *parentItem = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);
    void deleteChildren() { qDeleteAll(m_childItems); }
    void deleteChild(TreeItem *child);
    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant &value);

    int row() const;
    TreeItem *parentItem();
    void *getItemData() { return m_itemData; }
    void setItemData(void *data) { m_itemData = data; }

    TreeItemType getType() { return m_type; }

private:
    char *copyFromQString(QString str);

    QList<TreeItem*> m_childItems;
    void* m_itemData;
    TreeItem *m_parentItem;
    TreeItemType m_type;
};

#endif // TREEITEM_H
