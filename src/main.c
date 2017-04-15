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

#include <ncurses.h>
#include <argp.h>
#include <glib-2.0/glib.h>

#include "screen.h"
#include "input.h"

const char* editor_version = "text-editor 0.1";
const char* program_bug_address = "<github.com/CajetanP>";

static char doc[] = "text-editor -- a simple text editor";

static struct argp_option options[] = {
    { "debug", 'd', 0, 0, "Enable debug mode", 0 },
    { 0, 0, 0, 0, 0, 0},
};

struct Arguments {
    bool debug_mode;
    /* TODO: make it do anything? */
    char* output_file;
};

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    /* get the input argument */
    struct Arguments* arguments = state->input;

    switch(key) {
    case 'd':
        arguments->debug_mode = true;
        break;
        /* TODO: make it do anything? */
    case 'o':
        arguments->output_file = arg;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num > 0)
            /* too many arguments */
            argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, 0, doc, 0, 0, 0 };

/* TODO: clean all of this argument handling code */

int main(int argc, char** argv) {

    /* parsing arguments */
    struct Arguments arguments;
    arguments.debug_mode = false;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    /* ncurses initialization */
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    /* colors */
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);

    /* create new "screen" */
    Screen s = screen_init();

    /* set debug mode accordingly */
    s->debug_mode = arguments.debug_mode;

    /* start input loop */
    input_loop(s);

    /* the program should never get here */
    return 1;
}
