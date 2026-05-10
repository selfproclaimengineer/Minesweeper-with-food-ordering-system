#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ROWS 15
#define MAX_COLS 15
#define MAX_ORDERS 20
#define NAME_SIZE 40

typedef struct {
    int category;
    int type;
    int duration_seconds;
    time_t ordered_at;
    time_t ready_at;
    int collected;
    char label[NAME_SIZE];
} FoodOrder;

typedef struct {
    int rows;
    int cols;
    int bombs;
    int map[MAX_ROWS][MAX_COLS];
    int adjacent[MAX_ROWS][MAX_COLS];
    int discovered[MAX_ROWS][MAX_COLS];
    int flagged[MAX_ROWS][MAX_COLS];
    int flags_left;
    int opened_cells;
    int first_move;
    int lost;
} Game;

FoodOrder orders[MAX_ORDERS];
int order_count = 0;

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        ;
    }
}

void print_notification_header(void) {
    printf("\n=== NOTIFIKASI PESANAN ===\n");
    int any = 0;
    time_t now = time(NULL);
    for (int i = 0; i < order_count; i++) {
        if (orders[i].collected) {
            continue;
        }
        any = 1;
        if (orders[i].ready_at <= now) {
            printf("- Pesanan '%s' sudah siap. Silakan ambil di menu order.\n", orders[i].label);
        } else {
            int remaining = (int)difftime(orders[i].ready_at, now);
            int minutes = remaining / 60;
            int seconds = remaining % 60;
            printf("- Pesanan '%s' akan siap dalam %d menit %d detik.\n", orders[i].label, minutes, seconds);
        }
    }
    if (!any) {
        printf("- Belum ada pesanan aktif.\n");
    }
    printf("===========================\n\n");
}

void add_food_order(int category, int type) {
    if (order_count >= MAX_ORDERS) {
        printf("Pesanan penuh, tidak bisa menambah lagi.\n");
        return;
    }
    FoodOrder *order = &orders[order_count++];
    order->category = category;
    order->type = type;
    order->collected = 0;
    order->ordered_at = time(NULL);
    if (category == 1) {
        order->duration_seconds = 3 * 60;
        if (type == 1) {
            strcpy(order->label, "Bakso Original");
        } else if (type == 2) {
            strcpy(order->label, "Bakso Pangsit");
        } else {
            strcpy(order->label, "Bakso Mercon");
        }
    } else if (category == 2) {
        order->duration_seconds = 1 * 60;
        if (type == 1) {
            strcpy(order->label, "Soto Ayam");
        } else {
            strcpy(order->label, "Soto Sapi");
        }
    } else {
        order->duration_seconds = 2 * 60;
        if (type == 1) {
            strcpy(order->label, "Mie Ayam Original");
        } else if (type == 2) {
            strcpy(order->label, "Mie Ayam Jamur");
        } else {
            strcpy(order->label, "Mie Ayam Tanpa Ayam");
        }
    }
    order->ready_at = order->ordered_at + order->duration_seconds;
    printf("Pesanan '%s' berhasil ditambahkan. Tunggu notifikasi pesanan siap.\n", order->label);
}

void collect_ready_orders(void) {
    time_t now = time(NULL);
    int any_ready = 0;
    for (int i = 0; i < order_count; i++) {
        if (!orders[i].collected && orders[i].ready_at <= now) {
            any_ready = 1;
            printf("Mengambil pesanan '%s'.\n", orders[i].label);
            orders[i].collected = 1;
        }
    }
    if (!any_ready) {
        printf("Belum ada pesanan yang siap diambil.\n");
    }
}

