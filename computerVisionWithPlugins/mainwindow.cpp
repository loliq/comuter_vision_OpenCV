#include "mainwindow.h"
#include "ui_mainwindow.h"
#define PLUGINS_SUBFOLDER                   "/cvplugins/"
#define FILE_ON_DISK_DYNAMIC_PROPERTY       "absolute_file_path"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this); //ui_window.h是根据编译生成的
    populatePluginsMenu();
    initUI();
    initSlots();
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

void MainWindow:: initSlots()
{
    QObject::connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::on_directoryChanged);
    QObject::connect(ui->graphicsView, SIGNAL(sigMouseMovePoint(QPoint)), this, SLOT(on_mouseMovePoint(QPoint)));
//    QObject::connect(ui->graphicsView, SIGNAL(sigMouseClicked(QPoint)), this, SLOT(on_mouseCliked(QPoint)));

}

void MainWindow::on_mouseMovePoint(QPoint point)
{

    labViewCord->setText(QString::asprintf("View 坐标： %d, %d", point.x(),point.y()));
    QPointF pointScene = ui->graphicsView->mapToScene(point); //转到scene坐标
    labSceneCord->setText(QString::asprintf("Scene坐标: %.0f, %.0f", pointScene.x(), pointScene.y()));
    QGraphicsItem *item = NULL;
    item = m_scene.itemAt(pointScene, ui->graphicsView->transform());
//    ui->graphicsView->translate(-10,-10);
    if(item !=NULL)
    {
        QPointF pointItem = item->mapFromScene(pointScene);
        cv::Mat image;
        if(item == &m_processedPixmap)
        {
            image = m_processedMat;
        }
        else if(item == &m_originalPixmap)
        {
            image = m_originalMat;
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

    /*Fixed */
    m_originalPixmap.setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    m_processedPixmap.setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    ui->graphicsView->setMouseTracking(true); //设置能否鼠标跟踪
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsView->setScene(&m_scene); //设置场景,场景在视图中默认居中
    m_scene.addItem(&m_originalPixmap);    //放入Item必要的时候显示不同的Item
    m_scene.addItem(&m_processedPixmap);
    m_originalPixmap.setPos(0,0);
    m_processedPixmap.setPos(0,0);
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
    if(!m_originalMat.empty())
    {

        dftImg(m_originalMat, m_processedMat);
        m_originalQImage  = Mat2QImage(m_originalMat);
        m_originalPixmap.setPixmap(QPixmap::fromImage(m_originalQImage));

        m_processedQImage  = Mat2QImage(m_processedMat);
        m_processedPixmap.setPixmap(QPixmap::fromImage(m_processedQImage));
    }
}

void MainWindow::on_btn_openImageDir_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Input Image"),
                                                    QDir::currentPath(),
                                                    tr("Images") + " (*.jpg *.png *.bmp)");

    using namespace cv;
    m_originalMat = imread(fileName.toStdString(),-1);
    if(!m_originalMat.empty())
    {
        QFileInfo fileInfo(fileName);
        QString fileDir = fileInfo.path();
        if(m_fileWatcher.directories().length() != 0) //如果当前监控列表已经有目录，则删除，一次只监控一个文件夹
        {
            m_fileWatcher.removePaths(m_fileWatcher.directories()) ; //清空监控列表
        }
        m_fileWatcher.addPath(fileDir);   //监听图像目录
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
    QDir dir(m_fileWatcher.directories().at(0));
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
    m_originalPixmap.setVisible(checked);   
    m_processedPixmap.setVisible(!checked);
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
    QString fileName = m_fileWatcher.directories().at(0) + '/' +  ui->cmb_imgeList->currentText();
    m_originalMat = cv::imread(fileName.toStdString(), -1);
    onCurrentPluginUpdateNeeded();
}

void MainWindow::on_cmb_imgeList_currentIndexChanged(const QString &arg1)   //这句话在构造函数的时候也会变
{
    QString fileName = m_fileWatcher.directories().at(0) + '/' +  arg1;
    m_originalMat = cv::imread(fileName.toStdString(), -1);
    onCurrentPluginUpdateNeeded();

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
    QString fileName = m_fileWatcher.directories().at(0) + '/' +  ui->cmb_imgeList->currentText();
    m_originalMat = cv::imread(fileName.toStdString(), -1);
    onCurrentPluginUpdateNeeded();
}

void MainWindow::on_ckb_showOriImg_toggled(bool checked)
{
    m_originalPixmap.setVisible(checked);
    m_processedPixmap.setVisible(!checked);
}

void MainWindow::on_actionsaveImage_triggered()
{
    if(!ui->ckb_showOriImg->isChecked() && !m_processedMat.empty())
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), "*.jpg;;*.png;;*.bmp");
        if(!fileName.isEmpty())
        {
            cv::imwrite(fileName.toStdString(), m_processedMat);
        }
    }
    else if(ui->ckb_showOriImg->isChecked() && !m_originalMat.empty())
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), "*.jpg;;*.png;;*.bmp");
        if(!fileName.isEmpty())
        {
            cv::imwrite(fileName.toStdString(), m_originalMat);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"), tr("There is nothing to be saved!"));
    }
}

