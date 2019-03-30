#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageprocess.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this); //ui_window.h是根据编译生成的
    initUI();
    init_slots();
    initGraphicsSystem();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{

    //设置一些状态栏
    labViewCord = new QLabel("View坐标: ");
    labViewCord->setMinimumWidth(150);
    ui->statusBar->addWidget(labViewCord);
    labSceneCord = new QLabel("Scene坐标: ");
    labSceneCord->setMinimumWidth(150);
    ui->statusBar->addWidget(labSceneCord);
    labItemCord = new QLabel("Item坐标: ");
    labItemCord->setMinimumWidth(150);
    ui->statusBar->addWidget(labItemCord);
    //
}

void MainWindow:: init_slots()
{
    QObject::connect(&fileWatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::on_directoryChanged);
    QObject::connect(ui->graphicsView, SIGNAL(sigMouseMovePoint(QPoint)), this, SLOT(on_mouseMovePoint(QPoint)));
//    QObject::connect(ui->graphicsView, SIGNAL(sigMouseClicked(QPoint)), this, SLOT(on_mouseCliked(QPoint)));

}

void MainWindow::on_mouseMovePoint(QPoint point)
{

    labViewCord->setText(QString::asprintf("View 坐标： %d, %d", point.x(),point.y()));
    QPointF pointScene = ui->graphicsView->mapToScene(point); //转到scene坐标
    labSceneCord->setText(QString::asprintf("Scene坐标: %.0f, %.0f", pointScene.x(), pointScene.y()));
    QGraphicsItem *item = NULL;
    item = scene.itemAt(pointScene, ui->graphicsView->transform());
//    ui->graphicsView->translate(-10,-10);
    if(item !=NULL)
    {
        QPointF pointItem = item->mapFromScene(pointScene);
        cv::Mat image;
        if(item == &processedPixmap)
        {
            image = processedMat;
        }
        else if(item == &originalPixmap)
        {
            image = originalMat;
        }
        if(!image.empty())
        {
            int channelNum = image.channels();
            if(channelNum == 1)
            {
            labItemCord->setText(QString::asprintf("Item坐标: (%.0f, %.0f) 灰度值:(%d)", pointItem.y(), pointItem.x(),
                                                   image.at<uchar>(pointItem.y(), pointItem.x())));
            }
            else if(channelNum == 3)
            {
            labItemCord->setText(QString::asprintf("Item坐标: (%.0f, %.0f) RGB:(%d, %d, %d)", pointItem.y(), pointItem.x(),
                                                       image.at<cv::Vec3b>(pointItem.y(), pointItem.x())[2],
                                                       image.at<cv::Vec3b>(pointItem.y(), pointItem.x())[1],
                                                       image.at<cv::Vec3b>(pointItem.y(), pointItem.x())[0]));
            }
        }

    }

}
void MainWindow::on_directoryChanged()
{
    on_change_ImageDir(); // 更新图像列表
}

void MainWindow::initGraphicsSystem()
{

    originalPixmap.setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    processedPixmap.setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    ui->graphicsView->setMouseTracking(true); //设置能否鼠标跟踪
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsView->setScene(&scene); //设置场景,场景在视图中默认居中
    scene.addItem(&originalPixmap);    //放入Item必要的时候显示不同的Item
    scene.addItem(&processedPixmap);
    originalPixmap.setPos(0,0);
    processedPixmap.setPos(0,0);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::warning(this, tr("Exit"), tr("Are you sure you want to exit?"), QMessageBox::Yes, QMessageBox::No);
    if(result == QMessageBox::Yes)
    {
//        saveSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
void MainWindow::onImageProcessed()
{
    if(!originalMat.empty())
    {

//        cv::medianBlur(originalMat, processedMat, 3);
        dftImg(originalMat, processedMat);
        originalImage  = Mat2QImage(originalMat);
        originalPixmap.setPixmap(QPixmap::fromImage(originalImage));

        processedImage  = Mat2QImage(processedMat);
        processedPixmap.setPixmap(QPixmap::fromImage(processedImage));
    }
}

void MainWindow::on_btn_openImageDir_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Input Image"),
                                                    QDir::currentPath(),
                                                    tr("Images") + " (*.jpg *.png *.bmp)");

    using namespace cv;
    originalMat = imread(fileName.toStdString(),-1);
    if(!originalMat.empty())
    {
        QFileInfo fileInfo(fileName);
        QString fileDir = fileInfo.path();
        if(fileWatcher.directories().length() != 0) //如果当前监控列表已经有目录，则删除，一次只监控一个文件夹
        {
            fileWatcher.removePaths(fileWatcher.directories()) ; //清空监控列表
        }
        fileWatcher.addPath(fileDir);   //监听图像目录

        on_change_ImageDir();
        ui->cmb_imgeList->setCurrentText(fileInfo.fileName());  //设置文件为当前处理的文件，会多读一次文件，应该没关系

    }
    else if(!fileName.trimmed().isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Make sure the image file exists "
                                 "and it is accessible!"));
    }
}

void MainWindow:: on_change_ImageDir()
{
    QDir dir(fileWatcher.directories().at(0));
    QStringList strList = dir.entryList(QDir::Files);
    ui->cmb_imgeList->clear();
    foreach (QString str , strList) {
       QFileInfo fileInfo(str);
       QString fileBaseName = fileInfo.suffix().toLower();
       if(fileBaseName == "jpg" || fileBaseName == "png"
               || fileBaseName == "jpeg"  || fileBaseName == "bmp"  )
       {
           ui->cmb_imgeList->addItem(fileInfo.fileName());   //添加不带路径的文件名
       }
    }

}

void MainWindow::on_checkBox_toggled(bool checked)
{
    originalPixmap.setVisible(checked);
    processedPixmap.setVisible(!checked);
}

void MainWindow::on_btn_preImage_clicked()
{
    if(!ui->cmb_imgeList->count()) //如果没有设置图像目录
    {
        return;
    }
    int image_index = ui->cmb_imgeList->currentIndex();
    if( !image_index ) //如果是第一张，转到最后一张
    {
        ui->cmb_imgeList->setCurrentIndex(ui->cmb_imgeList->count()-1); //转到最后一张
    }
    else
    {
         ui->cmb_imgeList->setCurrentIndex(image_index-1);
    }
    QString fileName = fileWatcher.directories().at(0) + '/' +  ui->cmb_imgeList->currentText();
    originalMat = cv::imread(fileName.toStdString(), -1);
    onImageProcessed();
}

void MainWindow::on_cmb_imgeList_currentIndexChanged(const QString &arg1)   //这句话在构造函数的时候也会变
{
    QString fileName = fileWatcher.directories().at(0) + '/' +  arg1;
    originalMat = cv::imread(fileName.toStdString(), -1);
    onImageProcessed();

}

void MainWindow::on_btn_nextImage_clicked()
{
    if(!ui->cmb_imgeList->count()) //如果没有设置图像目录
    {
        return;
    }
    int image_index = ui->cmb_imgeList->currentIndex();
    if( image_index == ui->cmb_imgeList->count()-1) //如果是最后一张,转到第一张
    {
        ui->cmb_imgeList->setCurrentIndex(0); //转到第一张
    }
    else
    {
         ui->cmb_imgeList->setCurrentIndex(image_index+1);
    }
    QString fileName = fileWatcher.directories().at(0) + '/' +  ui->cmb_imgeList->currentText();
    originalMat = cv::imread(fileName.toStdString(), -1);
    onImageProcessed();
}
