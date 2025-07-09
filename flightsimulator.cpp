#define _USE_MATH_DEFINES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <string>
#include <array>
#include <algorithm>

// Global variables
std::array<bool, 256> keys;

// Flight variables
float aircraftX = 0.0f, aircraftY = 150.0f, aircraftZ = 0.0f; // Force aircraft high above terrain for debug
float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
float velocity = 2.0f;  // Start with some default velocity
float throttle = 0.3f;  // Start with 30% throttle
float cameraDistance = 100.0f;

// Auto-pilot variables
bool autoPilot = true;
float autoTimer = 0.0f;

// Terrain size
const int TERRAIN_SIZE = 100;
std::array<std::array<float, TERRAIN_SIZE>, TERRAIN_SIZE> terrain;

// Function declarations
void InitOpenGL();
void RenderScene();
void UpdatePhysics();
void HandleInput(GLFWwindow* window);
void DrawAircraft();
void DrawTerrain();
void DrawHUD();
void GenerateTerrain();
void SetupProjection(GLFWwindow* window);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

// Generate simple terrain
void GenerateTerrain() {
    for (int x = 0; x < TERRAIN_SIZE; ++x) {
        for (int z = 0; z < TERRAIN_SIZE; ++z) {
            // Simple sine wave terrain
            terrain[x][z] = sin(x * 0.1f) * 10.0f + cos(z * 0.1f) * 5.0f;
        }
    }
}

// Initialize OpenGL
void InitOpenGL() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Sky blue
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up lighting
    GLfloat lightPos[] = { 0.0f, 100.0f, 0.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    GenerateTerrain();
}

// Draw aircraft (simple shape)
void DrawAircraft() {
    glPushMatrix();
    glTranslatef(aircraftX, aircraftY, aircraftZ);
    glRotatef(yaw, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(roll, 0.0f, 0.0f, 1.0f);

    glDisable(GL_LIGHTING);
    glColor3f(0.8f, 0.2f, 0.2f); // Red aircraft

    // Fuselage
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, 10.0f);  // Nose
    glVertex3f(-2.0f, 0.0f, -10.0f);
    glVertex3f(2.0f, 0.0f, -10.0f);

    glVertex3f(0.0f, 0.0f, 10.0f);
    glVertex3f(0.0f, 2.0f, -10.0f);
    glVertex3f(-2.0f, 0.0f, -10.0f);

    glVertex3f(0.0f, 0.0f, 10.0f);
    glVertex3f(2.0f, 0.0f, -10.0f);
    glVertex3f(0.0f, 2.0f, -10.0f);
    glEnd();

    // Wings
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
    glVertex3f(-15.0f, 0.0f, -2.0f);
    glVertex3f(15.0f, 0.0f, -2.0f);
    glVertex3f(10.0f, 0.0f, 2.0f);
    glVertex3f(-10.0f, 0.0f, 2.0f);
    glEnd();

    // DEBUG: Draw a large white cube at the aircraft position
    glColor3f(1.0f, 1.0f, 1.0f);
    float s = 5.0f;
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(-s, -s,  s); glVertex3f( s, -s,  s); glVertex3f( s,  s,  s); glVertex3f(-s,  s,  s);
    // Back face
    glVertex3f(-s, -s, -s); glVertex3f(-s,  s, -s); glVertex3f( s,  s, -s); glVertex3f( s, -s, -s);
    // Left face
    glVertex3f(-s, -s, -s); glVertex3f(-s, -s,  s); glVertex3f(-s,  s,  s); glVertex3f(-s,  s, -s);
    // Right face
    glVertex3f( s, -s, -s); glVertex3f( s,  s, -s); glVertex3f( s,  s,  s); glVertex3f( s, -s,  s);
    // Top face
    glVertex3f(-s,  s, -s); glVertex3f(-s,  s,  s); glVertex3f( s,  s,  s); glVertex3f( s,  s, -s);
    // Bottom face
    glVertex3f(-s, -s, -s); glVertex3f( s, -s, -s); glVertex3f( s, -s,  s); glVertex3f(-s, -s,  s);
    glEnd();

    glEnable(GL_LIGHTING);
    glPopMatrix();

    // DEBUG: Print aircraft position
    printf("Aircraft position: X=%.2f Y=%.2f Z=%.2f\n", aircraftX, aircraftY, aircraftZ);
}

// Draw terrain
// void DrawTerrain() {
//     glColor3f(0.2f, 0.8f, 0.2f); // Green terrain
//     for (int x = 0; x < TERRAIN_SIZE - 1; ++x) {
//         glBegin(GL_TRIANGLE_STRIP);
//         for (int z = 0; z < TERRAIN_SIZE; ++z) {
//             glVertex3f((x - TERRAIN_SIZE / 2) * 10.0f, terrain[x][z], (z - TERRAIN_SIZE / 2) * 10.0f);
//             glVertex3f((x + 1 -TERRAIN_SIZE / 2) * 10.0f, terrain[x + 1][z], (z - TERRAIN_SIZE / 2) * 10.0f);
//         }
//         glEnd();
//     }
// }