void MainWindow::populatePluginsMenu()
{
    // Load all plugins and populate the menus
    QDir pluginsDir(qApp->applicationDirPath() + PLUGINS_SUBFOLDER);
    QFileInfoList pluginFiles = pluginsDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    foreach(QFileInfo pluginFile, pluginFiles)
    {
        if(QLibrary::isLibrary(pluginFile.absoluteFilePath()))
        {
            QPluginLoader pluginLoader(pluginFile.absoluteFilePath(), this);
            if(CvPluginInterface *plugin = dynamic_cast<CvPluginInterface*>(pluginLoader.instance()))
            {
                QAction *pluginAction = ui->menu_Plugins->addAction(plugin->title());
                pluginAction->setProperty(FILE_ON_DISK_DYNAMIC_PROPERTY, pluginFile.absoluteFilePath());
                connect(pluginAction, SIGNAL(triggered(bool)), this, SLOT(onPluginActionTriggered(bool)));
                if(m_currentPluginFile == pluginFile.absoluteFilePath())
                {
                    pluginAction->trigger();
                }
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"),
                                     QString(tr("Make sure %1 is a correct plugin for this application<br>"
                                                "and it's not in use by some other application!")).arg(pluginFile.fileName()));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"),
                                 QString(tr("Make sure only plugins exist in %1 folder.<br>"
                                            "%2 is not a plugin."))
                                 .arg(PLUGINS_SUBFOLDER)
                                 .arg(pluginFile.fileName()));
        }
    }

    if(ui->menu_Plugins->actions().count() <= 0)
    {
        QMessageBox::critical(this, tr("No Plugins"), QString(tr("This application cannot work without plugins!"
                                                                 "<br>Make sure that %1 folder exists "
                                                                 "in the same folder as the application<br>and that "
                                                                 "there are some filter plugins inside it")).arg(PLUGINS_SUBFOLDER));
        this->setEnabled(false);
    }
}

void MainWindow::onPluginActionTriggered(bool)
{
    if(!m_currentPlugin.isNull())
    {
        delete m_currentPlugin;
        delete m_currentPluginGui;
    }

    m_currentPluginFile = QObject::sender()->property(FILE_ON_DISK_DYNAMIC_PROPERTY).toString();
    m_currentPlugin = new QPluginLoader(m_currentPluginFile, this);
    m_currentPluginGui = new QWidget(this);
    ui->pluginLayout->addWidget(m_currentPluginGui);
    CvPluginInterface *currentPluginInstance = dynamic_cast<CvPluginInterface*>(m_currentPlugin->instance());

    if(currentPluginInstance)
    {
        currentPluginInstance->setupUi(m_currentPluginGui);
        connect(m_currentPlugin->instance(), SIGNAL(updateNeeded()), this, SLOT(onCurrentPluginUpdateNeeded()));
        connect(m_currentPlugin->instance(), SIGNAL(infoMessage(QString)), this, SLOT(onCurrentPluginInfoMessage(QString)));
        connect(m_currentPlugin->instance(), SIGNAL(errorMessage(QString)), this, SLOT(onCurrentPluginErrorMessage(QString)));

    }
}


void MainWindow::onCurrentPluginUpdateNeeded()
{
    if(!m_originalMat.empty())
    {
        if(!m_currentPlugin.isNull())
        {
            CvPluginInterface *currentPluginInstance = dynamic_cast<CvPluginInterface*>(m_currentPlugin->instance());
            if(currentPluginInstance)
            {
                cv::TickMeter meter;
                meter.start();
                currentPluginInstance->processImage(m_originalMat, m_processedMat);
                meter.stop();
                qDebug() << "The process took " << meter.getTimeMilli() << "milliseconds";
            }
        }
        else
        {
            m_processedMat = m_originalMat.clone();
        }

        m_originalQImage  = Mat2QImage(m_originalMat);
        ui->graphicsView->centerOn(QPointF(400,200));
        m_originalPixmap.setPixmap(QPixmap::fromImage(m_originalQImage));


        m_processedQImage  = Mat2QImage(m_processedMat);
        m_processedPixmap.setPixmap(QPixmap::fromImage(m_processedQImage));
    }
}

void MainWindow::onCurrentPluginErrorMessage(QString msg)
{
    QMessageBox::critical(this, tr("No Plugins"), QString(tr(" %1 "
                                                            )).arg(PLUGINS_SUBFOLDER));
}

void MainWindow::onCurrentPluginInfoMessage(QString msg)
{
    qDebug() << "Plugin Info Message : " << msg;
}




void MainWindow::on_pushButton_clicked()
{
    ui->graphicsView->resetTransform();
}
