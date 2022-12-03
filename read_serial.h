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

struct serialReader {
    int fd;
    void (*callback_ptr)(char *line);
};

// Returns: file descriptor or -1 on error
int serial_open(const char *dev);
// Returns: 0 on success, -1 on error
int serial_close(int ser_fd);
// Set struct serialReader. Function pointer is set to point to user defined function.
void serial_set_struct(struct serialReader *serStruct, int fd, void (*readline_funcptr)(char *line));
// Pthreads-compatible function for read-loop.
void *serial_read(void *serialStruct);

#endif // READ_SERIAL_H
