#include <stdio.h>

/* Function to initialize the chessboard */
void initializeBoard(char board[8][8]) {
    // Initialize the white pieces
    board[0][0] = 'R';
    board[0][1] = 'N';
    board[0][2] = 'B';
    board[0][3] = 'Q';
    board[0][4] = 'K';
    board[0][5] = 'B';
    board[0][6] = 'N';
    board[0][7] = 'R';

    for (int i = 0; i < 8; i++) {
        board[1][i] = 'P';
    }

    /* Initialize the empty spaces */
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = '-';
        }
    }

    /* Initialize the black pieces */
    for (int i = 0; i < 8; i++) {
        board[6][i] = 'p';
    }

    board[7][0] = 'r';
    board[7][1] = 'n';
    board[7][2] = 'b';
    board[7][3] = 'q';
    board[7][4] = 'k';
    board[7][5] = 'b';
    board[7][6] = 'n';
    board[7][7] = 'r';
}

/* Function to print the chessboard */
void printBoard(char board[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

int main() {
    char board[8][8];

    /* Initialize the chessboard */
    initializeBoard(board);

    /* Print the chessboard */
    printBoard(board);

    return 0;
}
