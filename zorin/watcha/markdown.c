#include "chess.h"

#define MAX_TOP_MOVES 10
#define MAX_LAST_MOVES 5

typedef struct {
    char source[3];
    char dest[3];
} Move;

typedef struct {
    int total_moves;
    char user[100];
} TopMove;

typedef struct {
    Move move;
    char author[100];
} LastMove;

typedef struct {
    Move moves[MAX_TOP_MOVES];
    int num_moves;
} MovesList;

typedef struct {
    char board[8][8];
} Board;

void create_link(char* text, char* link, char* result) {
    sprintf(result, "[%s](%s)", text, link);
}

void create_issue_link(char* source, char** dest_list, int num_dest, char* result) {
    char issue_link[200];
    char params[200];
    char encoded_params[200];

    sprintf(params, "{\"source\":\"%s\",\"dest\":[", source);
    for (int i = 0; i < num_dest; i++) {
        strcat(params, "\"");
        strcat(params, dest_list[i]);
        strcat(params, "\"");
        if (i < num_dest - 1) {
            strcat(params, ",");
        }
    }
    strcat(params, "]}");

    url_encode(params, encoded_params);

    sprintf(issue_link, settings.issues.link, getenv("GITHUB_REPOSITORY"), encoded_params);

    create_link(dest, issue_link, result);
}

void generate_top_moves(TopMove* top_moves, int* num_top_moves) {
    FILE* file = fopen("data/top_moves.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    char line[200];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL && count < MAX_TOP_MOVES) {
        sscanf(line, "%d %s", &top_moves[count].total_moves, top_moves[count].user);
        count++;
    }

    fclose(file);

    *num_top_moves = count;
}

void generate_last_moves(LastMove* last_moves, int* num_last_moves) {
    FILE* file = fopen("data/last_moves.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    char line[200];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL && count < MAX_LAST_MOVES) {
        char* token = strtok(line, ":");
        if (token == NULL) {
            continue;
        }

        strcpy(last_moves[count].move.source, token);

        token = strtok(NULL, ":");
        if (token == NULL) {
            continue;
        }

        strcpy(last_moves[count].move.dest, token);

        token = strtok(NULL, ":");
        if (token == NULL) {
            continue;
        }

        strcpy(last_moves[count].author, token);

        count++;
    }

    fclose(file);

    *num_last_moves = count;
}

void generate_moves_list(Board* board, MovesList* moves_list) {
    char moves_dict[8][8][8];
    memset(moves_dict, 0, sizeof(moves_dict));

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char source[3];
            char dest[3];

            sprintf(source, "%c%d", 'A' + i, 8 - j);
            sprintf(dest, "%c%d", 'A' + i, 8 - j);

            if (board->board[i][j] != '.') {
                strcat(moves_dict[i][j], dest);
            }
        }
    }

    char issue_link[200];
    char params[200];
    char encoded_params[200];

    if (board_is_game_over(board)) {
        sprintf(params, "{\"new_game\":true}");

        url_encode(params, encoded_params);

        sprintf(issue_link, settings.issues.link, getenv("GITHUB_REPOSITORY"), encoded_params);

        sprintf(moves_list->moves[moves_list->num_moves].source, "**GAME IS OVER!**");
        create_link("Click here", issue_link, moves_list->moves[moves_list->num_moves].dest);
        moves_list->num_moves++;
        return;
    }

    if (board_is_check(board)) {
        sprintf(moves_list->moves[moves_list->num_moves].source, "**CHECK!** Choose your move wisely!");
        moves_list->num_moves++;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (strlen(moves_dict[i][j]) > 0) {
                create_issue_link(moves_dict[i][j], moves_dict[i][j], moves_list->moves[moves_list->num_moves].dest);
                moves_list->num_moves++;
            }
        }
    }
}

void board_to_markdown(Board* board, char* markdown) {
    char images[8][8][50];
    memset(images, 0, sizeof(images));

    strcpy(images['r' - 'a'][0], "img/black/rook.png");
    strcpy(images['n' - 'a'][0], "img/black/knight.png");
    strcpy(images['b' - 'a'][0], "img/black/bishop.png");
    strcpy(images['q' - 'a'][0], "img/black/queen.png");
    strcpy(images['k' - 'a'][0], "img/black/king.png");
    strcpy(images['p' - 'a'][0], "img/black/pawn.png");

    strcpy(images['R' - 'A'][0], "img/white/rook.png");
    strcpy(images['N' - 'A'][0], "img/white/knight.png");
    strcpy(images['B' - 'A'][0], "img/white/bishop.png");
    strcpy(images['Q' - 'A'][0], "img/white/queen.png");
    strcpy(images['K' - 'A'][0], "img/white/king.png");
    strcpy(images['P' - 'A'][0], "img/white/pawn.png");

    strcpy(images['.'][0], "img/blank.png");

    if (board->turn == BLACK) {
        sprintf(markdown, "|   | H | G | F | E | D | C | B | A |   |\n");
    } else {
        sprintf(markdown, "|   | A | B | C | D | E | F | G | H |   |\n");
    }
    strcat(markdown, "|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|\n");

    for (int i = 0; i < 8; i++) {
        sprintf(markdown, "| **%d** | ", 8 - i);
        for (int j = 0; j < 8; j++) {
            strcat(markdown, "<img src=\"");
            strcat(markdown, images[board->board[i][j] - 'a'][0]);
            strcat(markdown, "\" width=50px> | ");
        }
        sprintf(markdown, "**%d** |\n", 8 - i);
    }

    if (board->turn == BLACK) {
        strcat(markdown, "|   | **H** | **G** | **F** | **E** | **D** | **C** | **B** | **A** |   |\n");
    } else {
        strcat(markdown, "|   | **A** | **B** | **C** | **D** | **E** | **F** | **G** | **H** |   |\n");
    }
}
