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
#include "screen.h"

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
        else if (c >= 32 && c <= 127)
            handle_insert_char(s, c);
    }

    screen_destroy_line(s); /* remove unnecessarily added line */
    screen_go_to_first_line(s);

    return true;
}

#define BUFF (((Line)curr->data)->buff)
bool file_save(Screen s) {
    s->file = freopen(s->args->file_name, "w", s->file);

    for (GList* curr = s->lines ; curr != NULL ; curr = curr->next) {
        for (int i = 0 ; i <= BUFF->end ; ++i) {
            if ((i < BUFF->gap_start || i > BUFF->gap_end) && BUFF->buffer[i])
                fputc(BUFF->buffer[i], s->file);
        }
    }

    return true;
}
#undef BUFF

bool file_close(Screen s) {
    return fclose(s->file) == 0;
}
