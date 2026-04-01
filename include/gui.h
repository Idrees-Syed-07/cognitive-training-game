#ifndef GUI_H
#define GUI_H

#include "maze.h"
#include "player.h"

//this function starts the gui
//argv is used for getting relative file paths and argc is needed for the GTK library
void run_gui(Graph *g, Player *p, int argc, char *argv[]);

#endif
