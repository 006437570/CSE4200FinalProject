#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 750;
const int GUI_HEIGHT = WINDOW_HEIGHT - WINDOW_WIDTH; //set height that will be for the GUI
const int BOARD_SIZE = 10;
const int CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE;

// Variables for color themes
float hiddenGrid[3] = {0.5, 0.5, 0.5}; //Grids that haven't been revealed yet (default gray)
float markedGrid[3] = {1.0, 0.0, 0.0}; //Grid that has been flagged by player (default red)
float bombGrid[3] = {0.0, 0.0, 0.0}; //Grid that has a bomb on it (default black)
float revealedGrid[3] = {0.8, 0.8, 0.8}; //Grid that has been revealed by the player (default light gray)
float gridBorder[3] = {1.0, 1.0, 1.0}; //Outline color of all the grids (default white)

// Cell states
const int HIDDEN = 0;
const int REVEALED = 1;
const int MARKED = 2;

// Cell contents
const int EMPTY = 0;
const int MINE = 1;

vector<vector<int>> cellState(BOARD_SIZE, vector<int>(BOARD_SIZE, HIDDEN));
vector<vector<int>> cellContent(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
vector<vector<int>> adjacentMines(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));

bool gameOverFlag = false;

int minesRemaining = 10;
time_t startTime;

void generateMines(int numMines) {
    srand(time(nullptr));
    int minesPlaced = 0;
    while (minesPlaced < numMines) {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;
        if (cellContent[y][x] != MINE) {
            cellContent[y][x] = MINE;
            minesPlaced++;
        }
    }
}

void countAdjacentMines() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (cellContent[y][x] == MINE) continue;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        cellContent[ny][nx] == MINE) {
                        adjacentMines[y][x]++;
                    }
                }
            }
        }
    }
}

void revealEmptyCells(int x, int y) {
    if (x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE) return;
    if (cellState[y][x] == REVEALED || cellContent[y][x] == MINE) return;

    cellState[y][x] = REVEALED;

    if (adjacentMines[y][x] > 0) return;

    revealEmptyCells(x - 1, y - 1);
    revealEmptyCells(x, y - 1);
    revealEmptyCells(x + 1, y - 1);
    revealEmptyCells(x - 1, y);
    revealEmptyCells(x + 1, y);
    revealEmptyCells(x - 1, y + 1);
    revealEmptyCells(x, y + 1);
    revealEmptyCells(x + 1, y + 1);
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    generateMines(10);
    countAdjacentMines();
    startTime = time(nullptr);
}

void drawCell(int x, int y) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + CELL_SIZE, y);
    glVertex2f(x + CELL_SIZE, y + CELL_SIZE);
    glVertex2f(x, y + CELL_SIZE);
    glEnd();
}