void order_menu(void) {
    while (1) {
        print_notification_header();
        printf("=== MENU ORDER MAKANAN ===\n");
        printf("1. Bakso (Original, Pangsit, Mercon)\n");
        printf("2. Soto (Ayam, Sapi)\n");
        printf("3. Mie Ayam (Original, Jamur, Tanpa Ayam)\n");
        printf("4. Ambil pesanan siap\n");
        printf("5. Kembali ke menu utama\n");
        printf("Pilih opsi: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Input tidak valid, coba lagi.\n");
            continue;
        }
        clear_input_buffer();

        if (choice == 5) {
            return;
        }
        if (choice == 4) {
            collect_ready_orders();
            continue;
        }
        if (choice < 1 || choice > 3) {
            printf("Opsi tidak tersedia.\n");
            continue;
        }

        if (choice == 1) {
            printf("Pilih jenis bakso: 1=Original, 2=Pangsit, 3=Mercon: ");
            int type;
            if (scanf("%d", &type) != 1) {
                clear_input_buffer();
                printf("Input tidak valid.\n");
                continue;
            }
            clear_input_buffer();
            if (type < 1 || type > 3) {
                printf("Jenis tidak tersedia.\n");
                continue;
            }
            add_food_order(1, type);
        } else if (choice == 2) {
            printf("Pilih jenis soto: 1=Ayam, 2=Sapi: ");
            int type;
            if (scanf("%d", &type) != 1) {
                clear_input_buffer();
                printf("Input tidak valid.\n");
                continue;
            }
            clear_input_buffer();
            if (type < 1 || type > 2) {
                printf("Jenis tidak tersedia.\n");
                continue;
            }
            add_food_order(2, type);
        } else if (choice == 3) {
            printf("Pilih jenis mie ayam: 1=Original, 2=Jamur, 3=Tanpa Ayam: ");
            int type;
            if (scanf("%d", &type) != 1) {
                clear_input_buffer();
                printf("Input tidak valid.\n");
                continue;
            }
            clear_input_buffer();
            if (type < 1 || type > 3) {
                printf("Jenis tidak tersedia.\n");
                continue;
            }
            add_food_order(3, type);
        }
    }
}

int read_level_file(const char *filename, Game *game) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Gagal membuka file '%s'. Pastikan file ada dan path benar.\n", filename);
        return 0;
    }

    game->bombs = 0;
    char line[256];
    int row = 0;
    while (row < game->rows && fgets(line, sizeof(line), file)) {
        int col = 0;
        char *token = strtok(line, " \t\r\n");
        while (token != NULL && col < game->cols) {
            if (token[0] != '0' && token[0] != '1') {
                fclose(file);
                printf("Format file tidak valid di baris %d. Gunakan 0 atau 1.\n", row + 1);
                return 0;
            }
            game->map[row][col] = token[0] - '0';
            if (game->map[row][col] == 1) {
                game->bombs++;
            }
            col++;
            token = strtok(NULL, " \t\r\n");
        }
        if (col != game->cols) {
            fclose(file);
            printf("File harus memiliki %d angka di setiap baris. Baris %d punya %d.\n", game->cols, row + 1, col);
            return 0;
        }
        row++;
    }
    fclose(file);
    if (row != game->rows) {
        printf("File harus memiliki %d baris, namun hanya ditemukan %d baris.\n", game->rows, row);
        return 0;
    }
    return 1;
}

void compute_adjacent(Game *game) {
    for (int r = 0; r < game->rows; r++) {
        for (int c = 0; c < game->cols; c++) {
            int count = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < game->rows && nc >= 0 && nc < game->cols) {
                        if (game->map[nr][nc] == 1) {
                            count++;
                        }
                    }
                }
            }
            game->adjacent[r][c] = count;
        }
    }
}

void display_game(const Game *game) {
    printf("\n=== MINESWEEPER ===\n");
    printf("   ");
    for (int c = 0; c < game->cols; c++) {
        printf("%2d", c);
    }
    printf("\n");
    for (int r = 0; r < game->rows; r++) {
        printf("%2d ", r);
        for (int c = 0; c < game->cols; c++) {
            if (game->flagged[r][c]) {
                printf(" F");
            } else if (!game->discovered[r][c]) {
                printf(" O");
            } else if (game->map[r][c] == 1) {
                printf(" B");
            } else if (game->adjacent[r][c] == 0) {
                printf("  ");
            } else {
                printf(" %d", game->adjacent[r][c]);
            }
        }
        printf("\n");
    }
    printf("Flags tersisa: %d\n", game->flags_left);
}

void open_cell_recursive(Game *game, int row, int col) {
    if (row < 0 || row >= game->rows || col < 0 || col >= game->cols) {
        return;
    }
    if (game->discovered[row][col] || game->flagged[row][col]) {
        return;
    }
    game->discovered[row][col] = 1;
    game->opened_cells++;
    if (game->adjacent[row][col] == 0) {
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                open_cell_recursive(game, row + dr, col + dc);
            }
        }
    }
}

int open_cell(Game *game, int row, int col) {
    if (row < 0 || row >= game->rows || col < 0 || col >= game->cols) {
        printf("Koordinat tidak valid. Gunakan nilai 0 sampai %d.\n", game->rows - 1);
        return 1;
    }
    if (game->flagged[row][col]) {
        printf("Kotak ini diberi flag. Hapus flag terlebih dahulu jika ingin membuka.\n");
        return 1;
    }
    if (game->discovered[row][col]) {
        printf("Kotak ini sudah terbuka.\n");
        return 1;
    }
    if (game->map[row][col] == 1) {
        game->discovered[row][col] = 1;
        game->lost = 1;
        printf("Boom! Anda membuka bom. Permainan berakhir.\n");
        return 0;
    }
    if (game->adjacent[row][col] == 0) {
        open_cell_recursive(game, row, col);
    } else {
        game->discovered[row][col] = 1;
        game->opened_cells++;
    }
    return 1;
}

