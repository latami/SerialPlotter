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

#include <iostream>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPainter>
#include <QPainterPath>

#include <QRegularExpression>
#include <QThread>

extern "C" {
#include <dirent.h>
}

//FIXME: Is there better way to pass object ptr to static member function?
MainWindow *ctx;

// Static function to pass for serial reader.
void MainWindow::printLine(char *a) {
    // Put char array to QString for threadsafe queueing.
    QString b = QString(a);

    // Emit signal requestline()
    emit ctx->requestLine(b);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->serial_fd = -1;

    ctx = this;

    populateDevices();

    // Thread for monitoring /dev
    this->monitorThread = new QThread;
    this->monitorWorker = new Worker_monitorDev;
    this->monitorWorker->moveToThread(this->monitorThread);
    connect(this->monitorThread, &QThread::started, this->monitorWorker, &Worker_monitorDev::monitor);
    connect(this->monitorWorker, &Worker_monitorDev::filesCreatedDeleted, this, &MainWindow::populateDevices);
    connect(this->monitorWorker, &Worker_monitorDev::finished, this->monitorThread, &QThread::quit);
    connect(this->monitorWorker, &Worker_monitorDev::finished, this->monitorThread, &Worker_monitorDev::deleteLater);
    connect(this->monitorWorker, &Worker_monitorDev::finished, this->monitorThread, &QThread::deleteLater);
    monitorThread->start();
}

MainWindow::~MainWindow()
{
    if (this->serial_fd != -1) {
        serial_close(this->serial_fd);
        pthread_join(this->SerialRead, NULL);
        this->serial_fd = -1;
    }
    delete ui;
}

void MainWindow::populateDevices() {
    DIR *dir;
    struct dirent *dirent;

    std::cout << "Going through devices..." << std::endl;

    ui->comboBox_device->clear();
    if ((dir = opendir("/dev/")) != NULL) {
        while((dirent = readdir(dir)) != NULL) {
            QString fname(dirent->d_name);
            if (fname.contains("ttyACM") || fname.contains("ttyUSB")) {
                ui->comboBox_device->addItem(fname.insert(0, "/dev/"));
            }
        }
        closedir(dir);
    }
    else
        std::cerr << "Could not open directory /dev!" << std::endl;
}

void MainWindow::updatePlot(QString line) {

    QString termLine = line;

    termLine.remove(QRegularExpression("\\\n"));
    ui->termLines->appendPlainText(termLine);

    ui->plotter->updatePlot(line);
}

void MainWindow::on_pushButton_openDevice_clicked()
{
    // Check if file descriptor is valid (In sense that device should be open)
    if (this->serial_fd == -1) {

        unsigned int bauds = baud_rates[12+ui->comboBox_baudrate->currentIndex()];
        this->serial_fd = serial_open(ui->comboBox_device->currentText().toStdString().data(), bauds);
        if (this->serial_fd != -1) {
            serial_set_struct(&this->reader, this->serial_fd, printLine);

            pthread_create(&this->SerialRead, NULL, serial_read, &reader);

            // Serial reader callback function hands over data to GUI thread
            connect(this, &MainWindow::requestLine, this, &MainWindow::updatePlot, Qt::QueuedConnection);

            ui->pushButton_openDevice->setText("Close device");
        }
    }
    else {
        if (this->serial_fd != -1) {
            serial_close(this->serial_fd);
            pthread_join(this->SerialRead, NULL);
            this->serial_fd = -1;

            disconnect(this, &MainWindow::requestLine, this, &MainWindow::updatePlot);

            ui->pushButton_openDevice->setText("Open device");
        }
    }
}
