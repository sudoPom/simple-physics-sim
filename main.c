// Example program:
// Using SDL3 to create an application window

#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 1920
#define HEIGHT 1080

#define COLOUR_WHITE 0xffffffff

#define RADIUS 100
#define MASS 10

typedef SDL_Color SDL_Colour;

typedef struct {
  double x;
  double y;
  double r;
  double mass;
  double vx, vy;
} Circle;

void draw_circle(SDL_Surface *surface, Circle *circle, Uint32 colour) {
  double radius_squared = pow(circle->r, 2);
  for (int x = circle->x - circle->r; x <= circle->x + circle->r; x++) {
    for (int y = circle->y - circle->r; y <= circle->y + circle->r; y++) {
      double distance_squared = pow(circle->x - x, 2) + pow(circle->y - y, 2);
      if (distance_squared <= radius_squared) {
        SDL_Rect rect = (SDL_Rect){x, y, 1, 1};
        SDL_FillSurfaceRect(surface, &rect, colour);
      }
    }
  }
}

// TODO: Circle Acceleration
// * Circle - circle collision
// * Circle - window collision

int main(int argc, char *argv[]) {

  SDL_Window *window; // Declare a pointer
  SDL_Surface *surface;
  bool done = false;

  SDL_Init(SDL_INIT_VIDEO);

  // Create an application window with the following settings:
  window = SDL_CreateWindow("Physics Sim",   // window title
                            WIDTH, HEIGHT, 0 // flags - see below
  );
  surface = SDL_GetWindowSurface(window);

  // Check that the window was successfully created
  if (window == NULL) {
    // In the case that the window could not be made...
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n",
                 SDL_GetError());
    return 1;
  }

  SDL_UpdateWindowSurface(window);

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
      if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
          event.button.button == SDL_BUTTON_LEFT) {
        Circle circle = {event.button.x, event.button.y, RADIUS, MASS, 0, 0};
        draw_circle(surface, &circle, COLOUR_WHITE);
        SDL_UpdateWindowSurface(window);
      }
    }

    // Do game logic, present a frame, etc.
  }

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}
