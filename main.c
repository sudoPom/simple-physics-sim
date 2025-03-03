// Example program:
// Using SDL3 to create an application window

#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 2540
#define HEIGHT 1300

#define COLOUR_WHITE 0xffffffff
#define COLOUR_BLACK 0x00000000
#define TRANSPARENT 0x000000ff
#define MAX_BALLS 1000000
#define BURST_COUNT 100

#define GRAVITY 0.5
#define ELASTICITY 1.0009
#define RADIUS 2
#define MASS 10

#define MIN_STARTING_SPEED 1
#define MAX_STARTING_SPEED 30

typedef struct {
  double x;
  double y;
  double r;
  double mass;
  double vx, vy;
  Uint32 colour;
} Circle;

void init_circle(Circle *circle, double x, double y, double r, double mass,
                 Uint32 colour) {
  circle->x = x;
  circle->y = y;
  circle->r = r;
  int directions[] = {1.0, -1.0};
  circle->mass = mass;
  circle->vx = (rand() % (int)(MAX_STARTING_SPEED + 1 - MIN_STARTING_SPEED) +
                MIN_STARTING_SPEED) *
               directions[rand() % 2];
  circle->vy = (rand() % (int)(MAX_STARTING_SPEED + 1 - MIN_STARTING_SPEED) +
                MIN_STARTING_SPEED) *
               directions[rand() % 2];
  circle->colour = colour;
}

void draw_circle(SDL_Surface *surface, Circle *circle) {
  double radius_squared = pow(circle->r, 2);
  for (int x = circle->x - circle->r; x <= circle->x + circle->r; x++) {
    for (int y = circle->y - circle->r; y <= circle->y + circle->r; y++) {
      double distance_squared = pow(circle->x - x, 2) + pow(circle->y - y, 2);
      if (distance_squared <= radius_squared) {
        SDL_Rect rect = (SDL_Rect){x, y, 1, 1};
        SDL_FillSurfaceRect(surface, &rect, circle->colour);
      }
    }
  }
}

void collide_circle_with_other_circle(Circle *circle, Circle *other) {
  double radius_squared = pow(circle->r, 2);
  double distance_squared =
      pow(circle->x - other->x, 2) - pow(circle->y - other->y, 2);
  if (distance_squared > radius_squared) {
    return;
  }
}

void accelerate_circles(Circle **circles, int num_circles) {
  for (int i = 0; i < num_circles; i++) {
    Circle *circle = circles[i];
    circle->vy += GRAVITY;
    circle->x += circle->vx;
    circle->y = circle->y + circle->vy;
  }
}

void draw_circles(SDL_Surface *surface, Circle **circles, int num_circles) {
  for (int i = 0; i < num_circles; i++) {
    Circle *circle = circles[i];
    draw_circle(surface, circle);
  }
}

void collide_ceil(Circle *circle) {
  if ((circle->y <= 0 + circle->r)) {
    double upper_bound = circle->r;
    circle->y = circle->y < upper_bound ? upper_bound : circle->y;
    circle->vy = circle->vy * ELASTICITY * -1.0;
    circle->colour = rand();
  }
}

void collide_floor(Circle *circle) {
  if ((circle->y >= HEIGHT - circle->r)) {
    double lower_bound = HEIGHT - circle->r;
    circle->y = circle->y > lower_bound ? lower_bound : circle->y;
    circle->vy = circle->vy * ELASTICITY * -1.0;
    if (pow(circle->vy, 2) > 1) {
      circle->colour = rand();
    }
  }
}

void collide_right(Circle *circle) {
  if ((circle->x >= WIDTH - circle->r)) {
    double lower_bound = WIDTH - circle->r;
    circle->x = circle->x > lower_bound ? lower_bound : circle->x;
    circle->vx = circle->vx * ELASTICITY * -1.0;
  }
}

void collide_left(Circle *circle) {
  if ((circle->x <= circle->r)) {
    double lower_bound = circle->r;
    circle->x = circle->x <= lower_bound ? lower_bound : circle->x;
    circle->vx = circle->vx * ELASTICITY * -1.0;
  }
}

void collide_circles(Circle **circles, int num_circles) {
  for (int i = 0; i < num_circles; i++) {
    Circle *circle = circles[i];
    collide_floor(circle);
    collide_ceil(circle);
    collide_left(circle);
    collide_right(circle);
  }
}

void add_circle(Circle **circles, int *circle_count, double x, double y) {
  Circle *circle = (Circle *)malloc(sizeof(Circle));
  init_circle(circle, x, y, RADIUS, MASS, COLOUR_WHITE);
  circles[(*circle_count)++] = circle;
}

// TODO: Circle Acceleration
// * Circle - circle collision
// * Circle - window collision

int main(int argc, char *argv[]) {

  SDL_Window *window; // Declare a pointer
  SDL_Surface *surface;
  SDL_Rect erase_rect;
  bool done = false;
  int circle_count = 0;
  Circle **circles = calloc(MAX_BALLS, sizeof(Circle));

  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("Physics Sim", WIDTH, HEIGHT, 0);
  SDL_SetWindowOpacity(window, 1.0);
  surface = SDL_GetWindowSurface(window);
  erase_rect = (SDL_Rect){0, 0, WIDTH, HEIGHT};

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
    SDL_FillSurfaceRect(surface, &erase_rect, COLOUR_BLACK);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
      if ((event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
           event.button.button == SDL_BUTTON_LEFT) &&
          circle_count < MAX_BALLS) {
        add_circle(circles, &circle_count, event.button.x, event.button.y);
      }
      if (event.button.button == SDL_BUTTON_RIGHT && event.button.down &&
          event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
          circle_count < MAX_BALLS) {
        for (int i = 0; i < BURST_COUNT; i++) {
          if (circle_count >= MAX_BALLS) {
            break;
          }
          add_circle(circles, &circle_count, event.button.x, event.button.y);
        }
      }
    }
    accelerate_circles(circles, circle_count);
    collide_circles(circles, circle_count);
    draw_circles(surface, circles, circle_count);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(10);
    // Do game logic, present a frame, etc.
  }

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}
