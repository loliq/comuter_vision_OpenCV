#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>   //保存窗口设置
#include <QCloseEvent>
#include <QMessageBox>
#include <QDir>
#include <QFileInfoList>
#include <QLibrary>
#include <QPluginLoader>
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QGraphicsScene>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QFileSystemWatcher>
#include <QPoint>
#include <opencv2/opencv.hpp>
#include "ImageProcess/imageprocess.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_openImageDir_clicked();
    void onImageProcessed();
    void on_checkBox_toggled(bool checked);
    void on_btn_preImage_clicked();
    void on_cmb_imgeList_currentIndexChanged(const QString &arg1);
    void on_btn_nextImage_clicked();
    void on_directoryChanged(); //监听目录的文件发生了变化
    void on_mouseMovePoint(QPoint point);
//    void on_mouseCliked(QPoint point);
protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QLabel* labViewCord;
    QLabel* labSceneCord;
    QLabel* labItemCord;
    void initUI();
    void initGraphicsSystem();
    void init_slots();
private:
    QFileSystemWatcher  fileWatcher;
    QGraphicsScene scene;    //场景类，用来管理里面的对象
    cv::Mat originalMat, processedMat;
    QImage originalImage, processedImage;
    QStringList fileNames;
    QGraphicsPixmapItem originalPixmap, processedPixmap;  //图像Item项
    void saveSettings();  //保存控件设置
    void loadSettings();  //导入控件设置
    void on_change_ImageDir();

};

#endif // MAINWINDOW_H
