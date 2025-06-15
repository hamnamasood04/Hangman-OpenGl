#include <GL/freeglut.h>  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MAX_TRIES 6

float headDropY = 0.6f;  
bool headDropping = false;

int shakeFrame = 0;
bool gameLostShake = false;

float bounceOffset = 0.0f;
bool gameWonBounce = false;

float bounceY = 0.0f;
bool goingUp = true;

char word[100];
char guessed[100];             
int wrongGuesses = 0;
char display[100];               

void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * (float)i / (float)num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawHangman() {
    glColor3f(0, 0, 0);
    glLineWidth(5.0);

    glBegin(GL_LINES);
    glVertex2f(-0.5, -0.8); glVertex2f(0.5, -0.8); 
    glVertex2f(0.0, -0.8); glVertex2f(0.0, 0.6);   
    glVertex2f(0.0, 0.6); glVertex2f(0.3, 0.6);     
    glVertex2f(0.3, 0.6); glVertex2f(0.3, 0.5);     
    glEnd();

    if (wrongGuesses > 0) {
     glColor3f(1, 0, 0);  
     glPushMatrix();
     glTranslatef(0.3, 0.4 + bounceY, 0); 
     drawCircle(0, 0, 0.05, 100);         
     glPopMatrix();
    }

    if (wrongGuesses > 1) {
        glBegin(GL_LINES);
        glVertex2f(0.3, 0.35); glVertex2f(0.3, 0.2);
        glEnd();
    }
    if (wrongGuesses > 2) {
        glBegin(GL_LINES);
        glVertex2f(0.3, 0.3); glVertex2f(0.2, 0.25);
        glEnd();
    }
    if (wrongGuesses > 3) {
        glBegin(GL_LINES);
        glVertex2f(0.3, 0.3); glVertex2f(0.4, 0.25);
        glEnd();
    }
    if (wrongGuesses > 4) {
        glBegin(GL_LINES);
        glVertex2f(0.3, 0.2); glVertex2f(0.2, 0.1);
        glEnd();
    }
    if (wrongGuesses > 5) {
        glBegin(GL_LINES);
        glVertex2f(0.3, 0.2); glVertex2f(0.4, 0.1);
        glEnd();
    }
}

void renderBitmapString(float x, float y, void *font, const char *string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string++);
    }
}

void displayFunc() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawHangman();

    glColor3f(0, 0, 1);
    renderBitmapString(-0.7, 0.8, GLUT_BITMAP_HELVETICA_18, "HANGMAN GAME");

    glColor3f(0, 0, 0);
    char wordDisplay[100] = "";
    for (int i = 0; i < strlen(word); i++) {
        char c[2] = "_";
        if (strchr(guessed, word[i]))
            c[0] = word[i];
        strncat(wordDisplay, c, 1);
        strcat(wordDisplay, " ");
    }

    renderBitmapString(-0.5, -0.4, GLUT_BITMAP_HELVETICA_18, wordDisplay);
    if (wrongGuesses >= MAX_TRIES) {
    gameLostShake = true;

    float shakeX = 0.0f;
    if (gameLostShake) {
        shakeX = 0.01f * ((shakeFrame % 2 == 0) ? -1 : 1); 
    }

    renderBitmapString(-0.5f + shakeX, -0.6f, GLUT_BITMAP_HELVETICA_18, "You Lost!");
   }else if (strcmp(display, word) == 0) {
        gameWonBounce = true;
        renderBitmapString(-0.5, -0.6 + bounceOffset, GLUT_BITMAP_HELVETICA_18, "You Won!");
    }

    glutSwapBuffers();
}

void timerFunc(int value) {
    
    if (gameLostShake) {
        shakeFrame++;
        if (shakeFrame > 10) {
            shakeFrame = 0;
            gameLostShake = false;
        }
    }

    if (headDropping) {
        headDropY -= 0.01f;
        if (headDropY <= 0.4f) {
            headDropY = 0.4f;
            headDropping = false;
        }
    }

    if (gameWonBounce) {
        bounceOffset += 0.01f;
        if (bounceOffset > 0.1f)
            bounceOffset = -0.1f;
    }
    
   if (wrongGuesses >= 1) {
     if (goingUp) {
         bounceY += 0.002f;
         if (bounceY > 0.02f)
             goingUp = false;
        } else {
         bounceY -= 0.002f;
        if (bounceY < -0.02f)
            goingUp = true;
        }
    }
    
    printf("bounceY = %.3f\n", bounceY);

    glutPostRedisplay();  
    glutTimerFunc(16, timerFunc, 0); 
}

void keyboardFunc(unsigned char key, int x, int y) {
    key = toupper(key);

    if (strchr(guessed, key) || wrongGuesses >= MAX_TRIES)
        return;

    strncat(guessed, &key, 1);

    if (!strchr(word, key)) {
        wrongGuesses++;
        if(wrongGuesses == 1){
            headDropping = true;
        }
    }

    for (int i = 0; i < strlen(word); i++) {
        display[i] = strchr(guessed, word[i]) ? word[i] : '_';
    }
    display[strlen(word)] = '\0';

    glutPostRedisplay();
}

void init() {
    glClearColor(0.83, 0.83, 0.83, 1.0); 
    strcpy(guessed, "");
    for (int i = 0; i < strlen(word); i++)
        display[i] = '_';
    display[strlen(word)] = '\0';
}

void inputWord(){
    char temp[100];
    printf("Enter the word to be guessed:\n");
    scanf("%s",temp);

    for(int i=0; temp[i]; i++){
        word[i] = toupper(temp[i]);
    }

    word[strlen(temp)] = '\0';
}

int main(int argc, char **argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Hangman with OpenGL");
    
    inputWord();

    init();
    glutDisplayFunc(displayFunc);
    glutIdleFunc(displayFunc); 
    glutKeyboardFunc(keyboardFunc);
    glutTimerFunc(0, timerFunc, 0);

    glutMainLoop();
    
    return 0;
}
