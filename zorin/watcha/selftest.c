#include "chess.h"

void get_test_data(char** labels, char** comments, char* move, char* author, char* owner, int i) {
    if (strstr(move, "Start new game") != NULL) {
        if (strcmp(author, owner) == 0) {
            char* successful_new_game = malloc(strlen(settings.comments.successful_new_game) + strlen(author) + 1);
            sprintf(successful_new_game, settings.comments.successful_new_game, author);
            comments[i] = successful_new_game;
        } else {
            char* invalid_new_game = malloc(strlen(settings.comments.invalid_new_game) + strlen(author) + 1);
            sprintf(invalid_new_game, settings.comments.invalid_new_game, author);
            comments[i] = invalid_new_game;
        }
    } else if ((move_data.is_consecutive == false || !move_data.is_consecutive) && (move_data.is_invalid == false || !move_data.is_invalid)) {
        char* color = malloc(6);
        if (i % 2 == 1) {
            strcpy(color, "White");
        } else {
            strcpy(color, "Black");
        }
        labels[i] = color;

        char* successful_move = malloc(strlen(settings.comments.successful_move) + strlen(author) + strlen(move) + 1);
        sprintf(successful_move, settings.comments.successful_move, author, move);
        comments[i] = successful_move;
    }

    if (move_data.is_winner == true || move_data.is_winner) {
        labels[i] = "👑 Winner!";

        char* game_over = malloc(strlen(settings.comments.game_over) + 50);
        sprintf(game_over, settings.comments.game_over, ".+", "\\d+", "\\d+", "(@.+,)* @.+");
        comments[i] = game_over;
    }

    if (move_data.is_draw == true || move_data.is_draw) {
        labels[i] = "👑 Draw!";

        char* game_over = malloc(strlen(settings.comments.game_over) + 50);
        sprintf(game_over, settings.comments.game_over, ".+", "\\d+", "\\d+", "(@.+,)* @.+");
        comments[i] = game_over;
    }

    if (move_data.is_capture == true || move_data.is_capture) {
        labels[i] = "⚔️ Capture!";
    }

    if (move_data.is_consecutive == true || move_data.is_consecutive) {
        labels[i] = "Invalid";

        char* consecutive_moves = malloc(strlen(settings.comments.consecutive_moves) + strlen(author) + 1);
        sprintf(consecutive_moves, settings.comments.consecutive_moves, author);
        comments[i] = consecutive_moves;
    }

    if (move_data.is_invalid == true || move_data.is_invalid) {
        labels[i] = "Invalid";

        char* invalid_move = malloc(strlen(settings.comments.invalid_move) + strlen(author) + strlen(move) + 1);
        sprintf(invalid_move, settings.comments.invalid_move, author, move);
        comments[i] = invalid_move;
    }
}

void run_test_case(char* filename, void (*main_fn)(Issue*, char*, char*)) {
    int passed = 0;
    int failed = 0;

    FILE* test_file = fopen(filename, "r");
    yaml_parser_t parser;
    yaml_event_t event;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, test_file);

    while (1) {
        if (!yaml_parser_parse(&parser, &event)) {
            break;
        }

        if (event.type == YAML_SCALAR_EVENT) {
            char* name = event.data.scalar.value;
            printf("\u001b[0m\u001b[1m\u001b[37m  %s\n", name);
        }

        if (event.type == YAML_SEQUENCE_START_EVENT) {
            int i = 0;
            while (1) {
                if (!yaml_parser_parse(&parser, &event)) {
                    break;
                }

                if (event.type == YAML_MAPPING_START_EVENT) {
                    Issue* issue = malloc(sizeof(Issue));
                    issue->move = NULL;
                    issue->labels = NULL;
                    issue->comments = NULL;

                    while (1) {
                        if (!yaml_parser_parse(&parser, &event)) {
                            break;
                        }

                        if (event.type == YAML_SCALAR_EVENT) {
                            char* key = event.data.scalar.value;

                            if (strcmp(key, "move") == 0) {
                                yaml_parser_parse(&parser, &event);
                                char* move = event.data.scalar.value;
                                issue->move = move;
                            }

                            if (strcmp(key, "author") == 0) {
                                yaml_parser_parse(&parser, &event);
                                char* author = event.data.scalar.value;
                                issue->author = author;
                            }
                        }

                        if (event.type == YAML_MAPPING_END_EVENT) {
                            break;
                        }
                    }

                    char** labels = malloc(sizeof(char*) * 10);
                    char** comments = malloc(sizeof(char*) * 10);

                    get_test_data(labels, comments, issue->move, issue->author, test_data->owner, i);

                    issue->labels = labels;
                    issue->comments = comments;

                    char* issue_author = issue->author;
                    char* repo_owner = test_data->owner;
                    setenv("GITHUB_REPOSITORY", repo_owner + 1, 1);

                    main_fn(issue, issue_author, repo_owner);

                    bool result = expectations_fulfilled(issue);
                    if (result) {
                        printf("\u001b[0m    \u001b[1m\u001b[32m✓\u001b[0m\u001b[37m %s by %s\u001b[0m\n", issue->move, issue->author);
                        passed++;
                    } else {
                        printf("\u001b[0m    \u001b[1m\u001b[32m✓\u001b[0m\u001b[37m %s by %s\u001b[1m →\u001b[31m %s\u001b[0m\n", issue->move, issue->author, issue->reason);
                        failed++;
                    }

                    i++;
                }

                if (event.type == YAML_SEQUENCE_END_EVENT) {
                    break;
                }
            }
        }

        if (event.type == YAML_STREAM_END_EVENT) {
            break;
        }
    }

    yaml_parser_delete(&parser);
    fclose(test_file);

    printf("\n\u001b[1m\u001b[33m    %d total", passed + failed);
    printf("\u001b[1m\u001b[32m   %d passed", passed);
    printf("\u001b[1m\u001b[31m   %d failed", failed);
}

void run(void (*main_fn)(Issue*, char*, char*)) {
    int passed = 0;
    int failed = 0;

    DIR* dir;
    struct dirent* entry;

    dir = opendir("tests/");
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, ".yml") != NULL) {
                char* filename = malloc(strlen(entry->d_name) + 7);
                sprintf(filename, "tests/%s", entry->d_name);

                run_test_case(filename, main_fn);

                free(filename);
            }
        }
        closedir(dir);
    }

    int total = passed + failed;

    printf("\n\u001b[1m\u001b[33m    %d total", total);
    printf("\u001b[1m\u001b[32m   %d passed", passed);
    printf("\u001b[1m\u001b[31m   %d failed", failed);
}
