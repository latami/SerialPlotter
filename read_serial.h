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

#ifndef READ_SERIAL_H
#define READ_SERIAL_H

/* Re-define baudrates from termios.h */
const unsigned int baud_rates[] = {
    /* termios.h */
    0000001,    // B50
    0000002,    // B75
    0000003,    // B110
    0000004,    // B134
    0000005,    // B150
    0000006,    // B200
    0000007,    // B300
    0000010,    // B600
    0000011,    // B1200
    0000012,    // B1800
    0000013,    // B2400
    0000014,    // B4800
    0000015,    // B9600
    0000016,    // B19200
    0000017,    // B38400
    /* termios-baud.h */
    0010001,    // B57600
    0010002,    // B115200
    0010003,    // B230400
    0010004,    // B460800
    0010005,    // B500000
    0010006,    // B576000
    0010007,    // B921600
    0010010,    // B1000000
    0010011,    // B1152000
    0010012,    // B1500000
    0010013,    // B2000000
    0010014,    // B2500000
    0010015,    // B3000000
    0010016,    // B3500000
    0010017     // B4000000
};

struct serialReader {
    int fd;
    void (*callback_ptr)(char *line);
};
/* dev: device name including path.
   baudrate: input values from array baud_rates.
   Returns: file descriptor or -1 on error. */
int serial_open(const char *dev, unsigned int baudrate);
// Returns: 0 on success, -1 on error
int serial_close(int ser_fd);
// Set struct serialReader. Function pointer is set to point to user defined function.
void serial_set_struct(struct serialReader *serStruct, int fd, void (*readline_funcptr)(char *line));
// Pthreads-compatible function for read-loop.
void *serial_read(void *serialStruct);

#endif // READ_SERIAL_H
