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

#include "channel.h"

Channel::Channel(QColor color)
{
    this->color = color;
}

void Channel::setName(const QString name) {
    this->name = name;
}

QString Channel::getName() const {
    return this->name;
}

void Channel::setColor(QColor color) {
    this->color = color;
}

QColor Channel::getColor() const {
    return color;
}

void Channel::append(float value) {
    values.append(value);
}

float Channel::getValue(int num) const {
    return values.at(num);
}

int Channel::getCount() {
    return this->values.count();
}
