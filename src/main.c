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

#include "screen.h"
#include "input.h"

/*****************************************************************************/
/*                      Handling command line arguments                      */
/*****************************************************************************/

const char* editor_version = "text-editor 0.1";
const char* program_bug_address = "<github.com/CajetanP>";

/* general information about the editor */
static char doc[] = "text-editor -- a simple text editor";

/* usage */
static char args_doc[] = "[FILE]";

/* possible arguments */
static struct argp_option options[] = {
    { "debug", 'd', 0, 0, "Enable debug mode", 0 },
    { 0, 0, 0, 0, 0, 0},
};

/* parse given command line argument */
static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    /* get the input argument */
    struct Arguments* arguments = state->input;

    switch(key) {
    case 'd':
        arguments->debug_mode = true;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
            /* too many arguments */
            argp_usage(state);

        arguments->file_name = arg;
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

/* argp control structure */
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

/*****************************************************************************/
/*                       General editor initialization                       */
/*****************************************************************************/

int main(int argc, char** argv) {
    /* parsing command line arguments */
    struct Arguments arguments;
    arguments.debug_mode = false;
    arguments.file_name = "";
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    /* ncurses initialization */
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    /* colors */
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    /* create new "screen" */
    Screen s = screen_init(&arguments);

    /* start input loop */
    input_loop(s);

    /* the program should never get here */
    return 1;
}
