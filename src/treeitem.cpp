#include "include/treeitem.h"

#include <QStringList>
#include <QDebug>

char *TreeItem::copyFromQString(QString str) {
    char *buf = new char[str.length()+2];

    if ( buf ) {
        strcpy(buf, str.toStdString().c_str());
    }

    return buf;
}

TreeItem::TreeItem(const TreeItemType type, void *ptr, TreeItem *parentItem) {
    m_parentItem = parentItem;
    m_type = type;
    m_itemData = ptr;
}

TreeItem::TreeItem(const TreeItemType type, TreeItem *parent)
{
    m_parentItem = parent;
    m_type = type;

    message_t *msg;
    message_list_t *msglst;

    switch ( type ) {
    case TREE_ITEM_MESSAGE:

        msglst = (message_list_t*) parent->getItemData();

        if ( msglst == NULL ) {
            msglst = new message_list_t;
            parent->setItemData((void*)msglst);
            ((dbc_t*)parent->parentItem()->getItemData())->message_list = msglst;

            memset(msglst, 0, sizeof(message_list_t));
        } else {
            while ( msglst->next != NULL ) {
                msglst = msglst->next;
            }

            msglst->next = new message_list_t;
            memset(msglst->next, 0, sizeof(message_list_t));
            msglst = msglst->next;
        }

        msglst->message = new message_t;
        m_itemData = msglst->message;

        ((message_t*)m_itemData)->name = copyFromQString(QString("New message"));
        ((message_t*)m_itemData)->id = 0;
        ((message_t*)m_itemData)->len = 0;
        ((message_t*)m_itemData)->comment = copyFromQString(QString(""));
        ((message_t*)m_itemData)->attribute_list = NULL;
        ((message_t*)m_itemData)->sender = copyFromQString(QString("Vector__XXX"));
        ((message_t*)m_itemData)->signal_list = NULL;
        ((message_t*)m_itemData)->transmitter_list = NULL;

        break;

    case TREE_ITEM_SIGNAL:

        msg = (message_t*)parent->getItemData();

        if ( msg ) {
            signal_list_t *lst = msg->signal_list;
            if ( lst ) {
                while ( lst->next ) {
                    lst = lst->next;
                }

                lst->next = new signal_list_t;
                memset(lst->next, 0, sizeof(signal_list_t));
                m_itemData = (void*)lst->next;
            } else {
                msg->signal_list = new signal_list_t;
                memset(msg->signal_list, 0, sizeof(signal_list_t));
                m_itemData = (void*)msg->signal_list;
            }

            ((signal_list_t*)m_itemData)->signal = new signal_t;
            memset(((signal_list_t*)m_itemData)->signal, 0, sizeof(signal_t));

            ((signal_list_t*)m_itemData)->signal->attribute_list = NULL;
            ((signal_list_t*)m_itemData)->signal->bit_len = 0;
            ((signal_list_t*)m_itemData)->signal->bit_start = 0;
            ((signal_list_t*)m_itemData)->signal->comment = copyFromQString(QString(""));
            ((signal_list_t*)m_itemData)->signal->endianess = 0;
            ((signal_list_t*)m_itemData)->signal->max = 0;
            ((signal_list_t*)m_itemData)->signal->min = 0;
            ((signal_list_t*)m_itemData)->signal->mux_type = m_signal;
            ((signal_list_t*)m_itemData)->signal->mux_value = 0;
            ((signal_list_t*)m_itemData)->signal->name = copyFromQString(QString("New signal"));
            ((signal_list_t*)m_itemData)->signal->offset = 0;
            ((signal_list_t*)m_itemData)->signal->scale = 0;
            ((signal_list_t*)m_itemData)->signal->signal_val_type = svt_integer;
            ((signal_list_t*)m_itemData)->signal->signedness = 0;
            ((signal_list_t*)m_itemData)->signal->unit = copyFromQString(QString(""));
            ((signal_list_t*)m_itemData)->signal->receiver_list = new string_list_t;
            ((signal_list_t*)m_itemData)->signal->receiver_list->next = NULL;
            ((signal_list_t*)m_itemData)->signal->receiver_list->string = copyFromQString(QString("Vector__XXX"));
        }

        break;

    case TREE_ITEM_MESSAGES_ROOT:
    case TREE_ITEM_ROOT:
    default:
        m_itemData = NULL;
        break;
    }
}

