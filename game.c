#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*
 * Tic-Tac-Toe (N x N) with dynamic allocation, logging, and 2/3 player support.
 * - Board size: 3 <= N <= 10
 * - Modes: User vs User, User vs Computer, 3 Players (mix human/computer)
 * - Computer uses simple random valid moves
 * - Logs every move and board state to "tic_tac_toe_log.txt"
 */

#define MIN_SIZE 3
#define MAX_SIZE 10
#define MAX_PLAYERS 3
const char LOG_FILENAME[] = "tic_tac_toe_log.txt";

/* ---------------- Function Prototypes ---------------- */
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

/* ---------------- Main ---------------- */
int main(void) {
    srand((unsigned int)time(NULL)); // Seed the random number generator
    
    int boardSize;
    int gameMode;
    int totalPlayers;
    
    // Get board size from user
    printf("Enter board size (%d - %d): ", MIN_SIZE, MAX_SIZE);
    if (scanf("%d", &boardSize) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }
    
    // Validate board size - learned this the hard way after crashes!
    if (boardSize < MIN_SIZE || boardSize > MAX_SIZE) {
        printf("Invalid board size! Must be between %d and %d.\n", MIN_SIZE, MAX_SIZE);
        return 1;
    }
    
    // Let user select what kind of game they want
    printf("\nChoose Game Mode:\n");
    printf("1. User vs User\n");
    printf("2. User vs Computer\n");
    printf("3. 3 Players (mix human/computer)\n");
    printf("Choice: ");
    if (scanf("%d", &gameMode) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }
    
    // Set total players based on mode
    if (gameMode == 1 || gameMode == 2) {
        totalPlayers = 2;
    } else if (gameMode == 3) {
        totalPlayers = 3;
    } else {
        printf("Invalid choice!\n");
        return 1;
    }
    
    // Create the game board dynamically
    char **board = createBoard(boardSize);
    if (board == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return 1;
    }
    initBoard(board, boardSize);
    
    // Setup player symbols and whether they're human or computer
    char symbols[MAX_PLAYERS] = {'X', 'O', 'Z'}; // Z for third player - kinda weird but works
    int playerType[MAX_PLAYERS] = {0, 0, 0}; // 0 = human, 1 = computer
    
    // Configure players based on game mode
    if (gameMode == 1) {
        // Both human players
        playerType[0] = 0;
        playerType[1] = 0;
    } else if (gameMode == 2) {
        playerType[0] = 0; // user
        playerType[1] = 1; // computer
    } else { // gameMode == 3
        printf("\nConfigure 3 players (0 = Human, 1 = Computer):\n");
        for (int i = 0; i < 3; i++) {
            int playerChoice;
            printf("Player %d (%c): ", i + 1, symbols[i]);
            if (scanf("%d", &playerChoice) != 1) {
                printf("Invalid input. Assuming human.\n");
                playerChoice = 0;
            }
            playerType[i] = (playerChoice == 1) ? 1 : 0;
        }
        // Make sure at least one player is human - otherwise what's the point?
        if (playerType[0] && playerType[1] && playerType[2]) {
            printf("At least one player must be human. Setting Player 1 to human.\n");
            playerType[0] = 0;
        }
    }
    
    // Initialize log file - clear any previous content
    FILE *logFile = fopen(LOG_FILENAME, "w");
    if (logFile) {
        fprintf(logFile, "Tic-Tac-Toe Log (board %d x %d)\n", boardSize, boardSize);
        fprintf(logFile, "=================================\n");
        fclose(logFile);
    }
    
    // Main game loop - this is where the magic happens
    int currentPlayer = 0;
    int gameOver = 0;
    
    while (!gameOver) {
        showBoard(board, boardSize);
        printf("Player %d (%c) turn\n", currentPlayer + 1, symbols[currentPlayer]);
        
        if (playerType[currentPlayer] == 0) {
            // Human player's turn
            playerMove(board, boardSize, symbols[currentPlayer], currentPlayer + 1);
            char logInfo[64];
            snprintf(logInfo, sizeof(logInfo), "Player %d (%c) moved (human)", currentPlayer + 1, symbols[currentPlayer]);
            logBoard(board, boardSize, logInfo);
        } else {
            // Computer player's turn
            computerMove(board, boardSize, symbols[currentPlayer]);
            char logInfo[64];
            snprintf(logInfo, sizeof(logInfo), "Player %d (%c) moved (computer)", currentPlayer + 1, symbols[currentPlayer]);
            logBoard(board, boardSize, logInfo);
        }
        
        // Check if current player won
        if (hasPlayerWon(board, boardSize, symbols[currentPlayer])) {
            showBoard(board, boardSize);
            printf("Player %d (%c) wins!\n", currentPlayer + 1, symbols[currentPlayer]);
            char winInfo[64];
            snprintf(winInfo, sizeof(winInfo), "Player %d (%c) WINS", currentPlayer + 1, symbols[currentPlayer]);
            logBoard(board, boardSize, winInfo);
            gameOver = 1;
        } else if (isBoardFull(board, boardSize)) {
            // Check for draw
            showBoard(board, boardSize);
            printf("It's a draw!\n");
            logBoard(board, boardSize, "Game ended in a DRAW");
            gameOver = 1;
        } else {
            // Move to next player
            currentPlayer = (currentPlayer + 1) % totalPlayers;
        }
    }
    
    // Clean up memory - important!
    freeBoard(board, boardSize);
    
    printf("Game log saved to \"%s\"\n", LOG_FILENAME);
    return 0;
}

