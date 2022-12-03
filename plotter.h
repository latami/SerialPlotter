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

#ifndef PLOTTER_H
#define PLOTTER_H

#include <QLabel>
#include <QPainterPath>

#include "channel.h"
#include "channelbox.h"

class Plotter : public QLabel
{
public:
    Plotter(QWidget *parent = nullptr);
    ~Plotter();

    void updatePlot(QString line);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void redraw();
    void updatePlotScaling();

    QLabel scale;

    QList<Channel*> channels;
    QList<ChannelBox*> chBoxes;
    QPixmap pix;
    QPainterPath path;

    float scalingFactor = 1.0f;
};

#endif // PLOTTER_H
