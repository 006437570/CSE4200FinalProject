#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 400;
const int BOARD_SIZE = 10;
const int CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE;


// Variables for color themes
float hiddenGrid[3] = {0.5, 0.5, 0.5}; //Grids that haven't been revealed yet (default gray)
float markedGrid[3] = {1.0, 0.0, 0.0}; //Grid that has been flagged by player (default red)
float bombGrid[3] = {0.0, 0.0, 0.0}; //Grid that has a bomb on it (default black)
float revealedGrid[3] = {0.8, 0.8, 0.8}; //Grid that has been revealed by the player (default light gray)

// The 3 different states that any grid cell can be in
enum class CellState {
    Hidden,
    Revealed,
    Marked
};

// A cell can either be empty or have a mine
enum class CellContent {
    Empty,
    Mine
};

struct Cell {
    CellState state = CellState::Hidden;
    CellContent content = CellContent::Empty;
    int adjacentMines = 0;
};

vector<vector<Cell>> board(BOARD_SIZE, vector<Cell>(BOARD_SIZE));

int minesRemaining = 10;

void generateMines(int numMines) {
    srand(time(nullptr));
    int minesPlaced = 0;
    while (minesPlaced < numMines) {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;
        if (board[y][x].content != CellContent::Mine) {
            board[y][x].content = CellContent::Mine;
            minesPlaced++;
        }
    }
}

void countAdjacentMines() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x].content == CellContent::Mine) continue;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        board[ny][nx].content == CellContent::Mine) {
                        board[y][x].adjacentMines++;
                    }
                }
            }
        }
    }
}

void revealEmptyCells(int x, int y) {
    if (x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE) return;
    if (board[y][x].state == CellState::Revealed || board[y][x].content == CellContent::Mine) return;

    board[y][x].state = CellState::Revealed;

    if (board[y][x].adjacentMines > 0) return;

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
    glColor3f(0.0, 0.0, 0.0); // Set line color to black
    glLineWidth(1.0); // Set line width to 1 pixel
    glBegin(GL_LINES);
    for (int i = 1; i < BOARD_SIZE; ++i) {
        glVertex2f(i * CELL_SIZE, 0);
        glVertex2f(i * CELL_SIZE, WINDOW_HEIGHT);
        glVertex2f(0, i * CELL_SIZE);
        glVertex2f(WINDOW_WIDTH, i * CELL_SIZE);
    }
    glEnd();

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x].state == CellState::Hidden) {
                //glColor3f(0.5, 0.5, 0.5); // Gray
                glColor3f(hiddenGrid[0], hiddenGrid[1], hiddenGrid[2]);
            } else if (board[y][x].state == CellState::Marked) {
                //glColor3f(1.0, 0.0, 0.0); // Red
                glColor3f(markedGrid[0],markedGrid[1],markedGrid[2]);
            } else {
                if (board[y][x].content == CellContent::Mine) {
                    //glColor3f(0.0, 0.0, 0.0); // Black
                    glColor3f(bombGrid[0],bombGrid[1],bombGrid[2]);
                } else {
                    //glColor3f(0.8, 0.8, 0.8); // Light gray
                    glColor3f(revealedGrid[0],revealedGrid[1],revealedGrid[2]);
                }
            }
            drawCell(x * CELL_SIZE, y * CELL_SIZE);

            if (board[y][x].state == CellState::Revealed && board[y][x].content != CellContent::Mine &&
                board[y][x].adjacentMines > 0) {
                glColor3f(0.0, 0.0, 0.0); // Black
                glRasterPos2i(x * CELL_SIZE + CELL_SIZE / 3, y * CELL_SIZE + CELL_SIZE / 1.5);
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + board[y][x].adjacentMines);
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBoard();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int cellX = x / CELL_SIZE;
        int cellY = y / CELL_SIZE;
        if (board[cellY][cellX].state == CellState::Hidden) {
            if (board[cellY][cellX].content == CellContent::Mine) {
                cout << "Game over!" << endl;
                exit(0);
            } else {
                revealEmptyCells(cellX, cellY);
            }
        }
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        int cellX = x / CELL_SIZE;
        int cellY = y / CELL_SIZE;
        if (board[cellY][cellX].state == CellState::Hidden) {
            if (minesRemaining > 0) {
                board[cellY][cellX].state = CellState::Marked;
                minesRemaining--;
            }
        } else if (board[cellY][cellX].state == CellState::Marked) {
            board[cellY][cellX].state = CellState::Hidden;
            minesRemaining++;
        }
    }
    glutPostRedisplay();
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