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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "read_serial.h"

#define READBUF_SIZE 1024

int serial_open(const char *dev, unsigned int baudrate) {
    int ser_fd = open(dev, O_RDONLY);
    if (ser_fd == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return ser_fd;
    }

    struct termios tty;

    if (tcgetattr(ser_fd, &tty) != 0) {
        fprintf(stderr, "tcgetattr() failed: %s\n", strerror(errno));
        close(ser_fd);
        return -1;
    }

    tty.c_cflag &= ~PARENB;         // Clear parity bit.
    tty.c_cflag &= ~CSTOPB;         // Only one stop bit.

    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;             // 8 bits per byte.

    tty.c_cflag &= ~CRTSCTS;        // No RTS/CTS hardware flow ctrl.
    tty.c_cflag |= CREAD | CLOCAL;  // Read data and ignore ctrl lines.


    tty.c_lflag &= ~ICANON;         // Disable canonical mode (No per line read).


    // After Platformio serial monitor, this was all zero.
    // See bits/termios-c_lflag.h for meaning.
    tty.c_lflag = 0;


    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow ctrl.
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable all special handling.


    tty.c_oflag &= ~OPOST;          // Do not interpret output bytes.
    tty.c_oflag &= ~ONLCR;          // Do not convert newline to carriage return/line feed.

    // Block forever for 1 byte.
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN]  = 1;


    cfsetispeed(&tty, baudrate);
    cfsetospeed(&tty, baudrate);

    if (tcsetattr(ser_fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Setting tty failed: %s\n", strerror(errno));
        close(ser_fd);
        return -1;
    }

    return ser_fd;
}

int serial_close(int ser_fd) {
    int ret = 0;
    if ((ret = close(ser_fd)) == -1)
        fprintf(stderr, "Error closing serial: %s\n", strerror(errno));

    return ret;
}

void serial_set_struct(struct serialReader *serStruct, int fd, void (*readline_funcptr)(char *line)) {
    serStruct->fd = fd;
    serStruct->callback_ptr = readline_funcptr;
}

void *serial_read(void *serialStruct) {
    struct serialReader *ser = serialStruct;

    char str[READBUF_SIZE];

    int i = 0;
    while(1) {
        ssize_t byte = read(ser->fd, &str[i], 1);
        if (byte == -1) {
            fprintf(stderr, "Read error: %s\n", strerror(errno));
            break;
        }
        else if (byte == 0) {
            fprintf(stderr, "Read zero bytes!\n");
            break;
        }
        if (str[i] == '\n') {
            // Newline detected, call function set by pointer.
            str[i+1] = '\0';
            ser->callback_ptr(str);
            i = 0;
        }
        else {
            i++;
            // If no room for newline AND null byte, reset index to zero.
            if (i == READBUF_SIZE-2) {
                fprintf(stderr, "No newline seen, setting index to beginning of read buffer.");
                i = 0;
            }
        }
    }
    return NULL;
}
