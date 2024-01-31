#include "chess.h"

typedef struct {
    char title[100];
    bool closed;
    char expected_labels[10][100];
    int expected_labels_count;
    char expected_comments[10][100];
    int expected_comments_count;
    char unexpected_labels[10][100];
    int unexpected_labels_count;
    char unexpected_comments[10][100];
    int unexpected_comments_count;
} Issue;

void create_comment(Issue *issue, char *text) {
    if (issue->expected_comments_count == 0) {
        strcpy(issue->unexpected_comments[issue->unexpected_comments_count], text);
        issue->unexpected_comments_count++;
    } else if (strcmp(issue->expected_comments[0], text) != 0) {
        strcpy(issue->unexpected_comments[issue->unexpected_comments_count], text);
        issue->unexpected_comments_count++;
    } else {
        for (int i = 0; i < issue->expected_comments_count - 1; i++) {
            strcpy(issue->expected_comments[i], issue->expected_comments[i + 1]);
        }
        issue->expected_comments_count--;
    }
}

void edit(Issue *issue, char *state, char labels[][100], int labels_count) {
    if (strcmp(state, "closed") == 0) {
        issue->closed = true;
    }

    for (int i = 0; i < labels_count; i++) {
        bool found = false;
        for (int j = 0; j < issue->expected_labels_count; j++) {
            if (strcmp(issue->expected_labels[j], labels[i]) == 0) {
                found = true;
                for (int k = j; k < issue->expected_labels_count - 1; k++) {
                    strcpy(issue->expected_labels[k], issue->expected_labels[k + 1]);
                }
                issue->expected_labels_count--;
                break;
            }
        }
        if (!found) {
            strcpy(issue->unexpected_labels[issue->unexpected_labels_count], labels[i]);
            issue->unexpected_labels_count++;
        }
    }
}

void add_to_labels(Issue *issue, char *label) {
    bool found = false;
    for (int i = 0; i < issue->expected_labels_count; i++) {
        if (strcmp(issue->expected_labels[i], label) == 0) {
            found = true;
            for (int j = i; j < issue->expected_labels_count - 1; j++) {
                strcpy(issue->expected_labels[j], issue->expected_labels[j + 1]);
            }
            issue->expected_labels_count--;
            break;
        }
    }
    if (!found) {
        strcpy(issue->unexpected_labels[issue->unexpected_labels_count], label);
        issue->unexpected_labels_count++;
    }
}

void expect_labels(Issue *issue, char labels[][100], int labels_count) {
    for (int i = 0; i < labels_count; i++) {
        strcpy(issue->expected_labels[i], labels[i]);
    }
    issue->expected_labels_count = labels_count;
}

void expect_comments(Issue *issue, char regex_list[][100], int regex_list_count) {
    for (int i = 0; i < regex_list_count; i++) {
        strcpy(issue->expected_comments[i], regex_list[i]);
    }
    issue->expected_comments_count = regex_list_count;
}

bool expectations_fulfilled(Issue *issue, char *error_message) {
    if (issue->expected_labels_count != 0) {
        sprintf(error_message, "Missing expected labels: ");
        for (int i = 0; i < issue->expected_labels_count; i++) {
            strcat(error_message, issue->expected_labels[i]);
            if (i != issue->expected_labels_count - 1) {
                strcat(error_message, ", ");
            }
        }
        return false;
    }
    if (issue->expected_comments_count != 0) {
        sprintf(error_message, "Missing expected comments: ");
        for (int i = 0; i < issue->expected_comments_count; i++) {
            strcat(error_message, issue->expected_comments[i]);
            if (i != issue->expected_comments_count - 1) {
                strcat(error_message, ", ");
            }
        }
        return false;
    }
    if (issue->unexpected_labels_count != 0) {
        sprintf(error_message, "Unexpected labels: ");
        for (int i = 0; i < issue->unexpected_labels_count; i++) {
            strcat(error_message, issue->unexpected_labels[i]);
            if (i != issue->unexpected_labels_count - 1) {
                strcat(error_message, ", ");
            }
        }
        return false;
    }
    if (issue->unexpected_comments_count != 0) {
        sprintf(error_message, "Unexpected comments: ");
        for (int i = 0; i < issue->unexpected_comments_count; i++) {
            strcat(error_message, issue->unexpected_comments[i]);
            if (i != issue->unexpected_comments_count - 1) {
                strcat(error_message, ", ");
            }
        }
        return false;
    }
    if (!issue->closed) {
        strcpy(error_message, "Issue not closed");
        return false;
    }

    return true;
}