// Draw HUD
void DrawHUD() {
    // Switch to 2D rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Draw simple HUD elements
    glColor3f(0.0f, 1.0f, 0.0f); // Green HUD color

    // Draw crosshair
    glBegin(GL_LINES);
    glVertex2f(395, 300); glVertex2f(405, 300); // Horizontal line
    glVertex2f(400, 295); glVertex2f(400, 305); // Vertical line
    glEnd();

    // Draw altitude indicator (simple bar)
    float altBarHeight = std::min((aircraftY / 100.0f) * 100.0f, 100.0f);
    glBegin(GL_QUADS);
    glVertex2f(750, 500);
    glVertex2f(780, 500);
    glVertex2f(780, 500 - altBarHeight);
    glVertex2f(750, 500 - altBarHeight);
    glEnd();

    // Draw speed indicator
    float speedBarHeight = std::min((velocity / 10.0f) * 100.0f, 100.0f);
    glBegin(GL_QUADS);
    glVertex2f(20, 500);
    glVertex2f(50, 500);
    glVertex2f(50, 500 - speedBarHeight);
    glVertex2f(20, 500 - speedBarHeight);
    glEnd();

    // Draw autopilot indicator
    if (autoPilot) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow for autopilot
        glBegin(GL_QUADS);
        glVertex2f(10, 10);
        glVertex2f(100, 10);
        glVertex2f(100, 30);
        glVertex2f(10, 30);
        glEnd();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Handle input
void HandleInput(GLFWwindow* window) {
    // Toggle autopilot
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !keys[GLFW_KEY_SPACE]) {
        autoPilot = !autoPilot;
    }
    keys[GLFW_KEY_SPACE] = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    // Manual controls (only when autopilot is off)
    if (!autoPilot) {
        // Throttle control
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) throttle = std::min(throttle + 0.02f, 1.0f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) throttle = std::max(throttle - 0.02f, 0.0f);

        // Flight controls
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) yaw -= 2.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) yaw += 2.0f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) pitch -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) pitch += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) roll -= 2.0f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) roll += 2.0f;
    }

    // Camera distance (always available)
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) cameraDistance = std::max(cameraDistance - 5.0f, 50.0f);
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) cameraDistance = std::min(cameraDistance + 5.0f, 200.0f);
}

// Update physics
void UpdatePhysics() {
    // Autopilot behavior
    if (autoPilot) {
        autoTimer += 0.02f;

        // Smooth flight pattern
        yaw += sin(autoTimer * 0.5f) * 0.5f;
        pitch = sin(autoTimer * 0.3f) * 10.0f;
        roll = sin(autoTimer * 0.4f) * 15.0f;

        // Vary throttle slightly
        throttle = 0.4f + sin(autoTimer * 0.2f) * 0.2f;

        // Keep aircraft at reasonable altitude
        if (aircraftY < 30.0f) pitch = -20.0f;
        if (aircraftY > 100.0f) pitch = 20.0f;
    }

    // Simple flight physics
    velocity = throttle * 8.0f + 1.0f; // Minimum velocity to stay airborne

    // Apply velocity to position
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;

    aircraftX += sin(radYaw) * cos(radPitch) * velocity;
    aircraftZ += cos(radYaw) * cos(radPitch) * velocity;
    aircraftY += sin(radPitch) * velocity;

    // Simple gravity (less strong)
    aircraftY -= 0.2f;

    // Ground collision
    if (aircraftY < 10.0f) {
        aircraftY = 10.0f;
        velocity *= 0.8f; // Reduce speed on ground contact
        if (autoPilot) {
            throttle = 0.8f; // Increase throttle to take off
            pitch = -30.0f; // Pitch up
        }
    }

    // Limit pitch and roll for stability
    pitch = std::clamp(pitch, -60.0f, 60.0f);
    roll = std::clamp(roll, -60.0f, 60.0f);

    // Wrap yaw around 360 degrees
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;
}

// Set up projection matrix
void SetupProjection(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Framebuffer size callback
void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Render the scene
void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up camera
    glLoadIdentity();

    // Third-person camera
    float cameraX = aircraftX - sin(yaw * M_PI / 180.0f) * cameraDistance;
    float cameraY = aircraftY + 30.0f;
    float cameraZ = aircraftZ - cos(yaw * M_PI / 180.0f) * cameraDistance;

    gluLookAt(cameraX, cameraY, cameraZ,
              aircraftX, aircraftY, aircraftZ,
              0.0f, 1.0f, 0.0f);

    // Draw terrain (disabled for debug)
    // DrawTerrain();

    // Draw aircraft
    DrawAircraft();

    // Draw HUD
    DrawHUD();
}

int main() {
    // Set GLFW error callback for detailed error messages
    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
    });

    // Check DISPLAY environment variable (for X11)
    const char* displayEnv = getenv("DISPLAY");
    if (!displayEnv || displayEnv[0] == '\0') {
        fprintf(stderr, "WARNING: DISPLAY environment variable is not set. No graphical output will be possible.\n");
    } else {
        printf("DISPLAY variable: %s\n", displayEnv);
    }

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW!\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Flight Simulator", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    printf("OpenGL context and window created successfully.\n");

    // Set up projection matrix
    SetupProjection(window);
    // Set framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    InitOpenGL();
    printf("OpenGL initialized. Entering main loop.\n");

    while (!glfwWindowShouldClose(window)) {
        HandleInput(window);
        UpdatePhysics();
        RenderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    printf("Exiting main loop. Cleaning up.\n");
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}