#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "IniFile.h"
#include <QListWidget>
#include <QCloseEvent>
#include <QPushButton>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_chooseFile_clicked();
    void add_button_clicked(bool clicked);
    void delete_key_button_clicked(bool clicked);
    void section_choice_changed(const QString &text);
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void closeEvent (QCloseEvent *event);
    void on_open_directory_clicked();
    void save_file_button_clicked(bool clicked);
    void add_section_button_clicked(bool clicked);
    void add_active_section_button_clicked(bool clicked);
    void cancel_adding_section_button_clicked(bool clicked);
    void add_file_trigger_button_clicked(bool clicked);
    void add_file_button_clicked(bool clicked);

private:
    Ui::MainWindow *ui;
    void updateInfo(IniFile filename);
    //IniFile someFile;
    std::map<QString,IniFile> workingFiles;
    void changeTitle(int changed);
    bool saved;
};
#endif // MAINWINDOW_H
