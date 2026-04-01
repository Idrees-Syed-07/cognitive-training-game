#include <gtk/gtk.h>
#include "gui.h"
#include "player.h"
#include "audio.h"

#define CELL_SIZE 128
#define COLOR_LIGHT "#f0d9b5"
#define COLOR_DARK  "#b58863"

static Graph *maze;
static Player *player;
static char *coin_image_path;
static char *player_image_path;
static GtkWidget *grid;

static GtkWidget *cells[GRID_SIZE][GRID_SIZE];
static Node *nodes[GRID_SIZE][GRID_SIZE]; 

//updates the image of a cell based on its current state (empty, has coin, or player)
static void update_cell(int x, int y) {
    Node *node = nodes[x][y];
    GtkWidget *image = cells[x][y];

    if (node == player->node) {
        gtk_image_set_from_file(GTK_IMAGE(image), player_image_path);
        gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
    } else if (node && node->has_coin) {
        gtk_image_set_from_file(GTK_IMAGE(image), coin_image_path);
        gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
    } else {
        gtk_image_clear(GTK_IMAGE(image));
    }
}

//closes program when dialogue box is closed
static void dialogue_close(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    gtk_window_destroy(window);
}

//shows a dialogue box when the player wins
static void show_win_dialogue(GtkWidget *window) {
    GtkAlertDialog *dialogue = gtk_alert_dialog_new("You collected all the coins! You win!");
    gtk_alert_dialog_choose(dialogue, GTK_WINDOW(window), NULL, dialogue_close, window);
    g_object_unref(dialogue);
}

//moves the player when a key is pressed 
static gboolean key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    //getting direction based on which key was pressed
    Direction direction;
    if (keyval == GDK_KEY_w || keyval == GDK_KEY_W) direction = UP;
    else if (keyval == GDK_KEY_a || keyval == GDK_KEY_A) direction = LEFT;
    else if (keyval == GDK_KEY_s || keyval == GDK_KEY_S) direction = DOWN;
    else if (keyval == GDK_KEY_d || keyval == GDK_KEY_D) direction = RIGHT;
    else return FALSE;

    int old_x = player->node->visual_position[0];
    int old_y = player->node->visual_position[1];

    //checking what happens when player tries to move in that direction
    MoveResult result = move_player(player, direction);

    if (result == MOVE_WALL) {
        play_audio();
        return TRUE;
    }

    //updating screen
    update_cell(old_x, old_y);
    update_cell(player->node->visual_position[0], player->node->visual_position[1]);

    if (result == MOVE_WIN) show_win_dialogue(GTK_WIDGET(user_data));
    
    return TRUE;
}

//loading css used for cell background colours
static void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();

    char *css = ".cell-light { background-color: " COLOR_LIGHT "; }.cell-dark  { background-color: " COLOR_DARK "; }";

    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
                                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
}

//creating grid of cells and populating it
static GtkWidget *build_grid(void) {
    grid = gtk_grid_new();

    //all cells same size
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    //for each cell
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            GtkWidget *image;
            Node *node = maze->nodes[row][col];

            //showing image based on what the cell has
            if (node == player->node) {
                image = gtk_image_new_from_file(player_image_path);
                gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
            } else if (node->has_coin) {
                image = gtk_image_new_from_file(coin_image_path);
                gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
            } else {
                image = gtk_image_new();
            }

            //adding image to the cell and styling using css
            GtkWidget *cell = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_widget_set_size_request(cell, CELL_SIZE, CELL_SIZE);
            gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(cell), image);
            const char *css_class = ((node->visual_position[0] + node->visual_position[1]) % 2 == 0) ? "cell-light" : "cell-dark";
            gtk_widget_add_css_class(cell, css_class);

            //adding cell to grid and soring pointers for updating gui later
            gtk_grid_attach(GTK_GRID(grid), cell, node->visual_position[0], node->visual_position[1], 1, 1);
            cells[node->visual_position[0]][node->visual_position[1]] = image;
            nodes[node->visual_position[0]][node->visual_position[1]] = node;
        }
    }

    return grid;
}

//start up the gui
static void activate(GtkApplication *game, gpointer user_data) {
    load_css();

    //creating window
    GtkWidget *window = gtk_application_window_new(game);
    gtk_window_set_title(GTK_WINDOW(window), "Cognitive Training Game");
    gtk_window_set_default_size(GTK_WINDOW(window), CELL_SIZE * GRID_SIZE, CELL_SIZE * GRID_SIZE);

    //creating grid in window
    GtkWidget *grid = build_grid();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    //setting up key press event
    GtkEventController *key_ctrl = gtk_event_controller_key_new();
    g_signal_connect(key_ctrl, "key-pressed", G_CALLBACK(key_pressed), window);
    gtk_widget_add_controller(window, key_ctrl);

    //showing window
    gtk_window_present(GTK_WINDOW(window));
}

void run_gui(Graph *g, Player *p, int argc, char *argv[]) {
    maze = g;
    player = p;

    //file names of assets based on exe location
    char *exe_dir = g_path_get_dirname(argv[0]);
    coin_image_path = g_build_filename(exe_dir, "..", "assets", "images", "coin.png", NULL);
    player_image_path = g_build_filename(exe_dir, "..", "assets", "images", "player.png", NULL);
    char *wall_sound_path = g_build_filename(exe_dir, "..", "assets", "sounds", "wall.wav", NULL);
    g_free(exe_dir);

    load_audio(wall_sound_path);
    g_free(wall_sound_path);

    //starting application
    GtkApplication *game = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(game, "activate", G_CALLBACK(activate), NULL);
    g_application_run(G_APPLICATION(game), argc, argv);

    //cleaning up
    g_object_unref(game);
    free_audio();
    g_free(coin_image_path);
    g_free(player_image_path);
}