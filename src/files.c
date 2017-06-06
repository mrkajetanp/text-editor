/************************************************************************
 * text-editor - a simple text editor                                   *
 *                                                                      *
 * Copyright (C) 2017 Kajetan Puchalski                                 *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                 *
 * See the GNU General Public License for more details.                 *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program. If not, see http://www.gnu.org/licenses/.   *
 *                                                                      *
 ************************************************************************/

#include <stdbool.h>

#include "files.h"
#include "input.h"

bool file_open(Screen s, char* name) {
    s->file = fopen(name, "r+");

    if (!s->file)
        return false;

    int c;
    while ((c = fgetc(s->file)) != EOF) {
        if (c == '\n')
            handle_enter(s);
        else if (c == '\t')
            handle_tab(s);
        else
            handle_insert_char(s, c);
    }

    return true;
}

bool file_close(Screen s) {
    return fclose(s->file) == 0;
}
