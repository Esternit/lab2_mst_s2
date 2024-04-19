#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QCloseEvent>
#include <QDialog>
#include <QFile>
#include <QUrl>

#include "IniFile.h"
#include <map>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sectionAdderBox->setVisible(false);
    ui->fileAdderBox->setVisible(false);
    saved = true;
    connect(ui->listWidget,&QListWidget::itemClicked,this, &MainWindow::on_listWidget_itemClicked);
    connect(ui->addButton, &QPushButton::clicked,this,&MainWindow::add_button_clicked);
    connect(ui->comboSectionsBox, &QComboBox::currentTextChanged,this,&MainWindow::section_choice_changed);
    connect(ui->deleteKeyButton, &QPushButton::clicked,this,&MainWindow::delete_key_button_clicked);
    connect(ui->openDirectoryButton, &QPushButton::clicked, this, &MainWindow::on_open_directory_clicked);
    connect(ui->saveCurFile,&QPushButton::clicked,this,&MainWindow::save_file_button_clicked);
    connect(ui->addSectionButton,&QPushButton::clicked, this,&MainWindow::add_section_button_clicked);
    connect(ui->addSectionActionButton,&QPushButton::clicked, this,&MainWindow::add_active_section_button_clicked);
    connect(ui->cancelSectionAddingButton,&QPushButton::clicked, this,&MainWindow::cancel_adding_section_button_clicked);
    connect(ui->addFileButtonTrigger, &QPushButton::clicked,this,&MainWindow::add_file_trigger_button_clicked);
    connect(ui->addFileButton, &QPushButton::clicked,this,&MainWindow::add_file_button_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chooseFile_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open File", "C://Dima/C++/MST/lab1_mst_s2");
    QListWidget* itemsList = ui->listWidget;
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(file_name);
    itemsList->addItem(newItem);
    itemsList->setCurrentItem(newItem);

    IniFile someFile = IniFile(file_name.toStdString());
    workingFiles[file_name] = someFile;
    updateInfo(workingFiles[file_name]);
    this->changeTitle(1);
    ui->openedFilesInfo->setText("Files opened: " + QString::number(workingFiles.size()));
    ui->deleteKeyGroupBox->setEnabled(true);
    ui->createKeyGroupBox->setEnabled(true);
    ui->addSectionButton->setEnabled(true);
    ui->cancelSectionAddingButton->setEnabled(true);
    ui->addFileButtonTrigger->setEnabled(true);

}

void MainWindow::updateInfo(IniFile file_name){
    this->changeTitle(0);
    //someFile = IniFile(file_name.toStdString());
    //IniFile someFile(file_name.toStdString());
    QTreeWidget* file_info = ui->documentTreeView;
    file_info->clear();

    std::map<std::string, KeysMap> data = file_name.getSections();

    file_info->setColumnCount(2);

    QStringList headerLabels = {"keys", "values"};
    QComboBox* sections_box = ui->comboSectionsBox;
    sections_box->clear();


    file_info->setHeaderLabels(headerLabels);
    for (const auto& element : data){
        //std::cout << element.first << std::endl;
        QTreeWidgetItem* root = new QTreeWidgetItem();

        root->setText(0,QString::fromStdString(element.first));
        sections_box->addItem(QString::fromStdString(element.first),0);
        for (const auto& elem : element.second){
            QTreeWidgetItem* child = new QTreeWidgetItem();
            //std::cout << elem.first << "=" << elem.second << std::endl;
            child->setText(0,QString::fromStdString(elem.first));
            child->setText(1,QString::fromStdString(elem.second));
            root->addChild(child);

        }

        file_info->addTopLevelItem(root);
        root->setExpanded(true);
    }

}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    //IniFile someFile = IniFile(item->text().toStdString());
    this->updateInfo(workingFiles[item->text()]);
    //this->changeTitle(1);


}

void MainWindow::add_button_clicked(bool clicked){
    QList sel = ui->listWidget->selectedItems();
    //IniFile someFile(sel[0]->text().toStdString());
    //std::map<std::string, KeysMap> data = workingFiles[sel[0]->text()].getSections();

    //IniFile someFile = workingFiles[sel[0]->text()];

    QString type_select = ui->typeEdit->currentText();
    QString section_select = ui->comboSectionsBox->currentText();
    QString key_select = ui->keyEdit->text();
    QString value_select = ui->valueEdit->text();
    //std::cout << key_select.toStdString() << std::endl;
    if(type_select == "int"){
        workingFiles[sel[0]->text()].writeInt(section_select.toStdString(),key_select.toStdString(),value_select.toInt());
    }
    else if(type_select == "double"){
        workingFiles[sel[0]->text()].writeDouble(section_select.toStdString(),key_select.toStdString(),value_select.toDouble());
    }
    else if(type_select == "bool"){
        for (const auto &trueValue : trueValues)
        {
            if (value_select.toStdString() == trueValue)
            {
                workingFiles[sel[0]->text()].writeBool(section_select.toStdString(),key_select.toStdString(),true);
            }
        }
        workingFiles[sel[0]->text()].writeBool(section_select.toStdString(),key_select.toStdString(),false);
    }
    else if(type_select == "string"){
        workingFiles[sel[0]->text()].writeString(section_select.toStdString(),key_select.toStdString(),value_select.toStdString());
    }

    //someFile.save();

    //QList sel = ui->listWidget->selectedItems();
    //std::cout << sel[0]->text().toStdString() << std::endl;
    this->updateInfo(workingFiles[sel[0]->text()]);
}

