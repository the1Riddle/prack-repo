#include "chess.h"

void update_top_moves(char* user) {
    /* Adds the given user to the top moves file */
    FILE* file = fopen("data/top_moves.txt", "r");
    if (file == NULL) {
        printf("ERROR: Failed to open top_moves.txt\n");
        return;
    }

    char contents[1000];
    if (fgets(contents, sizeof(contents), file) == NULL) {
        printf("ERROR: Failed to read top_moves.txt\n");
        fclose(file);
        return;
    }

    fclose(file);

    int dictionary_size = 0;
    char** dictionary = malloc(sizeof(char*));
    if (dictionary == NULL) {
        printf("ERROR: Failed to allocate memory for dictionary\n");
        return;
    }

    char* token = strtok(contents, ",");
    while (token != NULL) {
        dictionary[dictionary_size] = malloc(strlen(token) + 1);
        if (dictionary[dictionary_size] == NULL) {
            printf("ERROR: Failed to allocate memory for dictionary entry\n");
            for (int i = 0; i < dictionary_size; i++) {
                free(dictionary[i]);
            }
            free(dictionary);
            return;
        }

        strcpy(dictionary[dictionary_size], token);
        dictionary_size++;

        char** temp = realloc(dictionary, (dictionary_size + 1) * sizeof(char*));
        if (temp == NULL) {
            printf("ERROR: Failed to reallocate memory for dictionary\n");
            for (int i = 0; i < dictionary_size; i++) {
                free(dictionary[i]);
            }
            free(dictionary);
            return;
        }
        dictionary = temp;

        token = strtok(NULL, ",");
    }

    bool user_exists = false;
    for (int i = 0; i < dictionary_size; i++) {
        if (strcmp(dictionary[i], user) == 0) {
            user_exists = true;
            break;
        }
    }

    if (!user_exists) {
        dictionary[dictionary_size] = malloc(strlen(user) + 1);
        if (dictionary[dictionary_size] == NULL) {
            printf("ERROR: Failed to allocate memory for dictionary entry\n");
            for (int i = 0; i < dictionary_size; i++) {
                free(dictionary[i]);
            }
            free(dictionary);
            return;
        }

        strcpy(dictionary[dictionary_size], user);
        dictionary_size++;
    }

    file = fopen("data/top_moves.txt", "w");
    if (file == NULL) {
        printf("ERROR: Failed to open top_moves.txt\n");
        for (int i = 0; i < dictionary_size; i++) {
            free(dictionary[i]);
        }
        free(dictionary);
        return;
    }

    for (int i = 0; i < dictionary_size; i++) {
        fprintf(file, "%s,", dictionary[i]);
        free(dictionary[i]);
    }
    free(dictionary);

    fclose(file);
}

void update_last_moves(char* line) {
    /* Adds the given line to the last moves file */
    FILE* last_moves = fopen("data/last_moves.txt", "r+");
    if (last_moves == NULL) {
        printf("ERROR: Failed to open last_moves.txt\n");
        return;
    }

    char content[1000];
    if (fgets(content, sizeof(content), last_moves) == NULL) {
        printf("ERROR: Failed to read last_moves.txt\n");
        fclose(last_moves);
        return;
    }

    fseek(last_moves, 0, SEEK_SET);
    fprintf(last_moves, "%s\n%s", line, content);

    fclose(last_moves);
}

