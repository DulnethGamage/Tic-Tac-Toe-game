#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

int main(void) {
    srand((unsigned int)time(NULL));

    int boardSize, gameMode, totalPlayers;

    printf("Enter board size (%d-%d): ", MIN_SIZE, MAX_SIZE);
    scanf("%d", &boardSize);
    if (boardSize < MIN_SIZE || boardSize > MAX_SIZE) {
        printf("Invalid board size!\n");
        return 1;
    }

    printf("\nChoose Game Mode:\n");
    printf("1. User vs User\n");
    printf("2. User vs Computer\n");
    printf("3. 3 Players (mix human/computer)\n");
    printf("Choice: ");
    scanf("%d", &gameMode);

    if (gameMode == 1 || gameMode == 2) totalPlayers = 2;
    else if (gameMode == 3) totalPlayers = 3;
    else { printf("Invalid mode!\n"); return 1; }

    char **board = createBoard(boardSize);
    if (!board) { printf("Memory error!\n"); return 1; }
    initBoard(board, boardSize);

    char symbols[MAX_PLAYERS] = {'X', 'O', 'Z'};
    int playerType[MAX_PLAYERS] = {0, 0, 0};

    if (gameMode == 1) {
        playerType[0] = 0; playerType[1] = 0;
    } else if (gameMode == 2) {
        playerType[0] = 0; playerType[1] = 1;
    } else {
        printf("\nConfigure 3 players (0 = Human, 1 = Computer):\n");
        for (int i = 0; i < 3; i++) {
            printf("Player %d (%c): ", i + 1, symbols[i]);
            scanf("%d", &playerType[i]);
        }
        if (playerType[0] && playerType[1] && playerType[2]) {
            printf("At least one player must be human. Player 1 set to human.\n");
            playerType[0] = 0;
        }
    }

    /* Reset log file */
    FILE *f = fopen(LOG_FILENAME, "w");
    if (f) {
        fprintf(f, "Tic Tac Toe Log (Board %dx%d)\n====================\n", boardSize, boardSize);
        fclose(f);
    }

    int currentPlayer = 0;
    int gameOver = 0;

    while (!gameOver) {
        showBoard(board, boardSize);
        printf("Player %d (%c) turn\n", currentPlayer + 1, symbols[currentPlayer]);

        if (playerType[currentPlayer] == 0)
            playerMove(board, boardSize, symbols[currentPlayer], currentPlayer + 1);
        else
            computerMove(board, boardSize, symbols[currentPlayer]);

        logBoard(board, boardSize, "Move recorded");

        if (hasPlayerWon(board, boardSize, symbols[currentPlayer])) {
            showBoard(board, boardSize);
            printf("Player %d (%c) wins!\n", currentPlayer + 1, symbols[currentPlayer]);
            logBoard(board, boardSize, "Game ended - WINNER FOUND");
            gameOver = 1;
        } else if (isBoardFull(board, boardSize)) {
            showBoard(board, boardSize);
            printf("It's a draw!\n");
            logBoard(board, boardSize, "Game ended - DRAW");
            gameOver = 1;
        } else {
            currentPlayer = (currentPlayer + 1) % totalPlayers;
        }
    }

    freeBoard(board, boardSize);
    printf("Game log saved to \"%s\".\n", LOG_FILENAME);
    return 0;
}