/* ---------------- Function Definitions ---------------- */

// Create a dynamic 2D array for the board
char **createBoard(int size) {
    char **gameBoard = (char **)malloc(size * sizeof(char *));
    if (!gameBoard) return NULL;
    
    for (int i = 0; i < size; i++) {
        gameBoard[i] = (char *)malloc(size * sizeof(char));
        if (!gameBoard[i]) {
            // Cleanup if allocation fails partway through
            for (int j = 0; j < i; j++) {
                free(gameBoard[j]);
            }
            free(gameBoard);
            return NULL;
        }
    }
    return gameBoard;
}

// Free the dynamically allocated board
void freeBoard(char **board, int size) {
    if (!board) return; // Safety check
    
    for (int i = 0; i < size; i++) {
        free(board[i]);
    }
    free(board);
}

// Initialize all board positions to empty ('-')
void initBoard(char **board, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            board[i][j] = '-'; // Using '-' to represent empty spaces
        }
    }
}

// Display the current board state with nice formatting
void showBoard(char **board, int size) {
    printf("\n   "); // Extra spacing for alignment
    
    // Print column numbers
    for (int col = 0; col < size; col++) {
        printf("%2d ", col + 1);
    }
    printf("\n");
    
    // Print each row with row number
    for (int row = 0; row < size; row++) {
        printf("%2d ", row + 1);
        for (int col = 0; col < size; col++) {
            printf(" %c ", board[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}

// Check if a move is valid (within bounds and cell is empty)
int isValidMove(char **board, int size, int row, int col) {
    // Check bounds first
    if (row >= 0 && row < size && col >= 0 && col < size) {
        // Then check if cell is empty
        if (board[row][col] == '-') {
            return 1; // Valid move
        }
    }
    return 0; // Invalid move
}

// Check if the given player has won - this was tricky to get right!
int hasPlayerWon(char **board, int size, char symbol) {
    int i, j;
    
    // Check all rows
    for (i = 0; i < size; i++) {
        int rowComplete = 1;
        for (j = 0; j < size; j++) {
            if (board[i][j] != symbol) {
                rowComplete = 0;
                break;
            }
        }
        if (rowComplete) return 1;
    }
    
    // Check all columns  
    for (j = 0; j < size; j++) {
        int colComplete = 1;
        for (i = 0; i < size; i++) {
            if (board[i][j] != symbol) {
                colComplete = 0;
                break;
            }
        }
        if (colComplete) return 1;
    }
    
    // Check main diagonal (top-left to bottom-right)
    int diagComplete = 1;
    for (i = 0; i < size; i++) {
        if (board[i][i] != symbol) {
            diagComplete = 0;
            break;
        }
    }
    if (diagComplete) return 1;
    
    // Check anti-diagonal (top-right to bottom-left)
    diagComplete = 1; // Reuse variable
    for (i = 0; i < size; i++) {
        if (board[i][size - 1 - i] != symbol) {
            diagComplete = 0;
            break;
        }
    }
    if (diagComplete) return 1;
    
    return 0; // No win found
}

// Check if board is completely filled
int isBoardFull(char **board, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (board[i][j] == '-') {
                return 0; // Found empty space
            }
        }
    }
    return 1; // Board is full
}

// Handle human player input - keeps asking until valid move
void playerMove(char **board, int size, char symbol, int playerNum) {
    int row, col;
    
    while (1) { // Keep looping until valid move
        printf("Player %d enter your move (row col): ", playerNum);
        
        if (scanf("%d %d", &row, &col) != 2) {
            // Clear the input buffer if invalid input
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
                // Just clearing...
            }
            printf("Invalid input. Please enter two integers.\n");
            continue;
        }
        
        // Convert to 0-based indexing
        row--;
        col--;
        
        if (isValidMove(board, size, row, col)) {
            board[row][col] = symbol;
            break; // Valid move made, exit loop
        } else {
            printf("Invalid move! Cell is occupied or out of range. Try again.\n");
        }
    }
}

// Computer makes a random valid move - not very smart but works
void computerMove(char **board, int size, char symbol) {
    int row, col;
    
    // Keep trying random positions until we find a valid one
    // Note: This could be infinite loop if board is full, but we check that elsewhere
    do {
        row = rand() % size;
        col = rand() % size;
    } while (!isValidMove(board, size, row, col));
    
    board[row][col] = symbol;
    printf("Computer placed %c at (%d,%d)\n", symbol, row + 1, col + 1); // Convert back to 1-based for display
}

// Write current board state to log file
void logBoard(char **board, int size, const char *extraInfo) {
    FILE *logFile = fopen(LOG_FILENAME, "a"); // Append mode
    if (!logFile) {
        return; // Silently fail if can't open log file
    }
    
    if (extraInfo) {
        fprintf(logFile, "%s\n", extraInfo);
    }
    
    // Write board state
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(logFile, "%c ", board[i][j]);
        }
        fprintf(logFile, "\n");
    }
    fprintf(logFile, "---------------------------\n");
    
    fclose(logFile);
}
