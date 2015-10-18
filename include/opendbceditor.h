#ifndef OPENDBCEDITOR_H
#define OPENDBCEDITOR_H

#include <QMainWindow>
#include <QItemSelection>
#include <QDataWidgetMapper>

#include "include/treeitem.h"

namespace Ui {
class OpenDBCEditor;
}

class OpenDBCEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit OpenDBCEditor(QWidget *parent = 0);
    ~OpenDBCEditor();

private slots:
    void on_actionInsert_Message_triggered();

    void on_actionInsert_Signal_triggered();

    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    void on_pushButton_clicked();

    void on_action_Open_triggered();

    void on_action_Save_as_triggered();

    void on_pushButton_ApplyMessage_clicked();

    void on_action_Save_triggered();

    void on_action_About_triggered();

    void on_actionDelete_triggered();

    void on_action_New_triggered();

    void on_action_Help_triggered();

private:
    void RefreshTree();
    void SetupMapper();

    void SetupCleanModel();

    bool Save();
    bool SaveAs();
    bool CheckForChanges();

    bool m_changed;
    bool m_saved;
    QString m_filename;

    Ui::OpenDBCEditor *ui;
    TreeModel *m_model;
    QDataWidgetMapper *m_mapper_signal, *m_mapper_message;
};

#endif // OPENDBCEDITOR_H