TreeItem::~TreeItem()
{
    if ( m_type == TREE_ITEM_MESSAGE ) {
        message_t *ptr = (message_t*)m_itemData;

        message_list_t *lst = ((message_list_t*)m_parentItem->getItemData());

        if ( lst->message == ptr ) {
            m_parentItem->setItemData((void*)lst->next);
        } else {
            // Remove element from message_list
            while ( lst->next ) {
                if ( lst->next->message == ptr ) {
                    lst->next = lst->next->next;
                }
                lst = lst->next;
            }
        }

        //Delete all child nodes
        //ptr->signal_list is deleted there
        qDeleteAll(m_childItems);

        attribute_list_t *p = ptr->attribute_list;
        while ( p != NULL )  {
            delete p->attribute->name;
            delete p->attribute;
            attribute_list_t *k = p->next;
            delete p;
            p=k;
        }

        string_list_t *s = ptr->transmitter_list;
        while ( s != 0 ) {
            delete s->string;
            string_list_t *k = s->next;
            delete s;
            s=k;
        }

        delete ptr->name;
        delete ptr->comment;
        delete ptr->sender;
        delete ptr;
        m_parentItem->deleteChild(this);
    } else if ( m_type == TREE_ITEM_SIGNAL ) {
        signal_list_t *ptr = (signal_list_t*)m_itemData;

        signal_list_t *lst = ((message_t*)m_parentItem->getItemData())->signal_list;
        if ( lst == ptr ) {
            ((message_t*)m_parentItem->getItemData())->signal_list = lst->next;
        } else {
            while ( lst->next != NULL ) {
                if ( lst->next == ptr ) {
                    lst->next = lst->next->next;
                } else {
                    lst = lst->next;
                }
            }
        }

        attribute_list_t *p = ptr->signal->attribute_list;
        while ( p != NULL )  {
            delete p->attribute->name;
            delete p->attribute;
            attribute_list_t *k = p->next;
            delete p;
            p=k;
        }

        string_list_t *s = ptr->signal->receiver_list;
        while ( s != 0 ) {
            delete s->string;
            string_list_t *k = s->next;
            delete s;
            s=k;
        }

        val_map_t *v = ptr->signal->val_map;
        while ( s != 0 ) {
            delete v->val_map_entry->value;
            val_map_t *k = v->next;
            delete v;
            v=k;
        }


        delete ptr->signal->comment;
        delete ptr->signal->name;
        delete ptr->signal;
        delete ptr;
        m_parentItem->deleteChild(this);
    } else {
        // All toplevel items
        qDeleteAll(m_childItems);
    }

}

void TreeItem::deleteChild(TreeItem *child) {
    int i = m_childItems.indexOf(child);
    if ( i >= 0 ) {
        m_childItems.removeAt(i);
    }
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    switch ( m_type ) {
    case TREE_ITEM_MESSAGE:
    case TREE_ITEM_SIGNAL:
    case TREE_ITEM_MESSAGES_ROOT:
    case TREE_ITEM_ROOT:
        return 11;
    default:
        qDebug() << "Wrong type in " << __FILE__ << " line " << __LINE__;
        return 0;
    }
}