int has_won(const Game *game) {
    if (game->flags_left != 0) {
        return 0;
    }
    for (int r = 0; r < game->rows; r++) {
        for (int c = 0; c < game->cols; c++) {
            if (game->map[r][c] == 1 && !game->flagged[r][c]) {
                return 0;
            }
            if (game->flagged[r][c] && game->map[r][c] != 1) {
                return 0;
            }
        }
    }
    return 1;
}

int play_minesweeper(void) {
    Game game;
    game.first_move = 1;
    game.lost = 0;
    game.opened_cells = 0;
    memset(&game, 0, sizeof(Game));

    printf("Pilih level Minesweeper: 1=10x10, 2=15x15\n");
    int level;
    if (scanf("%d", &level) != 1) {
        clear_input_buffer();
        printf("Input tidak valid. Kembali ke menu utama.\n");
        return 0;
    }
    clear_input_buffer();
    if (level == 1) {
        game.rows = 10;
        game.cols = 10;
    } else if (level == 2) {
        game.rows = 15;
        game.cols = 15;
    } else {
        printf("Level tidak tersedia. Kembali ke menu utama.\n");
        return 0;
    }

    char filename[128];
    printf("Masukkan nama file level (misal level1.txt): ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Gagal membaca nama file.\n");
        return 0;
    }
    filename[strcspn(filename, "\r\n")] = '\0';
    if (!read_level_file(filename, &game)) {
        return 0;
    }

    compute_adjacent(&game);
    game.flags_left = game.bombs;
    memset(game.discovered, 0, sizeof(game.discovered));
    memset(game.flagged, 0, sizeof(game.flagged));

    while (!game.lost) {
        print_notification_header();
        display_game(&game);
        if (has_won(&game)) {
            printf("Selamat! Semua bom berhasil ditandai dan flag habis. Anda menang!\n");
            return 1;
        }
        printf("Pilih aksi: 1=Pasang/hapus flag, 2=Buka area, 3=Keluar ke menu utama\n");
        int action;
        if (scanf("%d", &action) != 1) {
            clear_input_buffer();
            printf("Input tidak valid.\n");
            continue;
        }
        clear_input_buffer();
        if (action == 3) {
            printf("Keluar dari Minesweeper.\n");
            return 0;
        }
        if (action != 1 && action != 2) {
            printf("Aksi tidak tersedia.\n");
            continue;
        }
        printf("Masukkan koordinat X dan Y (contoh: 2 3): ");
        int x, y;
        if (scanf("%d %d", &x, &y) != 2) {
            clear_input_buffer();
            printf("Koordinat tidak valid.\n");
            continue;
        }
        clear_input_buffer();
        if (x < 0 || x >= game.cols || y < 0 || y >= game.rows) {
            printf("Koordinat berada di luar area permainan.\n");
            continue;
        }

        if (action == 1) {
            if (game.discovered[y][x]) {
                printf("Tidak bisa memberi flag pada area yang sudah terbuka.\n");
            } else {
                if (game.flagged[y][x]) {
                    game.flagged[y][x] = 0;
                    game.flags_left++;
                    printf("Flag di koordinat (%d,%d) dihapus.\n", x, y);
                } else {
                    game.flagged[y][x] = 1;
                    game.flags_left--;
                    printf("Flag ditempatkan di koordinat (%d,%d).\n", x, y);
                }
            }
        } else {
            if (!open_cell(&game, y, x)) {
                break;
            }
        }
        game.first_move = 0;
    }

    display_game(&game);
    printf("PERMAINAN SELESAI.\n");
    printf("Bom ada: %d. Flags tersisa: %d.\n", game.bombs, game.flags_left);
    return 0;
}

int main(void) {
    while (1) {
        print_notification_header();
        printf("=== WARNET MINESWEEPER DAN ORDER MAKANAN ===\n");
        printf("1. Main Minesweeper\n");
        printf("2. Order makanan\n");
        printf("3. Exit warnet\n");
        printf("Pilih menu: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Input tidak valid.\n");
            continue;
        }
        clear_input_buffer();
        if (choice == 1) {
            play_minesweeper();
        } else if (choice == 2) {
            order_menu();
        } else if (choice == 3) {
            printf("Terima kasih sudah bermain.\n");
            return 0;
        } else {
            printf("Opsi tidak tersedia.\n");
        }
    }
    return 0;
}
