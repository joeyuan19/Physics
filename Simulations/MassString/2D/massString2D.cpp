#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <random>
#include <vector>
#include <SDL2/SDL.h>

#define PI 3.141592653589793238

using namespace std;

const double long THETA = PI/4.;

const int SCREEN_WIDTH = 1000;
const int SCREEN_MIDDLE_X = SCREEN_WIDTH/2;
const int SCREEN_HEIGHT = 800;
const int SCREEN_MIDDLE_Y = SCREEN_HEIGHT/2;

const int GRID_HEIGHT_PROJ_Z = SCREEN_HEIGHT/2;
const int GRID_WIDTH = SCREEN_WIDTH - GRID_HEIGHT_PROJ_Z/tan(THETA);
const int GRID_HEIGHT = GRID_HEIGHT_PROJ_Z/sin(THETA);

double long DEG_X = 60;
double long DEG_Y = 30;
double long DEG_Z = 0;
double long DEG_X_ROTATION_RATE = 0;
double long DEG_Y_ROTATION_RATE = 0;
double long DEG_Z_ROTATION_RATE = 100;

const int Nx = 5;
const int Ny = 5;
const int POINTS_X = Nx+2;
const int POINTS_Y = Ny+2;
const int POINTS = POINTS_X*POINTS_Y;

double long X[POINTS]; 
double long Y[POINTS]; 
double long Z[POINTS]; 
double long DZ[POINTS]; 

double long rX[POINTS]; 
double long rY[POINTS]; 

double long K1[2*POINTS]; 
double long K2[2*POINTS]; 
double long K3[2*POINTS]; 
double long K4[2*POINTS]; 

const double long SEPARATION_X = GRID_WIDTH/(POINTS_X-1);
const double long SEPARATION_Y = GRID_HEIGHT/(POINTS_Y-1);
const double long T = 1; 
const double long m = 1;
const double long R = 10*m;
const double long k_x = T/(m*SEPARATION_X);
const double long k_y = T/(m*SEPARATION_Y);
const double long c = 0.001;
const double long d_x = c/(2*sqrt(m*k_x));
const double long d_y = c/(2*sqrt(m*k_y));
const double long g = -1;

bool init_sdl();
bool loadMedia();
bool close();

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;

double long mod(double long x, double long limit) {
    if (x > limit) {
        return x - limit; 
    } else {
        return x;
    }
}

double long degToRad(double long deg) {
    return PI*deg/180.0;
}

void drawCircle(int x, int y, int r) {
    int i, j;
    for (i = -r; i <= r; i++) {
        for (j = -r; j <= r; j++) {
            if (i*i + j*j <= r*r) {
                SDL_RenderDrawPoint(renderer,x+i,y+j);
            }
        }
    }
}

void set_x(int nx, int ny, double long x) {
    X[nx*POINTS_Y+ny] = x;
}

void set_y(int nx, int ny, double long y) {
    Y[nx*POINTS_Y+ny] = y;
}

void set_z(int nx, int ny, double long z) {
    Z[nx*POINTS_Y+ny] = z;
}

void set_dz(int nx, int ny, double long dz) {
    DZ[nx*POINTS_Y+ny] = dz;
}

double long get_x(int nx, int ny) {
    return X[nx*POINTS_Y+ny];
}

double long get_y(int nx, int ny) {
    return Y[nx*POINTS_Y+ny];
}

double long get_z(int nx, int ny) {
    return Z[nx*POINTS_Y+ny];
}

double long get_dz(int nx, int ny) {
    return DZ[nx*POINTS_Y+ny];
}

void set_rx(int nx, int ny, double long x) {
    rX[nx*POINTS_Y+ny] = x;
}

void set_ry(int nx, int ny, double long y) {
    rY[nx*POINTS_Y+ny] = y;
}

double long get_rx(int nx, int ny) {
    return SCREEN_MIDDLE_X + rX[nx*POINTS_Y+ny];
}

double long get_ry(int nx, int ny) {
    return SCREEN_MIDDLE_Y + rY[nx*POINTS_Y+ny];
}

void set_k1_z(int nx, int ny, double long k1_z) {
    K1[2*(nx*POINTS_Y+ny)] = k1_z;
}

void set_k1_dz(int nx, int ny, double long k1_dz) {
    K1[2*(nx*POINTS_Y+ny)+1] = k1_dz;
}

void set_k2_z(int nx, int ny, double long k2_z) {
    K2[2*(nx*POINTS_Y+ny)] = k2_z;
}

