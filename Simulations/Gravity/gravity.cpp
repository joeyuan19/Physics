#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <random>
#include <vector>
#include <SDL2/SDL.h>

using namespace std;

const int SCREEN_WIDTH  = 800;
const int SCREEN_MIDDLE_X  = SCREEN_WIDTH/2;
const int SCREEN_HEIGHT = 600;
const int SCREEN_MIDDLE_Y  = SCREEN_HEIGHT/2;

bool init_sdl();
bool loadMedia();
bool close();

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;

void drawCircle(int x, int y, int r);

const double long DENSITY = 1;
const double long G = 1;

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
        void setColor(Uint8 _red, Uint8 _green, Uint8 _blue, Uint8 _alpha) {
            red = _red;
            green = _green;
            blue = _blue;
            alpha = _alpha;
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
            SDL_SetRenderDrawColor(renderer, red, green, blue , alpha);
            drawCircle(x,y,r);
        };
        bool intersects(Mass other) {
            double long d = r + other.radius();
            double long dx = x-other.X();
            double long dy = y-other.Y();
            return d >= sqrt(dy*dy + dx*dx);
        }; 
        void update(double long ddx, double long ddy) {
            x += dx;
            y += dy;

            dx += ddx;
            dy += ddy;
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

void applyGravity(vector<Mass> * masses)  {
    int i, j;
    double long Fg, rx, ry, r2, Fx, Fy;
    for (i = 0; i < masses->size(); i++) {
        Fx = 0;
        Fy = 0;
        for (j = 0; j < masses->size(); j++) {
            if (i != j) {
                Fg = G*masses->at(i).mass()*masses->at(j).mass();
                rx = masses->at(j).X() - masses->at(i).X(); 
                ry = masses->at(j).Y() - masses->at(i).Y();
                r2 = rx*rx + ry*ry;
                Fx += (rx/sqrt(r2))*(Fg/r2);
                Fy += (ry/sqrt(r2))*(Fg/r2);
            }
        }
        Fx = Fx/masses->at(i).mass();
        Fy = Fy/masses->at(i).mass();
        masses->at(i).update(Fx,Fy);
    }
}

void applyBounces(vector<Mass> * masses) {
    int i, j;
    double long dx, dy, mi, mj, vix, viy, vjx, vjy, vx, vy;
    for (i = 0; i < masses->size()-1; i++) {
        for (j = i+1; j < masses->size(); j++) {
            if (masses->at(i).intersects(masses->at(j))) {
                mi = masses->at(i).mass();
                mj = masses->at(j).mass();

                vix = masses->at(i).DX();
                viy = masses->at(i).DY();
                
                vjx = masses->at(j).DX();
                vjy = masses->at(j).DY();

                vx = (vix*(mi-mj) + 2*mj*vjx)/(mi+mj);
                vy = (viy*(mi-mj) + 2*mj*vjy)/(mi+mj);
                masses->at(i).DX(.95*vx);
                masses->at(i).DY(.95*vy);

                vx = (vjx*(mj-mi) + 2*mi*vix)/(mi+mj);
                vy = (vjy*(mj-mi) + 2*mi*viy)/(mi+mj);
                masses->at(j).DX(.95*vx);
                masses->at(j).DY(.95*vy);
            }
        }
    }
}

void applyBoundaries(vector<Mass> * masses) {
    for (int i = 0; i < masses->size(); i++) {
        if (masses->at(i).X()-masses->at(i).radius() < 0 || masses->at(i).X()+masses->at(i).radius() > SCREEN_WIDTH) {
            masses->at(i).DX(-masses->at(i).DX());
        }
        if (masses->at(i).Y()-masses->at(i).radius() < 0 || masses->at(i).Y()+masses->at(i).radius() > SCREEN_HEIGHT) {
            masses->at(i).DY(-masses->at(i).DY());
        }
    }
}

Mass mass1 (10,30,SCREEN_MIDDLE_X-100,SCREEN_MIDDLE_Y,0,-1);
Mass mass2 (50,30,SCREEN_MIDDLE_X+100,SCREEN_MIDDLE_Y,0,+1);
Mass mass3 (10,30,SCREEN_MIDDLE_X+100,SCREEN_MIDDLE_Y-100,0,0);
Mass mass4 (50,30,SCREEN_MIDDLE_X+100,SCREEN_MIDDLE_Y+100,1,0);

vector<Mass> masses; 

void drawMasses(vector<Mass> * masses) {
    for (int i = 0; i < masses->size(); i++) {
        masses->at(i).draw();
    }
}

void moveMasses(vector<Mass> * masses) {
    for (int i = 0; i < masses->size(); i++) {
        masses->at(i).update(0,0);
    }
}


bool init() {
    mass1.setColor(0x00,0x00,0xFF,0xFF);
    masses.push_back(mass1);
    mass2.setColor(0x00,0xFF,0x00,0xFF);
    masses.push_back(mass2);
    mass3.setColor(0x44,0xFF,0xAA,0xFF);
    masses.push_back(mass3);
    mass4.setColor(0xFF,0x55,0x00,0xFF);
    masses.push_back(mass4);
    return true;
}

bool render() {
    bool success = true;

    SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(renderer);
    
    drawMasses(&masses);
    moveMasses(&masses);
    applyGravity(&masses);
    applyBounces(&masses);
    applyBoundaries(&masses);

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

        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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


int main(int argc, char *argv[ ] )
{
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
