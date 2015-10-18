#include <QDebug>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

#include <float.h>

#include "../include/opendbceditor.h"
#include "../include/dialogabout.h"
#include "ui_opendbceditor.h"


OpenDBCEditor::OpenDBCEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OpenDBCEditor)
{
    ui->setupUi(this);

    m_model = NULL;
    m_mapper_signal = new QDataWidgetMapper();
    m_mapper_message = new QDataWidgetMapper();

    SetupCleanModel();

    // Set min/max values
    ui->doubleSpinBox_SignalFactor->setMinimum(DBL_MAX);
    ui->doubleSpinBox_SignalFactor->setMinimum(DBL_MIN);
    ui->doubleSpinBox_SignalMax->setMinimum(DBL_MAX);
    ui->doubleSpinBox_SignalMax->setMinimum(DBL_MIN);
    ui->doubleSpinBox_SignalMin->setMinimum(DBL_MAX);
    ui->doubleSpinBox_SignalMin->setMinimum(DBL_MIN);
    ui->doubleSpinBox_SignalOffset->setMinimum(DBL_MAX);
    ui->doubleSpinBox_SignalOffset->setMinimum(DBL_MIN);

    ui->stackedWidget->setCurrentWidget(ui->page_Info);
}

OpenDBCEditor::~OpenDBCEditor()
{
    delete ui;
}

void OpenDBCEditor::SetupMapper() {
    m_mapper_signal->setModel(m_model);
    m_mapper_message->setModel(m_model);

    m_mapper_signal->addMapping(ui->lineEdit_SignalName, 0);
    m_mapper_signal->addMapping(ui->spinBox_SignalStartbit, 1);
    m_mapper_signal->addMapping(ui->spinBox_SignalLength, 2);
    m_mapper_signal->addMapping(ui->comboBox_SignalType, 3, "currentIndex");
    m_mapper_signal->addMapping(ui->comboBox_SignalEndianess, 4, "currentIndex");
    m_mapper_signal->addMapping(ui->doubleSpinBox_SignalFactor, 5);
    m_mapper_signal->addMapping(ui->doubleSpinBox_SignalOffset, 6);
    m_mapper_signal->addMapping(ui->doubleSpinBox_SignalMin, 7);
    m_mapper_signal->addMapping(ui->doubleSpinBox_SignalMax, 8);
    m_mapper_signal->addMapping(ui->lineEdit_SignalUnit, 9);
    m_mapper_signal->addMapping(ui->lineEdit_SignalComment, 10);    // 10 is comment field

    m_mapper_message->addMapping(ui->lineEdit_MessageName, 0);
    m_mapper_message->addMapping(ui->spinBox_MessageID, 1);
    m_mapper_message->addMapping(ui->spinBox_MessageLen, 2);
    m_mapper_message->addMapping(ui->lineEdit_MessageComment, 10);// 10 is comment field

    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
    m_mapper_signal->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper_message->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

void OpenDBCEditor::SetupCleanModel() {
    ui->treeView->setModel(NULL);
    if ( m_model ) {
        delete m_model;
    }

    m_changed = false;
    m_saved = false;
    m_filename = "";

    m_model = new TreeModel();
    ui->treeView->setModel(m_model);

    SetupMapper();

    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->hideColumn(4);
    ui->treeView->hideColumn(5);
    ui->treeView->hideColumn(6);
    ui->treeView->hideColumn(7);
    ui->treeView->hideColumn(8);
    ui->treeView->hideColumn(9);
    // 10 is comment field
}

void OpenDBCEditor::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    Q_UNUSED(deselected);

    QModelIndex idx = selected.at(0).indexes().last();

    if ( m_model->getItemType(idx) == TREE_ITEM_MESSAGE ) {
        ui->stackedWidget->setCurrentWidget(ui->page_Message);
        m_mapper_message->setRootIndex(idx.parent());
        m_mapper_message->setCurrentModelIndex(idx);
    } else if ( m_model->getItemType(idx) == TREE_ITEM_SIGNAL ) {
        ui->stackedWidget->setCurrentWidget(ui->page_Signal);
        m_mapper_signal->setRootIndex(idx.parent());
        m_mapper_signal->setCurrentModelIndex(idx);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_Info);
    }
}

void OpenDBCEditor::on_actionInsert_Message_triggered()
{
    m_model->addMessage("New message");
    RefreshTree();
}

void OpenDBCEditor::on_actionInsert_Signal_triggered()
{
    QModelIndex idx = ui->treeView->currentIndex();
    m_model->addSignal(idx, "New signal");
    RefreshTree();
}

void OpenDBCEditor::RefreshTree() {
    ui->treeView->setModel(NULL);
    ui->treeView->setModel(m_model);
    ui->treeView->expandAll();

    SetupMapper();

    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->hideColumn(4);
    ui->treeView->hideColumn(5);
    ui->treeView->hideColumn(6);
    ui->treeView->hideColumn(7);
    ui->treeView->hideColumn(8);
    ui->treeView->hideColumn(9);
}

bool OpenDBCEditor::CheckForChanges() {
    if ( m_model->isChanged() ) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Data has been changed. Save?", "Save?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            return Save();
        } else {
            return true;
        }
    }

    return true;
}

void OpenDBCEditor::on_action_New_triggered()
{
    if ( !CheckForChanges() ) {
        return;
    }
    m_saved = false;
    ui->treeView->setModel(NULL);
    delete m_model;
    m_model = new TreeModel();
    ui->treeView->setModel(m_model);
    RefreshTree();
}

void OpenDBCEditor::on_action_Open_triggered()
{
    if ( !CheckForChanges() ) {
        return;
    }

    m_filename = QFileDialog::getOpenFileName(this,
        tr("Open DBC"), "", tr("DBC Files (*.dbc)"));

    if ( m_filename != "" ) {
        m_saved = true;
        ui->treeView->setModel(NULL);
        delete m_model;
        m_model = new TreeModel(m_filename);
        ui->treeView->setModel(m_model);
        RefreshTree();
    }
}

bool OpenDBCEditor::SaveAs() {
    QString filename = QFileDialog::getSaveFileName(this,
              tr("Open DBC"), "", tr("DBC Files (*.dbc)"));

    if ( filename != "" ) {
        m_filename = filename;
        m_saved = true;
        return Save();
    }
    return false;
}

bool OpenDBCEditor::Save() {
    if ( !m_saved ) {
        return SaveAs();
    } else {
        if ( m_model->SaveToFile(m_filename) )
        {
            m_changed = false;
            m_model->setChanged(false);
            return true;
        } else {
            return false;
        }
    }
}

void OpenDBCEditor::on_action_Save_as_triggered()
{
    SaveAs();
}

void OpenDBCEditor::on_pushButton_clicked()
{
    m_mapper_signal->submit();
}

void OpenDBCEditor::on_pushButton_ApplyMessage_clicked()
{
    m_mapper_message->submit();
}

void OpenDBCEditor::on_action_Save_triggered()
{
    Save();
}

void OpenDBCEditor::on_action_About_triggered()
{
    DialogAbout about(this);
    about.exec();
}

void OpenDBCEditor::on_actionDelete_triggered()
{
    if ( ui->treeView->selectionModel()->selection().size() > 0 ) {
        QModelIndex idx = ui->treeView->selectionModel()->selection().at(0).indexes().last();
        m_model->deleteItem(idx);
        RefreshTree();
    }
}


void OpenDBCEditor::on_action_Help_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.hans-dampf.org"));
}