void set_k2_dz(int nx, int ny, double long k2_dz) {
    K2[2*(nx*POINTS_Y+ny)+1] = k2_dz;
}

void set_k3_z(int nx, int ny, double long k3_z) {
    K3[2*(nx*POINTS_Y+ny)] = k3_z;
}

void set_k3_dz(int nx, int ny, double long k3_dz) {
    K3[2*(nx*POINTS_Y+ny)+1] = k3_dz;
}

void set_k4_z(int nx, int ny, double long k4_z) {
    K4[2*(nx*POINTS_Y+ny)] = k4_z;
}

void set_k4_dz(int nx, int ny, double long k4_dz) {
    K4[2*(nx*POINTS_Y+ny)+1] = k4_dz;
}

double long get_k1_z(int nx, int ny) {
    return K1[2*(nx*POINTS_Y+ny)];
}

double long get_k1_dz(int nx, int ny) {
    return K1[2*(nx*POINTS_Y+ny)+1];
}

double long get_k2_z(int nx, int ny) {
    return K2[2*(nx*POINTS_Y+ny)];
}

double long get_k2_dz(int nx, int ny) {
    return K2[2*(nx*POINTS_Y+ny)+1];
}

double long get_k3_z(int nx, int ny) {
    return K3[2*(nx*POINTS_Y+ny)];
}

double long get_k3_dz(int nx, int ny) {
    return K3[2*(nx*POINTS_Y+ny)+1];
}

double long get_k4_z(int nx, int ny) {
    return K4[2*(nx*POINTS_Y+ny)];
}

double long get_k4_dz(int nx, int ny) {
    return K4[2*(nx*POINTS_Y+ny)+1];
}

double long * rotate_x(double long x, double long y, double long z, double long degrees) {
    double long a = degToRad(degrees);
    return new double long[3] {x, y*cos(a) - z*sin(a), y*sin(a) + z*cos(a)};
}

double long * rotate_y(double long x, double long y, double long z, double long degrees) {
    double long a = degToRad(degrees);
    return new double long[3] {x*cos(a) + z*sin(a), y, -x*sin(a) + z*cos(a)}; 
}

double long * rotate_z(double long x, double long y, double long z, double long degrees) {
    double long a = degToRad(degrees);
    return new double long[3] {x*cos(a) - y*sin(a), x*sin(a) + y*cos(a), z}; 
}

void rotate_grid(double long degrees_x, double long degrees_y, double long degrees_z) {
    double long * r;
    for (int n = 0; n < POINTS; n++) {
        r = rotate_x(X[n],Y[n],Z[n],degrees_x);
        r = rotate_y(r[0],r[1],r[2],degrees_y);
        r = rotate_z(r[0],r[1],r[2],degrees_z);
        rX[n] = r[0];
        rY[n] = r[1];
    }
}