bool TreeItem::setData(int column, const QVariant &value)
{
    switch ( m_type ) {
    case TREE_ITEM_MESSAGE:
        if ( column == 0 ) {
            delete ((message_t*)m_itemData)->name;
            ((message_t*)m_itemData)->name = copyFromQString(value.value<QString>());
        } else if ( column == 1 ) {
            ((message_t*)m_itemData)->id = value.value<qint32>();
        } else if ( column == 2 ) {
            ((message_t*)m_itemData)->len = value.value<qint8>();
        } else if ( column == 10 ) {
            delete ((message_t*)m_itemData)->comment;
            ((message_t*)m_itemData)->comment = copyFromQString(value.value<QString>());
        } else {
            return false;
        }
        break;
    case TREE_ITEM_SIGNAL:
        if ( column == 0 ) {            // Name
            delete ((signal_list_t*)m_itemData)->signal->name;
            ((signal_list_t*)m_itemData)->signal->name = copyFromQString(value.value<QString>());
        } else if ( column == 1 ) {     // Startbit
            ((signal_list_t*)m_itemData)->signal->bit_start = value.value<qint8>();
        } else if ( column == 2 ) {     // Length
            ((signal_list_t*)m_itemData)->signal->bit_len = value.value<quint8>();
        } else if ( column == 3 ) {     // Value type
            quint8 t = (signal_val_type_t)value.value<quint8>();
            switch (t) {
            case 0:
                ((signal_list_t*)m_itemData)->signal->signal_val_type = svt_integer;
                ((signal_list_t*)m_itemData)->signal->signedness = 0; // unsigned
                break;
            case 1:
                ((signal_list_t*)m_itemData)->signal->signal_val_type = svt_integer;
                ((signal_list_t*)m_itemData)->signal->signedness = 1; // signed
                break;
            case 2:
                ((signal_list_t*)m_itemData)->signal->signal_val_type = svt_float;
                break;
            case 3:
                ((signal_list_t*)m_itemData)->signal->signal_val_type = svt_double;
                break;
            default:
                ((signal_list_t*)m_itemData)->signal->signal_val_type = svt_integer;
            }
        } else if ( column == 4 ) {     // Endianess
            ((signal_list_t*)m_itemData)->signal->endianess = value.value<quint8>();
        } else if ( column == 5 ) {     // Factor
            ((signal_list_t*)m_itemData)->signal->scale = value.value<double>();
        } else if ( column == 6 ) {     // Offset
            ((signal_list_t*)m_itemData)->signal->offset = value.value<double>();
        } else if ( column == 7 ) {     // Min value
            ((signal_list_t*)m_itemData)->signal->min = value.value<double>();
        } else if ( column == 8 ) {     // Max value
            ((signal_list_t*)m_itemData)->signal->max = value.value<double>();
        } else if ( column == 9 ) {     // Unit
            delete ((signal_list_t*)m_itemData)->signal->unit;
            ((signal_list_t*)m_itemData)->signal->unit = copyFromQString(value.value<QString>());
        } else if ( column == 10 ) {    // Comment
            delete ((signal_list_t*)m_itemData)->signal->comment;
            ((signal_list_t*)m_itemData)->signal->comment = copyFromQString(value.value<QString>());
        } else {
            return false;
        }
        break;
    case TREE_ITEM_MESSAGES_ROOT:
        return false;
    case TREE_ITEM_ROOT:
        return false;
    default:
        qDebug() << "Wrong type in " << __FILE__ << " line " << __LINE__;
        return false;
    }

    return true;
}