void drawBoard() {
    // Draw the grid lines
    glColor3f(0.0, 0.0, 0.0); // Set line color to black
    glLineWidth(2.0); // Set line width to 2 pixels

    glBegin(GL_LINES);
    for (int i = 0; i < (BOARD_SIZE + 1); ++i) {
        // Vertical Lines
        glVertex2f(i * CELL_SIZE, GUI_HEIGHT);
        glVertex2f(i * CELL_SIZE, WINDOW_HEIGHT);
        // Horizontal Lines
        glVertex2f(0, GUI_HEIGHT + (i * CELL_SIZE));
        glVertex2f(WINDOW_WIDTH, GUI_HEIGHT + (i * CELL_SIZE));
    }
    glEnd();

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (cellState[y][x] == HIDDEN) {
                glColor3fv(hiddenGrid); // Gray
            } else if (cellState[y][x] == MARKED) {
                glColor3fv(markedGrid); // Red
            } else {
                if (cellContent[y][x] == MINE) {
                    glColor3fv(bombGrid); // Black
                } else {
                    glColor3fv(revealedGrid); // Light gray
                }
            }
            drawCell(x * CELL_SIZE, GUI_HEIGHT + y * CELL_SIZE);

            if (cellState[y][x] == REVEALED && cellContent[y][x] != MINE &&
                adjacentMines[y][x] > 0) {
                glColor3f(0.0, 0.0, 0.0); // Black
                glRasterPos2i(x * CELL_SIZE + CELL_SIZE / 3, GUI_HEIGHT + y * CELL_SIZE + CELL_SIZE / 1.5);
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + adjacentMines[y][x]);
            }
        }
    }
    //Draw the grid lines
    glColor3f(gridBorder[0], gridBorder[1], gridBorder[2]);
    glLineWidth(2.0); // Set line width to 1 pixel

    glBegin(GL_LINES);
        for (int i = 0; i < (BOARD_SIZE + 1); ++i) {
            //Vertical Lines
            glVertex2f(i * CELL_SIZE, GUI_HEIGHT);
            glVertex2f(i * CELL_SIZE, WINDOW_HEIGHT);
            //Horizontal Lines
            glVertex2f(0, GUI_HEIGHT + (i * CELL_SIZE) );
            glVertex2f(WINDOW_WIDTH, GUI_HEIGHT + (i * CELL_SIZE) );
        }
    glEnd();
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBoard();

    // Display game timer or "Game Over" message
    string timerDisplay;
    if (gameOverFlag) {
        timerDisplay = "Game Over!";
        glColor3f(1.0, 0.0, 0.0); // Red color for game over message
    } else {
        // Display game timer if the timer is running
        time_t currentTime = time(nullptr);
        int elapsedTime = difftime(currentTime, startTime);
        int minutes = elapsedTime / 60;
        int seconds = elapsedTime % 60;
        timerDisplay = "Time: " + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
        glColor3f(0.0, 0.0, 0.0);
    }
    glRasterPos2i(10, 30); // Center the text horizontally
    for (char c : timerDisplay) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    // Display reset button
    string resetDisplay = "Reset";
    glRasterPos2i(WINDOW_WIDTH / 2 - 20, 30);
    for (char c : resetDisplay) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    // Display flags remaining
    string flagDisplay = "Flags: " + to_string(minesRemaining);
    glRasterPos2i(WINDOW_WIDTH - 120, 30);
    for (char c : flagDisplay) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glutSwapBuffers();

    // Trigger window refresh every second
    glutPostRedisplay();
}

void resetGame() {
    // Reset game variables
    minesRemaining = 10;
    startTime = time(nullptr);
    cellState.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, HIDDEN));
    cellContent.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
    adjacentMines.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
    generateMines(10);
    countAdjacentMines();
    gameOverFlag = false; // Reset game over flag
}

void gameOver() {
    time_t currentTime = time(nullptr);
    int elapsedTime = difftime(currentTime, startTime);
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    cout << "Game Over!" << endl;
    cout << "Time elapsed: " << minutes << " minutes and " << seconds << " seconds." << endl;
    gameOverFlag = true; // Set game over flag
}

void mouse(int button, int state, int x, int y) {

    if (gameOverFlag) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (x >= WINDOW_WIDTH / 2 - 20 && x <= WINDOW_WIDTH / 2 + 20 && y >= 10 && y <= 30) {
                resetGame(); // Reset game if reset button is clicked
            }
        }
        return;
    }

    if (x < 0 || y < GUI_HEIGHT || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) return; // Click outside the game board area
    int cellX = x / CELL_SIZE;
    int cellY = (y - GUI_HEIGHT) / CELL_SIZE;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (cellState[cellY][cellX] == HIDDEN) {
            if (cellContent[cellY][cellX] == MINE) {
                gameOver();
            } else {
                revealEmptyCells(cellX, cellY);
            }
        }
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (cellState[cellY][cellX] == HIDDEN) {
            if (minesRemaining > 0) {
                cellState[cellY][cellX] = MARKED;
                minesRemaining--;
            }
        } else if (cellState[cellY][cellX] == MARKED) {
            cellState[cellY][cellX] = HIDDEN;
            minesRemaining++;
        }
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, w, h, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// For keyboard functions
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        //closes program
        case 27: //escape key
            exit(-1);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Open Landmine");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}