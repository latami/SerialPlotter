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

#ifndef CHANNEL_H
#define CHANNEL_H

#include <QColor>
#include <QMap>

class Channel
{
public:
    Channel(QColor color);

    void setName(const QString name);
    QString getName() const;
    void append(float value);
    float getValue(int num) const;

    void setColor(QColor color);
    QColor getColor() const;

    int getCount();
private:
    QString name;
    QVector<float> values;
    QColor color;

};

#endif // CHANNEL_H