QVariant TreeItem::data(int column) const
{
    switch ( m_type ) {
    case TREE_ITEM_MESSAGE:
        if ( column == 0 ) return QString(((message_t*)m_itemData)->name);
        if ( column == 1 ) return (quint32)((message_t*)m_itemData)->id;
        if ( column == 2 ) return ((message_t*)m_itemData)->len;
        if ( column == 10 ) return QString(((message_t*)m_itemData)->comment);
        return QString("");
    case TREE_ITEM_SIGNAL:
        if ( column == 0 ) return QString(((signal_list_t*)m_itemData)->signal->name);
        if ( column == 1 ) return ((signal_list_t*)m_itemData)->signal->bit_start;
        if ( column == 2 ) return ((signal_list_t*)m_itemData)->signal->bit_len;
        if ( column == 3 ) {
            signal_val_type_t t = ((signal_list_t*)m_itemData)->signal->signal_val_type;
            if ( t == svt_integer ) {
                if ( ((signal_list_t*)m_itemData)->signal->signedness == 0 ) {
                    return 0;   // Unsigned
                } else {
                    return 1;   // Signed
                }
            } else if ( t == svt_float ) {
                return 2;
            } else if ( t == svt_double ) {
                return 3;
            }
        }
        if ( column == 4 ) return (quint8)((signal_list_t*)m_itemData)->signal->endianess;
        if ( column == 5 ) return ((signal_list_t*)m_itemData)->signal->scale;
        if ( column == 6 ) return ((signal_list_t*)m_itemData)->signal->offset;
        if ( column == 7 ) return ((signal_list_t*)m_itemData)->signal->min;
        if ( column == 8 ) return ((signal_list_t*)m_itemData)->signal->max;
        if ( column == 9 ) return QString(((signal_list_t*)m_itemData)->signal->unit);
        if ( column == 10 ) return QString(((signal_list_t*)m_itemData)->signal->comment);
        return QString("");
    case TREE_ITEM_MESSAGES_ROOT:
        if ( column == 0 ) return QString("Messages");
        return QString("");
    case TREE_ITEM_ROOT:
        if ( column == 0 ) return QString("Name");
        return QString("");
    default:
        qDebug() << "Wrong type in " << __FILE__ << " line " << __LINE__;
        return QString("Oooppss!!");
    }
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

void TreeModel::addMessage(QString name) {
    TreeItem *item = new TreeItem(TREE_ITEM_MESSAGE, messagesItem);
    messagesItem->appendChild(item);
    item->setData(0, name);
    m_changed = true;

}

bool TreeModel::addSignal(QModelIndex index, QString name) {
    if (!index.isValid())
        return false;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if ( item->getType() == TREE_ITEM_SIGNAL )  {
        item = item->parentItem();
        if ( !item ) {
            return false;
        }
    }

    if ( item->getType() == TREE_ITEM_MESSAGE )  {
        TreeItem *newitem = new TreeItem(TREE_ITEM_SIGNAL, item);
        item->appendChild(newitem);
        newitem->setData(0, name);
        m_changed = true;
    } else {
        return false;
    }

    return true;
}

TreeItemType TreeModel::getItemType(const QModelIndex index) {
    if (!index.isValid())
        return TREE_ITEM_ROOT;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->getType();
}

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = NULL;
    messagesItem = NULL;
    NewEmptyTree();
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if (role == Qt::EditRole)
    {
        return index.data(Qt::DisplayRole);
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    TreeItem *item = getItem(index);

    if ( item ) {
        if ( item->getType() == TREE_ITEM_MESSAGE || item->getType() == TREE_ITEM_SIGNAL ) {
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        }
    }

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    m_changed = true;

    if (result)
        emit dataChanged(index, index);

    return result;
}


void TreeModel::NewEmptyTree() {
    if ( rootItem != NULL ) {
        delete rootItem;
    }

    m_dbc = new dbc_t;
    memset(m_dbc, 0, sizeof(dbc_t));

    m_dbc->version = new char[3];
    strcpy(m_dbc->version, "04");

    m_dbc->network = new network_t;
    memset(m_dbc->network, 0, sizeof(network_t));

    rootItem = new TreeItem(TREE_ITEM_ROOT, (void*)m_dbc);
    messagesItem = new TreeItem(TREE_ITEM_MESSAGES_ROOT, (void*)(m_dbc->message_list), rootItem);
    rootItem->appendChild(messagesItem);

    m_changed = false;
}

TreeModel::TreeModel(QString filename, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = NULL;
    messagesItem = NULL;
    m_changed = false;

    char *s = new char[filename.length()+2];
    if ( !s ) {
        NewEmptyTree();
    }

    strcpy(s, filename.toStdString().c_str());
    try {
        m_dbc = dbc_read_file(s);
    } catch (...) {
        m_dbc = NULL;
    }

    delete s;

    if ( !m_dbc ) {
        NewEmptyTree();
        return;
    }

    rootItem = new TreeItem(TREE_ITEM_ROOT);
    messagesItem = new TreeItem(TREE_ITEM_MESSAGES_ROOT, (void*)(m_dbc->message_list), rootItem);

    rootItem->appendChild(messagesItem);

    message_list_t *msg = m_dbc->message_list;
    TreeItem *item;

    while ( msg ) {
        signal_list_t *sig = msg->message->signal_list;
        // Add message
        item = new TreeItem(TREE_ITEM_MESSAGE, (void*)msg->message, messagesItem);
        messagesItem->appendChild(item);

        while ( sig ) {
            // Add signal
            item->appendChild(new TreeItem(TREE_ITEM_SIGNAL, (void*)sig, item));

            sig = sig->next;
        }

        msg = msg->next;
    }
}

bool TreeModel::SaveToFile(QString filename) {
    bool result = true;

    if ( !m_dbc ) {
        return false;
    }

    FILE *f = fopen(filename.toStdString().c_str(), "w");

    if ( !f ) {
        return false;
    }

    try {
        dbc_write(f, m_dbc);
        m_changed = false;
    } catch (...) {
        result = false;
    }

    fclose(f);

    return result;
}

bool TreeModel::deleteItemItem(TreeItem *item) {
    if ( !item ) {
        return false;
    }
    if ( item->getType() == TREE_ITEM_ROOT || item->getType() == TREE_ITEM_MESSAGES_ROOT ) {
        return false;
    }

    delete item;

    return true;
}

bool TreeModel::deleteItem(QModelIndex &index) {
    qDebug() << "Delete " << index;
    if (!index.isValid())
        return false;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    beginRemoveRows(index, 0, 100);
    bool result = deleteItemItem(item);
    endRemoveRows();
    return result;

}