char* replace_text_between(char* original_text, char* marker_begin, char* marker_end, char* replacement_text) {
    /* Replace text between `marker_begin` and `marker_end` with `replacement_text` */
    char* delimiter_a = strstr(original_text, marker_begin);
    char* delimiter_b = strstr(original_text, marker_end);

    if (delimiter_a == NULL || delimiter_b == NULL) {
        return original_text;
    }

    int leading_text_length = delimiter_a - original_text;
    int trailing_text_length = strlen(original_text) - (delimiter_b - original_text) - strlen(marker_end);

    char* leading_text = malloc(leading_text_length + 1);
    if (leading_text == NULL) {
        printf("ERROR: Failed to allocate memory for leading_text\n");
        return original_text;
    }
    strncpy(leading_text, original_text, leading_text_length);
    leading_text[leading_text_length] = '\0';

    char* trailing_text = malloc(trailing_text_length + 1);
    if (trailing_text == NULL) {
        printf("ERROR: Failed to allocate memory for trailing_text\n");
        free(leading_text);
        return original_text;
    }
    strncpy(trailing_text, delimiter_b + strlen(marker_end), trailing_text_length);
    trailing_text[trailing_text_length] = '\0';

    int new_text_length = leading_text_length + strlen(replacement_text) + strlen(marker_begin) + strlen(marker_end) + trailing_text_length;
    char* new_text = malloc(new_text_length + 1);
    if (new_text == NULL) {
        printf("ERROR: Failed to allocate memory for new_text\n");
        free(leading_text);
        free(trailing_text);
        return original_text;
    }

    sprintf(new_text, "%s%s%s%s%s", leading_text, marker_begin, replacement_text, marker_end, trailing_text);

    free(leading_text);
    free(trailing_text);

    return new_text;
}

void parse_issue(char* title, int* action, char* move) {
    /* Parse issue title and return a tuple with (action, <move>) */
    if (strcmp(title, "chess: start new game") == 0) {
        *action = NEW_GAME;
        return;
    }

    if (strstr(title, "chess: move") != NULL) {
        char source[3];
        char dest[3];
        if (sscanf(title, "Chess: Move %2s to %2s", source, dest) == 2) {
            *action = MOVE;
            sprintf(move, "%s%s", source, dest);
            return;
        }
    }

    *action = UNKNOWN;
}

