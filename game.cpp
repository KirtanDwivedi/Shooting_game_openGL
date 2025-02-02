#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>

// Game Constants
const int WIDTH = 800;
const int HEIGHT = 600;
const float PLAYER_SPEED = 10.0f;
const int TARGET_KILLS = 10;

// Game State
struct GameObject {
    float x, y;
    float size;
    bool active;
};

GameObject player = { WIDTH / 2, HEIGHT / 2, 20.0f, true };
std::vector<GameObject> bullets;
std::vector<GameObject> enemies;
int score = 0;
bool gameOver = false;

// Input States
bool keyStates[256] = { false };

void spawnEnemy() {
    if (enemies.size() < 100) { // Max number of enemies
        GameObject enemy = {
            (float)(rand() % WIDTH),
            (float)(rand() % HEIGHT),
            15.0f,
            true
        };
        enemies.push_back(enemy);
    }
}

void init() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
}

void drawPlayer() {
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(player.x - player.size / 2, player.y - player.size / 2);
    glVertex2f(player.x + player.size / 2, player.y - player.size / 2);
    glVertex2f(player.x + player.size / 2, player.y + player.size / 2);
    glVertex2f(player.x - player.size / 2, player.y + player.size / 2);
    glEnd();
}

void drawBullets() {
    glColor3f(1.0f, 1.0f, 0.0f);
    for (auto& b : bullets) {
        if (b.active) {
            glBegin(GL_QUADS);
            glVertex2f(b.x - 3, b.y - 3);
            glVertex2f(b.x + 3, b.y - 3);
            glVertex2f(b.x + 3, b.y + 3);
            glVertex2f(b.x - 3, b.y + 3);
            glEnd();
        }
    }
}

void drawEnemies() {
    glColor3f(1.0f, 0.0f, 0.0f);
    for (auto& e : enemies) {
        if (e.active) {
            glBegin(GL_QUADS);
            glVertex2f(e.x - e.size / 2, e.y - e.size / 2);
            glVertex2f(e.x + e.size / 2, e.y - e.size / 2);
            glVertex2f(e.x + e.size / 2, e.y + e.size / 2);
            glVertex2f(e.x - e.size / 2, e.y + e.size / 2);
            glEnd();
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (!gameOver) {
        drawPlayer();
        drawBullets();
        drawEnemies();

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(10, HEIGHT - 20);
        std::string scoreText = "Kills: " + std::to_string(score) + "/" + std::to_string(TARGET_KILLS);
        for (char c : scoreText) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
    }
    else {
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(WIDTH / 2 - 50, HEIGHT / 2);
        std::string endText = (score >= TARGET_KILLS)
            ? "MISSION COMPLETE!"
            : "GAME OVER!";
        for (char c : endText) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!gameOver) {
        // Player movement
        if (keyStates['a'] && player.x > player.size / 2) player.x -= PLAYER_SPEED;
        if (keyStates['d'] && player.x < WIDTH - player.size / 2) player.x += PLAYER_SPEED;
        if (keyStates['w'] && player.y < HEIGHT - player.size / 2) player.y += PLAYER_SPEED;
        if (keyStates['s'] && player.y > player.size / 2) player.y -= PLAYER_SPEED;

        // Update bullets
        for (auto& b : bullets) {
            if (b.active) {
                b.y += 10.0f;
                if (b.y > HEIGHT) b.active = false;
            }
        }

        // Update enemies
        for (auto& e : enemies) {
            if (e.active) {
                // Move towards player
                float dx = player.x - e.x;
                float dy = player.y - e.y;
                float dist = sqrt(dx * dx + dy * dy);
                if (dist > 0) {
                    e.x += (dx / dist) * 2.0f;
                    e.y += (dy / dist) * 2.0f;
                }
            }
        }

        // Bullet-Enemy collision
        for (auto& b : bullets) {
            if (b.active) {
                for (auto& e : enemies) {
                    if (e.active) {
                        float dx = b.x - e.x;
                        float dy = b.y - e.y;
                        if (sqrt(dx * dx + dy * dy) < e.size / 2) {
                            e.active = false;
                            b.active = false;
                            score++;
                            if (score >= TARGET_KILLS) gameOver = true;
                        }
                    }
                }
            }
        }

        // Player-Enemy collision
        for (auto& e : enemies) {
            if (e.active) {
                float dx = player.x - e.x;
                float dy = player.y - e.y;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < (player.size / 2 + e.size / 2)) {
                    gameOver = true;
                    break;
                }
            }
        }

        // Spawn new enemies
        if (rand() % 100 < 2) spawnEnemy();
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    keyStates[key] = true;

    if (key == ' ') { // Shoot
        GameObject bullet = { player.x, player.y + player.size / 2, 0, true };
        bullets.push_back(bullet);
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Shooting red");

    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);

    glutMainLoop();
    return 0;
}
