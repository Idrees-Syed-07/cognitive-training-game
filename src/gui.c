#include <gtk/gtk.h>
#include "gui.h"
#include "player.h"
#include "audio.h"

#define CELL_SIZE 128
#define COLOR_LIGHT "#f0d9b5"
#define COLOR_DARK  "#b58863"

static Graph *g_graph;
static Player *g_player;
static char *g_coin_path;
static char *g_player_path;
static char *g_wall_sound_path;
static GtkWidget *g_grid;

/* Maps grid (x, y) -> image widget for quick updates */
static GtkWidget *g_cells[GRID_SIZE][GRID_SIZE];

static void update_cell(int x, int y) {
    Node *node = NULL;

    /* Find the node at this (x, y) position */
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (g_graph->nodes[r][c]->x == x && g_graph->nodes[r][c]->y == y) {
                node = g_graph->nodes[r][c];
                break;
            }
        }
        if (node) break;
    }

    GtkWidget *image = g_cells[x][y];

    if (node == g_player->node) {
        gtk_image_set_from_file(GTK_IMAGE(image), g_player_path);
        gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
    } else if (node && node->has_coin) {
        gtk_image_set_from_file(GTK_IMAGE(image), g_coin_path);
        gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
    } else {
        gtk_image_clear(GTK_IMAGE(image));
    }
}

static void show_win_dialog(GtkWidget *window) {
    GtkAlertDialog *dialog = gtk_alert_dialog_new("You collected all the coins! You win!");
    gtk_alert_dialog_show(dialog, GTK_WINDOW(window));
    g_object_unref(dialog);
}

static gboolean on_key_pressed(GtkEventControllerKey *controller,
                                guint keyval, guint keycode,
                                GdkModifierType state, gpointer user_data) {
    (void)controller;
    (void)keycode;
    (void)state;

    char dir = 0;
    switch (keyval) {
        case GDK_KEY_w: case GDK_KEY_W: dir = 'w'; break;
        case GDK_KEY_a: case GDK_KEY_A: dir = 'a'; break;
        case GDK_KEY_s: case GDK_KEY_S: dir = 's'; break;
        case GDK_KEY_d: case GDK_KEY_D: dir = 'd'; break;
        default: return FALSE;
    }

    int old_x = g_player->node->x;
    int old_y = g_player->node->y;

    MoveResult result = player_move(g_player, dir);

    if (result == MOVE_WALL) {
        audio_play(g_wall_sound_path);
        return TRUE;
    }

    /* Update the old cell and the new cell */
    update_cell(old_x, old_y);
    update_cell(g_player->node->x, g_player->node->y);

    if (result == MOVE_WIN) {
        show_win_dialog(GTK_WIDGET(user_data));
    }

    return TRUE;
}

static void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
        ".cell-light { background-color: " COLOR_LIGHT "; }"
        ".cell-dark  { background-color: " COLOR_DARK "; }";
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

static GtkWidget *build_grid(void) {
    g_grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(g_grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(g_grid), TRUE);

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            Node *node = g_graph->nodes[row][col];
            GtkWidget *image;

            if (node == g_player->node) {
                image = gtk_image_new_from_file(g_player_path);
                gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
            } else if (node->has_coin) {
                image = gtk_image_new_from_file(g_coin_path);
                gtk_image_set_pixel_size(GTK_IMAGE(image), CELL_SIZE);
            } else {
                image = gtk_image_new();
            }

            /* Wrap image in a box with checkerboard background */
            GtkWidget *cell = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_widget_set_size_request(cell, CELL_SIZE, CELL_SIZE);
            gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
            gtk_box_append(GTK_BOX(cell), image);

            const char *css_class = ((node->x + node->y) % 2 == 0)
                                    ? "cell-light" : "cell-dark";
            gtk_widget_add_css_class(cell, css_class);

            gtk_grid_attach(GTK_GRID(g_grid), cell, node->x, node->y, 1, 1);
            g_cells[node->x][node->y] = image;
        }
    }

    return g_grid;
}

static void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    load_css();

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Cognitive Training Game");
    gtk_window_set_default_size(GTK_WINDOW(window),
                                CELL_SIZE * GRID_SIZE, CELL_SIZE * GRID_SIZE);

    GtkWidget *grid = build_grid();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    /* Add keyboard controller */
    GtkEventController *key_ctrl = gtk_event_controller_key_new();
    g_signal_connect(key_ctrl, "key-pressed", G_CALLBACK(on_key_pressed), window);
    gtk_widget_add_controller(window, key_ctrl);

    gtk_window_present(GTK_WINDOW(window));
}

int gui_run(Graph *graph, int argc, char *argv[]) {
    g_graph = graph;
    g_player = player_create(graph);
    if (!g_player) {
        fprintf(stderr, "Failed to create player.\n");
        return 1;
    }

    /* Build asset paths relative to the executable */
    char *exe_dir = g_path_get_dirname(argv[0]);
    g_coin_path = g_build_filename(exe_dir, "..", "assets", "images", "coin.png", NULL);
    g_player_path = g_build_filename(exe_dir, "..", "assets", "images", "player.png", NULL);
    g_wall_sound_path = g_build_filename(exe_dir, "..", "assets", "sounds", "wall.mp3", NULL);
    g_free(exe_dir);

    GtkApplication *app = gtk_application_new("com.cognitive.training",
                                               G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    player_free(g_player);
    g_free(g_coin_path);
    g_free(g_player_path);
    g_free(g_wall_sound_path);
    return status;
}
