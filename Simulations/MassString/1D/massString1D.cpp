#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <random>
#include <vector>
#include <SDL2/SDL.h>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_MIDDLE_X = SCREEN_WIDTH/2;
const int SCREEN_HEIGHT = 400;
const int SCREEN_MIDDLE_Y = SCREEN_HEIGHT/2;
const int BUFFER = 50;

const int N = 20;
const int POINTS = N+2;

double long X[POINTS]; 
double long Y[POINTS]; 
double long DY[POINTS]; 

double long K1[2*POINTS]; 
double long K2[2*POINTS]; 
double long K3[2*POINTS]; 
double long K4[2*POINTS]; 

const double long SEPARATION = (SCREEN_WIDTH-2*BUFFER)/(N-1);
const double long T = 5000; 
const double long m = 5;
const double long R = 2*m;
const double long k = T/(m*SEPARATION);
const double long c = 3;
const double long d = c/(2*sqrt(m*k));
const double long g = -2000;

bool init_sdl();
bool loadMedia();
bool close();

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;

class Mass;

void drawCircle(int x, int y, int r);

const double long DENSITY = 1;

class Mass {
    double long m;
    double long r;
    double long x;
    double long y;
    double long dx;
    double long dy;
    Uint8 red;
    Uint8 green;
    Uint8 blue;
    Uint8 alpha;
    public:
        Mass(double long _m, double long _r, double long _x, double long _y, double long _dx, double long _dy) {
            m = _m;
            r = _r;
            x = _x;
            y = _y;
            dx = _dx;
            dy = _dy;
        };
        double long mass() {
            return m;
        };
        double long X() {
            return x;
        };
        double long Y() {
            return y;
        };
        void DX(double long _dx) {
            dx = _dx;
        };
        void DY(double long _dy) {
            dy = _dy;
        };
        double long DX() {
            return dx;
        };
        double long DY() {
            return dy;
        };
        double long radius() {
            return r;
        }
        void draw() {
            drawCircle(x,SCREEN_MIDDLE_Y+y,r);
        };
};

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

void drawString() {
    int n;
    SDL_SetRenderDrawColor(renderer,0x00,0x00,0x00,0xFF);
    for (n = 0; n < POINTS-1; n++) {
        SDL_RenderDrawLine(renderer,X[n],SCREEN_MIDDLE_Y+Y[n],X[n+1],SCREEN_MIDDLE_Y+Y[n+1]);
    }
    SDL_SetRenderDrawColor(renderer,0xFF,0x00,0x00,0xFF);
    for (n = 1; n < POINTS-1; n++) {
        drawCircle(X[n],SCREEN_MIDDLE_Y+Y[n],R);
    }
    SDL_SetRenderDrawColor(renderer,0x55,0x55,0x55,0xFF);
}

double long a(double long y0, double long y1, double long y2, double long dy1) {
    return k*(y0 - 2*y1 + y2) - d*dy1;
}

void rk4(double long h) {
    int n;
    for (n = 1; n < POINTS-1; n++) {
        K1[2*n] = DY[n];
        K1[2*n+1] = a(Y[n-1],Y[n],Y[n+1],DY[n]);
    }
    for (n = 1; n < POINTS-1; n++) {
        K2[2*n] = DY[n] + (h/2.)*K1[2*n+1];
        K2[2*n+1] = a(Y[n-1]+(h/2.)*K1[2*(n-1)],Y[n]+(h/2.)*K1[2*n],Y[n+1]+(h/2.)*K1[2*(n+1)],DY[n]+(h/2.)*K1[2*n+1]);
    }
    for (n = 1; n < POINTS-1; n++) {
        K3[2*n] = DY[n] + (h/2.)*K2[2*n+1];
        K3[2*n+1] = a(Y[n-1]+(h/2.)*K2[2*(n-1)],Y[n]+(h/2.)*K2[2*n],Y[n+1]+(h/2.)*K2[2*(n+1)],DY[n]+(h/2.)*K2[2*n+1]);
    }
    for (n = 1; n < POINTS-1; n++) {
        K4[2*n] = DY[n] + h*K2[2*n+1];
        K4[2*n+1] = a(Y[n-1]+h*K3[2*(n-1)],Y[n]+h*K3[2*n],Y[n+1]+h*K3[2*(n+1)],DY[n]+h*K3[2*n+1]);
    }
    for (n = 1; n < POINTS-1; n++) {
        Y[n] = Y[n] + (h/6.)*(K1[2*n] + 2*K2[2*n] + 2*K3[2*n] + K4[2*n]);
        DY[n] = DY[n] + (h/6.)*(K1[2*n+1] + 2*K2[2*n+1] + 2*K3[2*n+1] + K4[2*n+1]);
    }
}

void rk2(double long h) {
    int n;
    for (n = 1; n < POINTS-1; n++) {
        K1[2*n] = DY[n];
        K1[2*n+1] = a(Y[n-1],Y[n],Y[n+1],DY[n]);
    }
    for (n = 1; n < POINTS-1; n++) {
        K2[2*n] = DY[n] + (h/2.)*K1[2*n];
        K2[2*n+1] = a(Y[n-1]+(h/2.)*K1[2*(n-1)+1],Y[n]+(h/2.)*K1[2*n+1],Y[n+1]+(h/2.)*K1[2*(n+1)+1],DY[n]+(h/2.)*K1[2*n+1]);
    }
    for (n = 1; n < POINTS-1; n++) {
        Y[n] = Y[n] + (h/2.)*(K1[2*n]+K2[2*n]);
        DY[n] = DY[n] + (h/2.)*(K1[2*n+1] + K2[2*n+1]);
    }
}

void euler(double long h) {
    for (int n = 1; n < POINTS-1; n++) {
        Y[n] = Y[n] + h*DY[n];
        DY[n] = DY[n] + h*a(Y[n-1],Y[n],Y[n+1],DY[n]);
    }
}

void updateMasses() {
    rk4(5e-4);
}

bool init() {
    int n = 0;
    X[n] = 0;
    DY[n] = 0;
    for (n = 1; n < POINTS-1; n++) {
        X[n] = BUFFER+SEPARATION*(n-1);
        Y[n] = 0;
        DY[n] = 0;
    }
    X[n] = SCREEN_WIDTH;
    DY[n] = 0;
    if (N%2 == 0) {
        Y[N/2] = 200;
        Y[N/2+1] = -200;
    } else {
        Y[N/2+1] = -200;
    }
    return true;
}

bool render() {
    bool success = true;

    SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(renderer);
    
    drawString();
    updateMasses();

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

        window = SDL_CreateWindow("1D Masses on String", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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


int main(int argc, char * argv[]){
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
