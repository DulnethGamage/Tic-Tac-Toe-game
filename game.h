#ifndef GAME_H
#define GAME_H

#define MIN_SIZE 3
#define MAX_SIZE 10
#define MAX_PLAYERS 3
extern const char LOG_FILENAME[];

/* Function Prototypes */
char **createBoard(int size);
void freeBoard(char **board, int size);
void initBoard(char **board, int size);
void showBoard(char **board, int size);
int isValidMove(char **board, int size, int row, int col);
int hasPlayerWon(char **board, int size, char symbol);
int isBoardFull(char **board, int size);
void playerMove(char **board, int size, char symbol, int playerNum);
void computerMove(char **board, int size, char symbol);
void logBoard(char **board, int size, const char *extraInfo);

#endif
