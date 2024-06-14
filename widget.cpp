#include "widget.h"
#include "./ui_widget.h"

bool isDir(const QString &path) {
    QDir dir(path);
    return dir.exists();
}

bool isAudioFile(const QString &path) {
    QString str(path);
    QFileInfo fileInfo(str);
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(fileInfo.filePath());
    if(mime.name().toLower().startsWith("audio")){
        return true;
    }else{
        return false;
    }
}

void Widget::listFile(QString mp3dir){
    QDir dir(mp3dir);
    QStringList list = dir.entryList();
    foreach (QString str, list.toList()) {
        if(str != "." && str != ".."){
            str = mp3dir + QDir::separator() + str;
            if(isDir(str)){
                if (!str.endsWith("/store")){
                    listFile(str);
                }
            }else {
                if(isAudioFile(str)){
                    musicList->append(str);
                }
            }
        }
    }
}




Widget::Widget(Widget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , trayIcon(new QSystemTrayIcon(this))
{
    ui->setupUi(this);

    trayIconMenu = new QMenu(this);
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    createTrayIcon();
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);


    mp = new QMediaPlayer(this);
    ao = new QAudioOutput(this);
    ao->setVolume(0.3);
    mp->setAudioOutput(ao);
    ui->volume->setMinimum(0);
    ui->volume->setMaximum(100);
    ui->volume->setValue(30);

    musicList = new QStringList();
    mp3index = 0;


    settings = new QSettings("carpenter-production", "dereshi-player");
    mp3dir = new QString(settings->value("mp3dir").toString());
    if(mp3dir->isEmpty()){
        ui->ed->setText("pls choose a mp3 resource folder to kick off.");
    }else{
        listFile(*mp3dir);
        ui->ed->setText(QString::number(musicList->size()) + " audio files loaded");
    }
    if(settings->value("mp3index").toString().isEmpty()){
        mp3index = 0;
    }else {
        mp3index = settings->value("mp3index").toString().toInt();
    }
    if(settings->value("volumenum").toString().isEmpty()){
        volumenum = 30;
    }else {
        volumenum = settings->value("volumenum").toString().toInt();
    }


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::grogress_changed);
    timer->start(500);

    connect(mp, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChanged);

    device = new QMediaDevices();

    connect(device, &QMediaDevices::audioOutputsChanged,
            this, &Widget::onAudioDevicesChanged);

    QFile file(":/sty.css");
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        this->setStyleSheet(styleSheet);
    }

    int id = QFontDatabase::addApplicationFont(":/Mediaeval.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    ui->ed->setFont(QFont(family, 14));


}

Widget::~Widget()
{
    settings->setValue("mp3index", mp3index);
    settings->setValue("volumenum", volumenum);
    delete ui;
    delete trayIcon;
    delete musicList;
    delete mp3dir;
    delete settings;
    delete device;
    QString appid = "dereshi-player";
    QSharedMemory sharedMemory(appid, qApp);
    if(sharedMemory.isAttached()){
        sharedMemory.detach();
    }
}

void Widget::createTrayIcon()
{
    QSvgRenderer svgRenderer;
    svgRenderer.load(QString(":/dereshi.svg"));
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    svgRenderer.render(&painter);

    QIcon icon(pixmap);
    trayIcon->setIcon(icon);

    trayIcon->setToolTip("System Tray Example");

    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &Widget::onTrayIconTriggered);
}

void Widget::onTrayIconTriggered(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        this->isHidden() ? this->show() : this->hide();

    }
}

void Widget::playtheaudio(){
    QString filename =  musicList->at(mp3index);
    mp->setSource(QUrl::fromLocalFile(filename));
    ao->setVolume((float)volumenum/100);
    ui->volume->setValue(volumenum);
    mp->play();
    QFileInfo fileinfo(filename);
    ui->ed->setText(fileinfo.fileName() + "  ...  " + QString::number(mp3index + 1) + " of " + QString::number(musicList->size()) + "  ...");
}

void Widget::on_homebtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectoryUrl(this, "select a dir").toString().replace("file://", "");
    if(dir.isEmpty()){
        return;
    }else{
        settings->setValue("mp3dir", dir);
        settings->setValue("mp3index", mp3index);
        settings->setValue("volumenum", volumenum);
        musicList->clear();
        listFile(dir);
        mp3dir = &dir;
        playtheaudio();
    }
}

void Widget::on_volume_valueChanged(int value)
{
    //ui->ed->setText( QString::number(value));
    ao->setVolume((float)value/100);
    //qInfo() << "volume changed: " << value;
    volumenum = value;
}

void Widget::grogress_changed()
{
    if(mp->hasAudio()){
        ui->progress->setValue(mp->position()*100/mp->duration());
    }
}





void Widget::on_closebtn_clicked()
{
    this->hide();
}

bool isProgressBar(QWidget *widget) {
    QProgressBar *progressBar = qobject_cast<QProgressBar*>(widget);
    return progressBar != nullptr;
}

void Widget::mousePressEvent(QMouseEvent *event)
{

    if(mp->hasAudio()){
        QPoint clickPosition = event->pos();
        QWidget *clickedWidget = childAt(clickPosition);

        if (clickedWidget && isProgressBar(clickedWidget)) {


            mp->pause();
            mp->setPosition((qint64)mp->duration()*((double)((event->pos().rx()-clickedWidget->pos().rx())/(double)clickedWidget->width())));
            mp->play();
            // qInfo() << mp->duration();
            // qInfo() << clickedWidget->pos().rx();
            // qInfo() << event->pos().rx();
            // qInfo() << clickedWidget->width();
            // qInfo() << ((event->pos().rx()-clickedWidget->pos().rx())/clickedWidget->width());
            // qInfo() << (double)((event->pos().rx()-clickedWidget->pos().rx())/(double)clickedWidget->width());
        }
    }
}



void Widget::on_playbtn_clicked()
{
    if(mp->hasAudio()){
        mp->play();
    }else if(musicList->size()>0){
        playtheaudio();
    }

}

void Widget::on_pausebtn_clicked()
{
    if(mp->hasAudio()){
        mp->pause();
    }

}

void Widget::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {

        //qDebug() << "MP3 has finished playing " << musicList->size();
        if(mp3index < (musicList->size() - 1)){
            mp3index += 1;
        }else {
            mp3index = 0;
        }
        playtheaudio();
    }
}






void Widget::on_prebtn_clicked()
{
    if(mp3index > 0 ){
        mp3index -= 1;
    }else {
        mp3index = musicList->size() - 1;
    }
    playtheaudio();
}


void Widget::on_postbtn_clicked()
{
    if(mp3index < (musicList->size() - 1)){
        mp3index += 1;
    }else {
        mp3index = 0;
    }
    playtheaudio();
}

void Widget::onAudioDevicesChanged()
{
    //qInfo() << "default audio changed............";
    on_pausebtn_clicked();
    delete ao;
    ao = new QAudioOutput(QMediaDevices::defaultAudioOutput(), this);
    ao->setVolume((float)volumenum/100);
    mp->setAudioOutput(ao);
    //qInfo() << "changed to device:" << QMediaDevices::defaultAudioOutput().id();
}