void MainWindow::section_choice_changed(const QString &text){
    //std::cout << text.toStdString() << std::endl;
    QList sel = ui->listWidget->selectedItems();
    std::map<std::string, KeysMap> data = workingFiles[sel[0]->text()].getSections();

    QComboBox* keys_box = ui->deleteKeyBox;
    keys_box->clear();

    for (const auto& element : data){
        for (const auto& elem : element.second){
            if(element.first == text.toStdString()){
                keys_box->addItem(QString::fromStdString(elem.first),0);
            }

        }
    }
}


void MainWindow::delete_key_button_clicked(bool clicked){
    QString section_select = ui->comboSectionsBox->currentText();
    QString key_select = ui->deleteKeyBox->currentText();
    QList sel = ui->listWidget->selectedItems();


    bool res = workingFiles[sel[0]->text()].deleteKey(section_select.toStdString(),key_select.toStdString());
    this->updateInfo(workingFiles[sel[0]->text()]);
}

void MainWindow::changeTitle(int changed){
    if(changed == 0){
        saved = false;
        QWidget::setWindowTitle("MainWindow *");
    }
    else{
        saved = true;
        QWidget::setWindowTitle("MainWindow");
    }
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    if(saved == false){
        QMessageBox::StandardButton resBtn =
            QMessageBox::question( this, "Confirmation",tr("Save Before Exiting?\n"),
                                  QMessageBox::No | QMessageBox::Yes,
                                  QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            for(auto& element : workingFiles){
                element.second.save();
            }
            //someFile.save();
            this->changeTitle(1);
            event->ignore();
        }
        else
        {
            event->accept();
        }
    }

}

void MainWindow::on_open_directory_clicked(){
    QDir dir_name = QFileDialog::getExistingDirectory(this,"Open Dir", "C://");
    QStringList images = dir_name.entryList(QStringList() << "*.ini" << "*.INI",QDir::Files);
    foreach(QString file_name, images) {
        QListWidget* itemsList = ui->listWidget;
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(dir_name.absoluteFilePath(file_name));
        itemsList->addItem(newItem);
        itemsList->setCurrentItem(newItem);

        IniFile someFile = IniFile(dir_name.absoluteFilePath(file_name).toStdString());
        workingFiles[dir_name.absoluteFilePath(file_name)] = someFile;
        updateInfo(workingFiles[dir_name.absoluteFilePath(file_name)]);
        this->changeTitle(1);


    }
    ui->openedFilesInfo->setText("Files opened: " + QString::number(workingFiles.size()));
    ui->deleteKeyGroupBox->setEnabled(true);
    ui->createKeyGroupBox->setEnabled(true);
    ui->addSectionButton->setEnabled(true);
    ui->cancelSectionAddingButton->setEnabled(true);
    ui->addFileButtonTrigger->setEnabled(true);
}

void MainWindow::save_file_button_clicked(bool clicked){
    for(auto& element : workingFiles){
        element.second.save();
    }
}

void MainWindow::add_section_button_clicked(bool clicked){
    ui->sectionAdderBox->setVisible(true);
    ui->sectionAdderBox->setEnabled(true);
}


void MainWindow::cancel_adding_section_button_clicked(bool clicked){
    QList sel = ui->listWidget->selectedItems();
    QString section = ui->comboSectionsBox->currentText();
    workingFiles[sel[0]->text()].deleteSection(section.toStdString());
    this->updateInfo(workingFiles[sel[0]->text()]);

}

void MainWindow::add_active_section_button_clicked(bool clicked){
    QString section_name = ui->sectionNameInput->text();
    QList sel = ui->listWidget->selectedItems();

    if(section_name.length() > 0){
        workingFiles[sel[0]->text()].addNewSection(section_name.toStdString());
    }
    ui->sectionAdderBox->setVisible(false);
    ui->sectionAdderBox->setEnabled(false);
    this->updateInfo(workingFiles[sel[0]->text()]);
}

void MainWindow::add_file_trigger_button_clicked(bool clicked){
    ui->fileAdderBox->setVisible(true);
    ui->fileAdderBox->setEnabled(true);
}

void MainWindow::add_file_button_clicked(bool clicked){
    QString file_name = ui->fileNameInput->text();
    //std::cout << workingFiles.size() << std::endl;
    QList sel = ui->listWidget->selectedItems();
    QString dir = sel[0]->text();
    std::cout << dir.toStdString() << std::endl;
    QString basename = QUrl(dir).fileName();
    QString basepath = dir.remove(basename);
    QString name = basepath + file_name;
    if(!file_name.contains(".ini")){

        name = name + ".ini";
    }
    std::cout << name.toStdString() << std::endl;
    if(workingFiles.find(name) == workingFiles.end()){

        QFile file(name);
        file.open(QIODevice::WriteOnly);
        file.close();
        workingFiles[name] = IniFile(name.toStdString());
        std::cout << "checked" << std::endl;
    }
    QListWidget* itemsList = ui->listWidget;
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(name);
    itemsList->addItem(newItem);
    itemsList->setCurrentItem(newItem);
    this->updateInfo(workingFiles[name]);
    ui->fileAdderBox->setVisible(false);
    ui->fileAdderBox->setEnabled(false);
}