void drawGrid() {
    int nx, ny;
    rotate_grid(DEG_X,DEG_Y,DEG_Z);
    SDL_SetRenderDrawColor(renderer,0x00,0x00,0x00,0xFF);
    for (nx = 0; nx < POINTS_X-1; nx++) {
        for (ny = 0; ny < POINTS_Y-1; ny++) {
            SDL_RenderDrawLine(renderer,
                    SCREEN_MIDDLE_X+get_x(nx  ,ny)+get_y(nx,ny)*cos(THETA),
                    SCREEN_MIDDLE_Y+get_z(nx  ,ny)+get_y(nx  ,ny)*sin(THETA),
                    SCREEN_MIDDLE_X+get_x(nx+1,ny)+get_y(nx+1,ny)*cos(THETA),
                    SCREEN_MIDDLE_Y+get_z(nx+1,ny)+get_y(nx+1,ny)*cos(THETA));
            SDL_RenderDrawLine(renderer,
                    SCREEN_MIDDLE_X+get_x(nx,ny  )+get_y(nx,ny  )*cos(THETA),
                    SCREEN_MIDDLE_Y+get_z(nx,ny  )+get_y(nx,ny  )*sin(THETA),
                    SCREEN_MIDDLE_X+get_x(nx,ny+1)+get_y(nx,ny+1)*cos(THETA),
                    SCREEN_MIDDLE_Y+get_z(nx,ny+1)+get_y(nx,ny+1)*sin(THETA));
        }
    }
    for (nx = 0; nx < POINTS_X-1; nx++) {
        SDL_RenderDrawLine(renderer,
                SCREEN_MIDDLE_X+get_x(nx  ,POINTS_Y-1)+get_y(nx  ,POINTS_Y-1)*cos(THETA),
                SCREEN_MIDDLE_Y+get_z(nx  ,POINTS_Y-1)+get_y(nx  ,POINTS_Y-1)*sin(THETA),
                SCREEN_MIDDLE_X+get_x(nx+1,POINTS_Y-1)+get_y(nx+1,POINTS_Y-1)*cos(THETA),
                SCREEN_MIDDLE_Y+get_z(nx+1,POINTS_Y-1)+get_y(nx+1,POINTS_Y-1)*sin(THETA));
    }
    for (ny = 0; ny < POINTS_Y-1; ny++) {
        SDL_RenderDrawLine(renderer,
                SCREEN_MIDDLE_X+get_x(POINTS_X-1,ny  )+get_y(POINTS_X-1,ny  )*cos(THETA),
                SCREEN_MIDDLE_Y+get_z(POINTS_X-1,ny  )+get_y(POINTS_X-1,ny  )*cos(THETA),
                SCREEN_MIDDLE_X+get_x(POINTS_X-1,ny+1)+get_y(POINTS_X-1,ny+1)*cos(THETA),
                SCREEN_MIDDLE_Y+get_z(POINTS_X-1,ny+1)+get_y(POINTS_X-1,ny+1)*cos(THETA));
    }
    SDL_SetRenderDrawColor(renderer,0xFF,0x00,0x00,0xFF);
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
            drawCircle(SCREEN_MIDDLE_X+get_x(nx,ny)+get_y(nx,ny)*cos(THETA),
                SCREEN_MIDDLE_Y+get_z(nx,ny)+get_y(nx,ny)*sin(THETA),
                R);
        }
    }
}

double long a(double long left, double long right, double long up, double long down, double long z, double long dz) {
    return k_x*(left - 2*z + right) + k_y*(up - 2*z + down) - (d_x + d_y)*dz; // - g;
}

void rk4(double long h) {
    int nx, ny;
    double long h2 = h/2.;
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
            set_k1_z(nx,ny,get_dz(nx,ny));
            set_k1_dz(nx,ny,a(get_z(nx-1,ny),get_z(nx+1,ny),get_z(nx,ny+1),get_z(nx,ny-1),get_z(nx,ny),get_dz(nx,ny)));
        }
    }
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
        set_k2_z(nx,ny,get_dz(nx,ny) + h2*get_k1_z(nx,ny));
        set_k2_dz(nx,ny,a(get_z(nx-1,ny)+h2*get_k1_dz(nx-1,ny),
                          get_z(nx+1,ny)+h2*get_k1_dz(nx+1,ny),
                          get_z(nx,ny-1)+h2*get_k1_dz(nx,ny-1),
                          get_z(nx,ny+1)+h2*get_k1_dz(nx,ny+1),
                          get_z(nx,ny)+h2*get_k1_dz(nx,ny),
                          get_dz(nx,ny)+h2*get_k1_dz(nx,ny)));
        }
    }
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
        set_k3_z(nx,ny,get_dz(nx,ny) + h2*get_k2_z(nx,ny));
        set_k3_dz(nx,ny,a(get_z(nx-1,ny)+h2*get_k2_dz(nx-1,ny),
                          get_z(nx+1,ny)+h2*get_k2_dz(nx+1,ny),
                          get_z(nx,ny-1)+h2*get_k2_dz(nx,ny-1),
                          get_z(nx,ny+1)+h2*get_k2_dz(nx,ny+1),
                          get_z(nx,ny)+h2*get_k2_dz(nx,ny),
                          get_dz(nx,ny)+h2*get_k2_dz(nx,ny)));
        }
    }
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
        set_k4_z(nx,ny,get_dz(nx,ny) + h*get_k3_z(nx,ny));
        set_k4_dz(nx,ny,a(get_z(nx-1,ny)+h*get_k3_dz(nx-1,ny),
                          get_z(nx+1,ny)+h*get_k3_dz(nx+1,ny),
                          get_z(nx,ny-1)+h*get_k3_dz(nx,ny-1),
                          get_z(nx,ny+1)+h*get_k3_dz(nx,ny+1),
                          get_z(nx,ny)+h*get_k3_dz(nx,ny),
                          get_dz(nx,ny)+h*get_k3_dz(nx,ny)));
        }
    }
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
            set_z(nx,ny,get_z(nx,ny) + (h/6.)*(get_k1_z(nx,ny)+2*get_k2_z(nx,ny)+2*get_k3_z(nx,ny)+get_k4_z(nx,ny)));
            set_dz(nx,ny,get_dz(nx,ny) + (h/6.)*(get_k1_dz(nx,ny)+2*get_k2_dz(nx,ny)+2*get_k3_dz(nx,ny)+get_k4_dz(nx,ny)));
        }
    }
}

