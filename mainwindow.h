/*  SerialPlotter reads serial port and creates plots from received text.
 *  Copyright (C) 2022  Taneli Mikkonen
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/> */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainterPath>

#include "channel.h"
#include "plotter.h"
#include "worker_monitordev.h"

extern "C" {
#include "read_serial.h"
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QThread *monitorThread;
    Worker_monitorDev *monitorWorker;

    int serial_fd;
    pthread_t SerialRead;
    struct serialReader reader;

    QList<Channel*> channels;
    Plotter plotter;

    QImage img;
    QPixmap pix;
    QPainterPath path;

signals:
    void requestLine(QString a);
private slots:
    static void printLine(char *a);
    void updatePlot(QString line);
    void populateDevices();
    void on_pushButton_openDevice_clicked();
};
#endif // MAINWINDOW_H
