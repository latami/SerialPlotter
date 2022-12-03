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

#include <QPainter>
#include <QtMath>

#include "plotter.h"
#include <iostream>
Plotter::Plotter(QWidget *parent) : QLabel(parent)
{
    this->pix = QPixmap(this->size());

    scale.setParent(this);
    scale.setPixmap(QPixmap(40, this->height()));

    QColor default_colors[8];
    default_colors[0] = QColor(0, 255, 0, 255);
    default_colors[1] = QColor(255, 0, 0, 255);
    default_colors[2] = QColor(0, 0, 255, 255);
    default_colors[3] = QColor(255, 255, 0, 255);
    default_colors[4] = QColor(0, 255, 255, 255);
    default_colors[5] = QColor(255, 255, 255, 255);
    default_colors[6] = QColor(128, 128, 128, 255);
    default_colors[7] = QColor(0, 128, 128, 255);

    for(int i=0; i < 8; i++) {
        Channel *channel = new Channel(default_colors[i]);
        this->channels.append(channel);

        ChannelBox *box = new ChannelBox(this);
        box->move(40+i*80, 0);
        this->chBoxes.append(box);
        // Changing checkbox state forces redraw of the plot
        connect(box->getCheckBox(), &QCheckBox::stateChanged, this, &Plotter::redraw);
    }
}

Plotter::~Plotter() {
    while(this->channels.length() > 0) {
        delete this->channels.first();
        this->channels.removeFirst();
    }
}

void Plotter::updatePlotScaling() {
    float largestValue = 1.0f;
    for(int i = 0; i <channels.count(); i++) {
        // Scaling is determined by visible channels
        if (this->chBoxes.at(i)->channelIsChecked()) {
            Channel *ch = channels.at(i);
            int min = ch->getCount()-this->width();
            if (min < 0)
                min = 0;
            for(int j=ch->getCount()-1; j > min; j--) {
                if (largestValue < qFabs(ch->getValue(j)))
                    largestValue = qFabs(ch->getValue(j));
            }
        }
    }
    float scaling = this->height() / (largestValue *2.0f);
    /* FIXME: using mul 128 to avoid zero when scalingFactor goes under 2.
     * Mul by 16 is to support few subzero scaling factors that are multiples of 2. */
    float factor;
    for(int power=-4; power < 5; power++) {
        factor = qPow(2.0f, -power);
        if (factor < scaling)
            break;
    }
    this->scalingFactor = factor;
}

void Plotter::updatePlot(QString line) {

    QStringList strList = line.split(" ");

    bool previousOK = false, updatePlot = false;
    int runningValue = 0;       // Running value for 'nameless' values in a line.
    for(int i=0; i < strList.size(); i++) {
        bool isOk;
        QString str = strList.at(i);

        // Check for and remove comma
        if (str.endsWith(',')) {
            str.remove(str.length()-1, 1);
        }
        // Check if string is a number
        float value = str.toFloat(&isOk);
        if (isOk) {
            QString name;
            if (previousOK) {
                /* If previous string was also a number, assign integer
                 * (nameless count) as a name. */
                name = QString::number(runningValue);
                runningValue++;
            }
            else {
                // Previous string is a name
                name = strList.at(i-1);
            }

            int j;
            bool nameFound = false;
            // Check if there is a channel with a same name.
            for(j=0; j < channels.length(); j++) {
                Channel *ch = channels.at(j);
                if (ch->getName().compare(name) == 0) {
                    nameFound = true;
                    break;
                }
            }
            if (nameFound) {
                channels.at(j)->append(value);
                if (chBoxes.at(j)->channelIsChecked())
                    updatePlot = true;
            }
            else {
                // If no channel with that name, name one.
                int k;
                for(k=0; k < channels.length(); k++) {
                    Channel *ch = channels.at(k);
                    if (ch->getName().compare("") == 0) {
                        ch->setName(strList.at(i-1));
                        ch->append(value);

                        std::cout << ch->getName().toStdString() << std::endl;
                        chBoxes.at(k)->setLabel(ch->getName(), ch->getColor());

                        if (chBoxes.at(k)->channelIsChecked())
                            updatePlot = true;
                        break;
                    }
                }
            }
            // Remember this was a number for a next string
            previousOK = true;
        }
        else
            previousOK = false;
    }

    if (updatePlot) {
        static float previousScalingFactor = scalingFactor;

        updatePlotScaling();

        if (previousScalingFactor == scalingFactor) {

            QPainter painter(&pix);

            // Scroll and paint new area black
            QRegion region;
            this->pix.scroll(-1, 0, 1, 0, pix.width()-1, pix.height(), &region);
            painter.fillRect(region.boundingRect(), QColor(0, 0, 0, 255));

            // Draw new point
            for(int i=0; i < this->channels.size(); i++) {
                Channel *ch = channels.at(i);

                if (chBoxes.at(i)->channelIsChecked()) {
                    if (ch->getCount() > 1) {
                        path.clear();
                        float x = this->width()-1.0f;

                        int lastKey = ch->getCount()-1;

                        path.moveTo(x, pix.height()/2.0 - ch->getValue(lastKey-1)*scalingFactor);
                        path.lineTo(x+1.0f, pix.height()/2.0f - ch->getValue(lastKey)*scalingFactor);

                        painter.setPen(QPen(QBrush(ch->getColor()), 1.0f));
                        painter.drawPath(path);
                    }
                }
            }
            this->setPixmap(pix);
        }
        else {
            redraw();
        }
        previousScalingFactor = scalingFactor;
    }
}

void Plotter::resizeEvent(QResizeEvent *event) {
    redraw();
}

void Plotter::redraw() {
    this->pix = QPixmap(this->size());

    QPainter painter(&pix);

    pix.fill(QColor(0, 0, 0, 255));

    updatePlotScaling();

    // Draw channel paths
    for(int i=0; i < this->channels.size(); i++) {
        if (chBoxes.at(i)->channelIsChecked()) {
            Channel *ch = channels.at(i);

            if (ch->getCount() > 0) {
                path.clear();
                float x=0.0f;

                painter.setPen(QPen(QBrush(ch->getColor()), 1.0f));

                int j;
                int ch_end = ch->getCount()-1;
                // Draw from right to left
                path.moveTo(this->width()-1, this->height()/2.0f - ch->getValue(ch_end)*scalingFactor);
                for(j=0, x=(float)(this->width()-2); j < ch->getCount()-1; j++, x-=1.0f) {
                    path.lineTo(x, this->height()/2.0f - ch->getValue(ch_end-j)*scalingFactor);
                }
                painter.drawPath(path);
            }
        }
    }
    this->setPixmap(pix);

    // Draw scale
    scale.resize(40, this->height());
    QPixmap scalePix = QPixmap(scale.size());

    scalePix.fill(QColor(0, 0, 0, 0));

    QPainter scalePainter(&scalePix);
    scalePainter.setPen(QPen(QBrush(QColor(255,255,255,255)), 2.0f));

    int notch = scale.height()/10;
    // Round upward to tens
    notch = (notch+9)/10 * 10;

    int mid = (int)(scale.height()/2.0f / notch)*notch;
    for(int i = scale.height()/2.0 - mid; i < scale.height(); i += notch) {
        scalePainter.drawLine(0, i, scale.width()-1, i);
        QString str;
        str.setNum((scale.height()/2.0f - i)/scalingFactor);
        scalePainter.drawText(0, i-2, str);
    }

    scale.setPixmap(scalePix);
}
