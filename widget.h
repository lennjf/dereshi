#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QIcon>
#include <QPixmap>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QtMultimedia>
#include <QtCore>
#include <QDebug>
#include <QtWidgets>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.player.dereshi")
public:
    Widget(Widget *parent = nullptr);
    ~Widget();
    void createTrayIcon();
    void listFile(QString mp3dir);
    void playtheaudio();
private slots:
    void onTrayIconTriggered(QSystemTrayIcon::ActivationReason reason);

    void on_homebtn_clicked();

    void on_volume_valueChanged(int value);


    void grogress_changed();

    // void on_progress_sliderMoved(int position);

    void on_playbtn_clicked();

    void on_pausebtn_clicked();

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void on_prebtn_clicked();

    void on_postbtn_clicked();

    void onAudioDevicesChanged();

    void on_closebtn_clicked();
public slots:
    void showAndActivate();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::Widget *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QMediaPlayer *mp;
    QAudioOutput *ao;
    QTimer *timer;
    QSettings *settings;
    QStringList *musicList;
    int mp3index;
    QString *mp3dir;
    int volumenum;

    QMediaDevices *device;
};
#endif // WIDGET_H
