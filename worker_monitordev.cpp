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

#include <unistd.h>
#include <sys/inotify.h>

#include "worker_monitordev.h"

Worker_monitorDev::Worker_monitorDev(QObject *parent) : QObject{parent} {

}

void Worker_monitorDev::monitor() {
    int fd =inotify_init();
    if (fd == -1) {
        perror("Inotify_init failed");
        return;
    }
    else {
        int fd_w = inotify_add_watch(fd, "/dev", IN_CREATE | IN_DELETE);
        if (fd_w == -1) {
            perror("Inotify_add_watch failed");
            close(fd);
            emit finished();
            return;
        }

        char buf[sizeof(struct inotify_event)+PATH_MAX+1] __attribute__((aligned(__alignof__(struct inotify_event))));
        const struct inotify_event *event;

        while(1) {
            ssize_t bytes = read(fd, buf, sizeof(buf));

            if (bytes <= 0)
                break;

            for(char *ptr = buf; ptr < buf + bytes; ptr += sizeof(struct inotify_event) + event->len) {
                event = (const struct inotify_event *)ptr;
                if (event->wd == fd_w) {

                    if (event->len) {
                        std::cout << "File " << event->name;
                        if (event->mask == IN_CREATE)
                            std::cout << " created.";
                        else if (event->mask == IN_DELETE)
                            std::cout << " deleted.";
                        std::cout << std::endl;

                        emit filesCreatedDeleted();
                    }
                }
                else
                    std::cerr << "Other inotify events should not happen." << std::endl;
            }
        }
        close(fd_w);
        close(fd);

    }

    emit finished();
}
