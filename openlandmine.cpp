#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <GL/glut.h>

#include "themes.h"

using namespace std;

// Width and Height of game window
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 750;
const int GUI_HEIGHT = WINDOW_HEIGHT - WINDOW_WIDTH; // set height that will be for the GUI
int BOARD_SIZE = 10; // Initial size of the game board
int MAX_BOARD_SIZE = 20;
int MIN_BOARD_SIZE = 8;
int CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE; // Size of each cell on the game board
int MINES_COUNT = 10; // Initial number of miens on the game board

// Variables for color themes
//float hiddenGrid[3] = {0.5, 0.5, 0.5};    // Grids that haven't been revealed yet (default gray)
//float markedGrid[3] = {0.0, 1.0, 0.0};    // Grid that has been flagged by player (default green)
//float bombGrid[3] = {1.0, 0.0, 0.0};      // Grid that has a bomb on it (default red)
//float revealedGrid[3] = {0.8, 0.8, 0.8};  // Grid that has been revealed by the player (default light gray)
//float gridBorder[3] = {1.0, 1.0, 1.0};    // Outline color of all the grids (default white)
ThemeColors currentTheme = themeDefault;

// Cell states
const int HIDDEN = 0; // Cell is hidden
const int REVEALED = 1; // Cell is revealed
const int MARKED = 2; // Cell is marked by a player

// Cell contents
const int EMPTY = 0; // Cell is empty
const int MINE = 1; // Cell contains a mine

vector<vector<int>> cellState(BOARD_SIZE, vector<int>(BOARD_SIZE, HIDDEN));
vector<vector<int>> cellContent(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
vector<vector<int>> adjacentMines(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));

// Number of mines remaining
int minesRemaining = MINES_COUNT;

// Time when the game starts
time_t startTime;

// Flags indicating whether the game is over or if the win condition is met
bool gameOverFlag = false;
bool winConditionMet = false;
bool gameState = false;

void drawFlag(GLfloat x, GLfloat y)
{
    //Flagpole
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x+5, y+5);
        glVertex2f(x+5, y+CELL_SIZE-5);
        glVertex2f(x+15, y+CELL_SIZE-5);
        glVertex2f(x+15, y+5);
    glEnd();

    //Flag
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x+15, y+5);
        glVertex2f(x+15, y+20);
        glVertex2f(x+45, y+20);
        glVertex2f(x+45, y+5);
    glEnd();

    //Outline of flag
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x+15, y+5);
        glVertex2f(x+15, y+20);
        glVertex2f(x+45, y+20);
        glVertex2f(x+45, y+5);
    glEnd();
}

void drawMine(GLfloat x, GLfloat y)
{
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x+15,y+CELL_SIZE-5);
        glVertex2f(x+15,y+CELL_SIZE-10);
        glVertex2f(x+CELL_SIZE-15,y+CELL_SIZE-10);
        glVertex2f(x+CELL_SIZE-15,y+CELL_SIZE-5);
    glEnd();
}

// Function to generate mines
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

// Function to count adjacent mines
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

// Function to reveal empty cells
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

// Function to initialize the game
void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    generateMines(MINES_COUNT);
    countAdjacentMines();
    startTime = time(nullptr);
}

// Function to draw a cell
void drawCell(int x, int y) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + CELL_SIZE, y);
    glVertex2f(x + CELL_SIZE, y + CELL_SIZE);
    glVertex2f(x, y + CELL_SIZE);
    glEnd();
}

