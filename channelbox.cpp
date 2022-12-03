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

#include "channelbox.h"

ChannelBox::ChannelBox(QWidget *parent) : QGroupBox(parent)
{
    box = new QCheckBox(this);

    // Workaround to set label size big enough for later
    box->setText("     ");
}

bool ChannelBox::channelIsChecked() const {
    return this->box->isChecked();
}

QCheckBox* ChannelBox::getCheckBox() const {
    return this->box;
}

void ChannelBox::setLabel(QString label, QColor color) {
    this->box->setText(label);

    QPalette p = box->palette();
    p.setColor(QPalette::Active, QPalette::WindowText, color);
    box->setPalette(p);
}