void rk2(double long h) {
    int nx, ny;
    double long h2 = h/2.;
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
            set_k1_z(nx,ny,get_dz(nx,ny));
            set_k1_dz(nx,ny,a(get_z(nx-1,ny),get_z(nx+1,ny),get_z(nx,ny+1),get_z(nx,ny-1),get_z(nx,ny),get_dz(nx,ny)));
        }
    }
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
        set_k2_z(nx,ny,get_dz(nx,ny) + h2*get_k1_z(nx,ny));
        set_k2_dz(nx,ny,a(get_z(nx-1,ny)+h2*get_k1_dz(nx-1,ny),
                          get_z(nx+1,ny)+h2*get_k1_dz(nx+1,ny),
                          get_z(nx,ny-1)+h2*get_k1_dz(nx,ny-1),
                          get_z(nx,ny+1)+h2*get_k1_dz(nx,ny+1),
                          get_z(nx,ny)+h2*get_k1_dz(nx,ny),
                          get_dz(nx,ny)+h2*get_k1_dz(nx,ny)));
        }
    }
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
            set_z(nx,ny,get_z(nx,ny) + h2*(get_k1_z(nx,ny)+get_k2_z(nx,ny)));
            set_dz(nx,ny,get_dz(nx,ny) + h2*(get_k1_dz(nx,ny)+get_k2_dz(nx,ny)));
        }
    }
}

void euler(double long h) {
    int nx, ny;
    for (nx = 1; nx < POINTS_X-1; nx++) {
        for (ny = 1; ny < POINTS_Y-1; ny++) {
            set_z(nx,ny,get_z(nx,ny) + h*get_dz(nx,ny));
            set_dz(nx,ny,get_dz(nx,ny) + h*a(get_z(nx-1,ny),get_z(nx+1,ny),get_z(nx,ny-1),get_z(nx,ny+1),get_z(nx,ny),get_dz(nx,ny)));
        }
    }
}

void updateMasses() {
    rk4(5e-2);
}

void rotateView() {
    DEG_X = mod(DEG_X+DEG_X_ROTATION_RATE,360); 
    DEG_Y = mod(DEG_Y+DEG_Y_ROTATION_RATE,360); 
    DEG_Z = mod(DEG_Z+DEG_Z_ROTATION_RATE,360); 
}

bool init() {
    int nx, ny;
    for (nx = 0; nx < POINTS_X; nx++) {
        for (ny = 0; ny < POINTS_Y; ny++) {
            set_x(nx,ny,SEPARATION_X*nx-GRID_WIDTH/2);
            set_y(nx,ny,SEPARATION_Y*ny-GRID_HEIGHT/2);
            set_z(nx,ny,0);
            set_dz(nx,ny,0);
        }
    }
    set_z(POINTS_X/2,POINTS_Y/2,100);
    return true;
}

bool render() {
    bool success = true;

    SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(renderer);
    
    drawGrid();
    updateMasses();
    rotateView();
    
    SDL_RenderPresent(renderer);
    return success;
}

bool init_sdl() {
    bool success = true;

    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    } else {
        if ( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1") ) {
            printf("Warning: Linear texture filtering not enabled!");
        }

        window = SDL_CreateWindow("2D Masses on String Grid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if ( window == NULL ) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            SDL_SetRenderDrawColor(renderer,0xff,0xff,0xff,0xff);
            SDL_RenderClear(renderer);
        }
    }
    return success;
}

bool loadMedia() {
    bool success = true;
    return success;
}

bool close() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();

    return true;
}


int main(int argc, char * argv[]) {
    if ( !init_sdl() ) {
        printf("Failed to initialize!\n");
    } else {
        if ( !loadMedia() ) {
            printf("Failed to load media!\n");
        } else {
            bool quit = false;
            SDL_Event e;
            if (init()) {
                while ( !quit ) {
                    while ( SDL_PollEvent( &e ) != 0 ) {
                        if ( e.type == SDL_QUIT ) {
                            quit = true;
                        }
                    }
                    if (!render()) {
                        quit = true;
                    }
                }
            }
        }
    }
    close();
    return 0;
}