// Function to draw the game board
void drawBoard() {
    // Draw the grid lines
    glColor3f(0.0, 0.0, 0.0);   // Set line color to black
    glLineWidth(2.0);           // Set line width to 2 pixels

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
                glColor3fv(currentTheme.hiddenGrid); // Gray
            } else if (cellState[y][x] == MARKED) {
                glColor3fv(currentTheme.markedGrid); // Red
            } else {
                if (cellContent[y][x] == MINE) {
                    glColor3fv(currentTheme.bombGrid); // Black
                } else {
                    glColor3fv(currentTheme.revealedGrid); // Light gray
                }
            }
            drawCell(x * CELL_SIZE, GUI_HEIGHT + y * CELL_SIZE);

            //if cell has been marked, draw flag on it
            if (cellState[y][x] == MARKED)
            {
                drawFlag(x * CELL_SIZE, GUI_HEIGHT + y * CELL_SIZE);
            }
            
	    //if cell has a mine, then draw a mine on it
            if (cellState[y][x] == MINE && cellContent[y][x] == REVEALED)
            {
                drawMine(x * CELL_SIZE, GUI_HEIGHT + y * CELL_SIZE);
            }

            if (cellState[y][x] == REVEALED && cellContent[y][x] != MINE &&
                adjacentMines[y][x] > 0) {
	        // Adjust font size based on cell size and number of digits
                int numDigits = (adjacentMines[y][x] == 0) ? 1 : (int)log10(adjacentMines[y][x]) + 1;
                int fontSize = CELL_SIZE / (2 * numDigits); // Adjust this factor as needed
                glColor3f(0.0, 0.0, 0.0); // Black
                glRasterPos2i(x * CELL_SIZE + CELL_SIZE / 3, GUI_HEIGHT + y * CELL_SIZE + CELL_SIZE / 2 + fontSize / 2);
                // Draw number with adjusted font size
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + adjacentMines[y][x]);
	    }
        }
    }
    // Draw the grid lines
    glColor3f(currentTheme.gridBorder[0], currentTheme.gridBorder[1], currentTheme.gridBorder[2]);
    glLineWidth(2.0); // Set line width to 1 pixel

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
}

void drawThemeButton() {
    // Draw a button rectangle
    glColor3f(0.5, 0.5, 0.5); // Gray color
    glBegin(GL_QUADS);
    glVertex2i(WINDOW_WIDTH / 2 - 40, 40); // Top-left
    glVertex2i(WINDOW_WIDTH / 2 + 40, 40); // Top-right
    glVertex2i(WINDOW_WIDTH / 2 + 40, 70); // Bottom-right
    glVertex2i(WINDOW_WIDTH / 2 - 40, 70); // Bottom-left
    glEnd();

    // Draw button label
    glColor3f(1.0, 1.0, 1.0); // White color
    glRasterPos2i(WINDOW_WIDTH / 2 - 20, 60); // Position the text

    // Display "Theme" text
    string themeDisplay = "Theme";
    for (char c : themeDisplay) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

// Function to display the game
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBoard();
    drawThemeButton();

    // Check win condition
    winConditionMet = true;
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
           if (cellState[y][x] == HIDDEN && cellContent[y][x] == MINE) {
               winConditionMet = false;
               break;
           }
           if (cellState[y][x] == MARKED && cellContent[y][x] != MINE) {
               winConditionMet = false;
               break;
           }
        }
        if (!winConditionMet) {
            break;
        }
    }

    // Display game status
    string statusDisplay;
    if (gameOverFlag) {
        statusDisplay = "Game Over!";
        glColor3f(1.0, 0.0, 0.0); // Red color for game over message
    } else if (winConditionMet) {
        statusDisplay = "Winner!";
	glColor3f(0.0, 1.0, 0.0); // Green color for winner message
    } else {
        // Display game timer if the timer is running
        time_t currentTime = time(nullptr);
        int elapsedTime = difftime(currentTime, startTime);
        int minutes = elapsedTime / 60;
        int seconds = elapsedTime % 60;
        statusDisplay = "Time: " + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
        glColor3f(0.0, 0.0, 0.0);
    }
    glRasterPos2i(10, 30); // Position status display on the left-hand side
    for (char c : statusDisplay) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    // Display reset button
    string resetDisplay = "Reset";
    glRasterPos2i(WINDOW_WIDTH / 2 - 28, 30);
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

// Function to reset the game
void resetGame() {
    gameOverFlag = false;
    gameState = false;
    cellState.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, HIDDEN));
    cellContent.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
    adjacentMines.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
    minesRemaining = MINES_COUNT;
    generateMines(MINES_COUNT);
    countAdjacentMines();
    startTime = time(nullptr);
}