int main(int argc, char* argv[]) {
    if (argc >= 2 && strcmp(argv[1], "--self-test") == 0) {
        selftest_run(main);
        return 0;
    } else {
        char* issue_title = getenv("ISSUE_TITLE");
        char* issue_author = getenv("ISSUE_AUTHOR");
        char* repo_owner = getenv("REPO_OWNER");

        int action;
        char move[5];
        parse_issue(issue_title, &action, move);

        Board gameboard;
        init_board(&gameboard);

        FILE* settings_file = fopen("data/settings.yaml", "r");
        if (settings_file == NULL) {
            printf("ERROR: Failed to open settings.yaml\n");
            return 1;
        }

        char settings[1000];
        if (fgets(settings, sizeof(settings), settings_file) == NULL) {
            printf("ERROR: Failed to read settings.yaml\n");
            fclose(settings_file);
            return 1;
        }

        fclose(settings_file);

        if (action == NEW_GAME) {
            if (access("games/current.pgn", F_OK) == 0 && strcmp(issue_author, repo_owner) != 0) {
                printf("ERROR: A current game is in progress. Only the repo owner can start a new game\n");
                return 1;
            }

            printf("Successful new game\n");

            FILE* last_moves = fopen("data/last_moves.txt", "w");
            if (last_moves == NULL) {
                printf("ERROR: Failed to open last_moves.txt\n");
                return 1;
            }

            fprintf(last_moves, "Start game: %s", issue_author);

            fclose(last_moves);

            // Create new game
            Game game;
            init_game(&game);
            strcpy(game.headers[EVENT], strcat(repo_owner, "'s Online Open Chess Tournament"));
            strcpy(game.headers[SITE], "https://github.com/");
            strcat(game.headers[SITE], getenv("GITHUB_REPOSITORY"));
            strcpy(game.headers[DATE], "YYYY.MM.DD");
            strcpy(game.headers[ROUND], "1");
        } else if (action == MOVE) {
            if (access("games/current.pgn", F_OK) != 0) {
                printf("ERROR: There is no game in progress! Start a new game first\n");
                return 1;
            }

            // Load game from "games/current.pgn"
            FILE* pgn_file = fopen("games/current.pgn", "r");
            if (pgn_file == NULL) {
                printf("ERROR: Failed to open current.pgn\n");
                return 1;
            }

            Game game;
            init_game(&game);
            load_game(&game, pgn_file);
            fclose(pgn_file);

            FILE* moves = fopen("data/last_moves.txt", "r");
            if (moves == NULL) {
                printf("ERROR: Failed to open last_moves.txt\n");
                return 1;
            }

            char line[1000];
            if (fgets(line, sizeof(line), moves) == NULL) {
                printf("ERROR: Failed to read last_moves.txt\n");
                fclose(moves);
                return 1;
            }

            fclose(moves);

            char last_player[100];
            char last_move[100];
            sscanf(line, "%[^:]: %s", last_move, last_player);

            for (int i = 0; i < game.num_moves; i++) {
                make_move(&game, game.moves[i]);
            }

            if (tolower(move[0]) == tolower(move[2]) && tolower(move[1]) == tolower(move[3])) {
                printf("ERROR: Move is invalid!\n");
                return 1;
            }

            // Try to move with promotion to queen
            char promoted_move[5];
            sprintf(promoted_move, "%sq", move);
            if (is_valid_move(&gameboard, promoted_move)) {
                strcpy(move, promoted_move);
            }

            if (!is_valid_move(&gameboard, move)) {
                printf("ERROR: Move is invalid!\n");
                return 1;
            }

            if (!is_valid_board(&gameboard)) {
                printf("ERROR: Board is invalid!\n");
                return 1;
            }

            char* issue_labels[2];
            if (is_capture(&gameboard, move)) {
                issue_labels[0] = "⚔️ Capture!";
                issue_labels[1] = (gameboard.turn == WHITE) ? "White" : "Black";
            } else {
                issue_labels[0] = (gameboard.turn == WHITE) ? "White" : "Black";
                issue_labels[1] = NULL;
            }

            printf("Successful move\n");

            update_last_moves(strcat(move, strcat(": ", issue_author)));
            update_top_moves(issue_author);

            // Perform move
            make_move(&game, move);
            add_main_variation(&game, move, issue_author);
            strcpy(game.headers[RESULT], get_game_result(&gameboard));
        } else {
            printf("ERROR: Unknown action\n");
            return 1;
        }

        FILE* pgn_file = fopen("games/current.pgn", "w");
        if (pgn_file == NULL) {
            printf("ERROR: Failed to open current.pgn\n");
            return 1;
        }

        save_game(&game, pgn_file);
        fclose(pgn_file);

        char* readme = malloc(10000);
        if (readme == NULL) {
            printf("ERROR: Failed to allocate memory for readme\n");
            return 1;
        }

        FILE* file = fopen("README.md", "r");
        if (file == NULL) {
            printf("ERROR: Failed to open README.md\n");
            free(readme);
            return 1;
        }

        if (fgets(readme, 10000, file) == NULL) {
            printf("ERROR: Failed to read README.md\n");
            fclose(file);
            free(readme);
            return 1;
        }

        fclose(file);

        char* new_board = replace_text_between(readme, "{chess_board}", gameboard_to_markdown(&gameboard));
        char* new_moves_list = replace_text_between(new_board, "{moves_list}", generate_moves_list(&gameboard));
        char* new_turn = replace_text_between(new_moves_list, "{turn}", (gameboard.turn == WHITE) ? "white" : "black");
        char* new_last_moves = replace_text_between(new_turn, "{last_moves}", generate_last_moves());
        char* new_readme = replace_text_between(new_last_moves, "{top_moves}", generate_top_moves());

        file = fopen("README.md", "w");
        if (file == NULL) {
            printf("ERROR: Failed to open README.md\n");
            free(new_board);
            free(new_moves_list);
            free(new_turn);
            free(new_last_moves);
            free(new_readme);
            return 1;
        }

        fprintf(file, "%s", new_readme);

        fclose(file);

        free(new_board);
        free(new_moves_list);
        free(new_turn);
        free(new_last_moves);
        free(new_readme);

        return 0;
    }
}
