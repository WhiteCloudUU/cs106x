/**
 * File: life.cpp
 * --------------
 * Implements the Game of Life.
 */

#include <iostream>  // for cout
using namespace std;

#include "console.h" // required of all files that contain the main function
#include "simpio.h"  // for getLine
#include "gevents.h" // for mouse event detection
#include "strlib.h"
#include "filelib.h" // for listDirectory()
#include "gtimer.h"


#include "life-constants.h"  // for kMaxAge
#include "life-graphics.h"   // for class LifeDisplay

const string kFolderPath = "./files/"; // The built files exist in the same dir of provided grid files

/**
 * Function: welcome
 * -----------------
 * Introduces the user to the Game of Life and its rules.
 */
static void welcome() {
    cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
    cout << "Cells live and die by the following rules:" << endl << endl;
    cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
    cout << "\tLocations with 2 neighbors remain stable" << endl;
    cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
    cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl << endl;
    cout << "In the animation, new cells are dark and fade to gray as they age." << endl << endl;
    getLine("Hit [enter] to continue....   ");
}

Grid<int> InitializeBoard();
Grid<int> GenerateRandomBoard();
Grid<int> GenerateBoardFromFile(string file_path);

static void RunAnimation(LifeDisplay& display, Grid<int>& board, int ms);
void AdvanceBoard(LifeDisplay& display, Grid<int>& board);
Grid<int> ComputeNextGeneration(const Grid<int>& board);
int CountNeighbors(const Grid<int>& board, int row_idx, int col_idx);

/**
 * Function: main
 * --------------
 * Provides the entry point of the entire program.
 */
int main() {
    LifeDisplay display;
    display.setTitle("Game of Life");
    welcome();
    Grid<int> board = InitializeBoard();
    RunAnimation(display, board, 1500);
    return 0;
}

/**
 * Function: InitializeBoard()
 * --------------
 * Ask the user to choose how to initialize the grid
 */
Grid<int> InitializeBoard() {
    cout << endl;
    Grid<int> board;
//    while(true) {
//        int starting_mode = getInteger("Please pick a starting mode to generate an initial setup"
//                                  " (Mode 1: Randomly generated; Mode 2: Generated from file.): ");
//        if (starting_mode == 1) {
//            cout << "You have chosen to generate a random setup..." << endl << endl;

//            board = GenerateRandomBoard();
//            break;
//        } else if (starting_mode == 2) {
//            cout << "You have chosen to generate from a file..." << endl;
//            Vector<string> files = listDirectory(kFolderPath);
//            for (int i = 0; i < files.size(); i++) {
//                cout << i << ": " << files.get(i) << endl;
//            }
//            int selected_file_idx = getInteger("Please select a file ID from above list: ");
//            board = GenerateBoardFromFile(files[selected_file_idx]);

//            break;
//        } else {
//            cout << "Not a valid mode..." << endl;
//        }
//    }
    Vector<string> files = listDirectory(kFolderPath);
    board = GenerateBoardFromFile(files[9]); // Simple Bar

    cout << "Selected board has been generated." << endl << endl;

    getLine("Hit [enter] to continue....   ");
    return board;
}

/**
 * Function: GenerateRandomBoard
 * -----------------------------
 * Generate a board with random rows and columns and
 * each grid has the same chance to be occupied or not
 * with a value (ie. age) between 1 and kMaxAge
 */
Grid<int> GenerateRandomBoard() {
    int row = randomInteger(40, 60);
    int col = randomInteger(40, 60);

    Grid<int> board(row, col);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (randomChance(0.5)) {
                board[i][j] = randomInteger(1, kMaxAge);
            }
        }
    }

    return board;
}

/**
 * Function: GenerateBoardFromFile
 * -----------------------------
 * Generate a board with random rows and columns and
 * each grid has the same chance to be occupied or not
 * with a value (ie. age) between 1 and kMaxAge
 */