// Function to handle agme over condition
void gameOver() {
    time_t currentTime = time(nullptr);
    int elapsedTime = difftime(currentTime, startTime);
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    cout << "Game Over!" << endl;
    cout << "Time elapsed: " << minutes << " minutes and " << seconds << " seconds." << endl;
    gameOverFlag = true; // Set game over flag

    // Reveal all the mines
    for (int y = 0; y < BOARD_SIZE; ++y) {
       for (int x = 0; x < BOARD_SIZE; ++x) {
           if (cellContent[y][x] == MINE) {
               cellState[y][x] = REVEALED;
           }
       }
    }
}

// Function to handle keyboard events
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    //closes program
    case 27: //escape key
        exit(-1);
    //reveal all mines (developer keybind)
    case 'm':
    case 'M':
        if (!gameOverFlag) {
            for (int y = 0; y < BOARD_SIZE; ++y) {
                for (int x = 0; x < BOARD_SIZE; ++x) {
                    if (cellContent[y][x] == MINE) {
                        cellState[y][x] = REVEALED;
                    }
                }
            }
        }
        break;
    // Increase difficulty
    case '+':
	if (BOARD_SIZE < MAX_BOARD_SIZE) {
            BOARD_SIZE += 2;
	    if (MINES_COUNT > BOARD_SIZE * BOARD_SIZE) {
	        MINES_COUNT = BOARD_SIZE * BOARD_SIZE - 1;
	    } else {
	        MINES_COUNT += 5;
	    }
            CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE;
            resetGame();	
	}
        break;
    // Decrease difficulty
    case '-':
        if (BOARD_SIZE > MIN_BOARD_SIZE) {
            BOARD_SIZE -= 2;
	    if (MINES_COUNT > BOARD_SIZE * BOARD_SIZE) {
		MINES_COUNT = BOARD_SIZE * BOARD_SIZE - 1;
	    } else {
	        MINES_COUNT -= 5;
	    }
            CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE;
            resetGame();
        }
        break;
    // Decrease mine count 
    case 'u':
    case 'U':
        if (MINES_COUNT > 1) {
	    MINES_COUNT--;
	    resetGame();
	}
	break;
    // Increase mine count
    case 'i':
    case 'I':
	if (MINES_COUNT < BOARD_SIZE * BOARD_SIZE - 1) {
	    MINES_COUNT++;
	    resetGame();
	}
	break;
    }
}

// Function to reshape the window
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, w, h, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Function to cycle through themes
void cycleThemes() {
    static int themeIndex = 0;
    themeIndex = (themeIndex + 1) % 3; // Update theme index cyclically
    switch (themeIndex) {
        case 0:
            currentTheme = themeDefault;
            break;
        case 1:
            currentTheme = theme1;
            break;
        case 2:
            currentTheme = theme2;
            break;
    }
    // Redraw the board with the new theme
    glutPostRedisplay();
}

// Function to handle mouse events
void mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (x >= WINDOW_WIDTH / 2 - 20 && x <= WINDOW_WIDTH / 2 + 20 && y >= 10 && y <= 30) {
            resetGame(); // Reset game if reset button is clicked
        }
        
	// Check if the click is within the theme button area
        if (x >= WINDOW_WIDTH / 2 - 40 && x <= WINDOW_WIDTH / 2 + 40 &&
            y >= 40 && y <= 70) {
            cycleThemes(); // Cycle through themes
        }
    }

    if (x < 0 || y < GUI_HEIGHT || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) return; // Click outside the game board area
    int cellX = x / CELL_SIZE;
    int cellY = (y - GUI_HEIGHT) / CELL_SIZE;

    // Handle gameState
    if (gameOverFlag) {
        gameState = true;
    }
    if (winConditionMet) {
	gameState = true;
    }

    // When a game state is reached then disable input on the grid
    if (gameState == false) {
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
}

// Main Function
int main(int argc, char **argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Set display mode
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // Set window size
    glutCreateWindow("Open-Landmine");
    init(); // Initialize the game
    glutDisplayFunc(display); // Register display callback function
    glutMouseFunc(mouse); // Register mouse callback function
    glutKeyboardFunc(keyboard); // Register keyboard callback function
    glutReshapeFunc(reshape); // Register reshape callback function
    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}