Grid<int> GenerateBoardFromFile(string file_path) {
    ifstream file_stream;
//    cout << "File path: " << kFolderPath + file_path << endl;

    if (!openFile(file_stream, kFolderPath + file_path)) {
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }

    string line;
    while (getline(file_stream, line)) {

        if (stringIsInteger(line)) {
            break;
        }

    }

    int n_row = stringToInteger(line);
    getline(file_stream, line);
    int n_col = stringToInteger(line);

    Grid<int> board(n_row, n_col);

    for (int i = 0; i < n_row; i++) {
        getline(file_stream, line);
        for (int j = 0; j < n_row; j++) {
            if (line[j] != '-') {
                board[i][j] = 1;
            }
        }

    }
    return board;
}

/**
 * Function: RunAnimation
 * -----------------------------
 * Run non-manual mode with 3 difference speed dependent on the 3rd argument 'ms'
 * with mouse click to quit
 */
static void RunAnimation(LifeDisplay& display, Grid<int>& board, int ms) {
    GTimer timer(ms);
    timer.start();
    while (true) {
        GEvent event = waitForEvent(TIMER_EVENT + MOUSE_EVENT);
        if (event.getEventClass() == TIMER_EVENT) {
            AdvanceBoard(display, board);
        } else if (event.getEventType() == MOUSE_PRESSED) {
            break;
        }
    }
    timer.stop();
}

/**
 * Function: AdvanceBoard
 * -----------------------------
 * Display the newly generated board
 */

void AdvanceBoard(LifeDisplay& display, Grid<int>& board) {
    int n_rows = board.numRows();
    int n_cols = board.numCols();

    Grid<int> next_board = ComputeNextGeneration(board);
    if (board != next_board) {
        board = next_board;
    } else {
        // Stable
        getLine("Colony has been stable... Hit [enter] to quit...   ");
        exit(0);
    }

    display.setDimensions(n_rows, n_cols);
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {
            display.drawCellAt(i, j, board[i][j]);
        }
    }
    display.repaint();
}

/**
 * Function: ComputeNextGeneration
 * -----------------------------
 * Generate the board of next generation based on the specified rules
 */
Grid<int> ComputeNextGeneration(const Grid<int>& board) {
    int n_rows = board.numRows();
    int n_cols = board.numCols();

    Grid<int> new_board(n_rows, n_cols);
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {

            int n_neighbors = CountNeighbors(board, i, j);

            if (n_neighbors <= 1) {
                // Rule 1: Dies of lonliness
//                new_board[i][j] = 0;
                continue;
            } else if (n_neighbors == 2) {
                // Rule 2: Stable
                new_board[i][j] = board[i][j] == 0 ? 0 : board[i][j] + 1;
            } else if (n_neighbors == 3 ) {
                // Rule 3: New cell is born or old cell remains
                new_board[i][j] = board[i][j] == kMaxAge ? kMaxAge : board[i][j] + 1;
            } else {
                // Rule 4: Dies of overcrowding
//                new_board[i][j] = 0;
                continue;
            }
        }
    }

    return new_board;
}

/**
 * Function: CountNeighbors
 * -----------------------------
 * Count neighbors and apply a corresponding rule
 */
int CountNeighbors(const Grid<int>& board, int row_idx, int col_idx) {
    int n_rows = board.numRows();
    int n_cols = board.numCols();

    int n_neighbors = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int nei_row_idx = row_idx + i;
            int nei_col_idx = col_idx + j;

            if (i == 0 && j == 0) { // Do not need to check current cell
                continue;
            } else if (nei_row_idx < 0 || nei_row_idx > n_rows-1 ||
                    nei_col_idx < 0 || nei_col_idx > n_cols-1) { // Check if out of bound
                continue;
            } else if (board[nei_row_idx][nei_col_idx] > 0) {
                n_neighbors += 1;
            }
        }
    }

    return n_neighbors;
}

