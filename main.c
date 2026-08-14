#include "./thirdparty/raylib/src/raylib.h"
#include "./thirdparty/raylib/src/raymath.h"
#include <stdbool.h>
#include <stdio.h>

#define POINTER_SIZE 5.0f
#define MAX_THINGS 200

typedef struct State_Manager {
  // General:
  bool is_reset_flag;

  // Mouse stuff:
  bool is_mouse_button_down;
  Vector2 mouse_start_position;
  Vector2 mouse_end_position;
  Vector2 mouse_direction;
  // - mouse arrow tip:
  Vector2 mouse_arrow_tip;
  Vector2 mouse_arrow_left_base_tip;
  Vector2 mouse_arrow_right_base_tip;

  // Saved things:
  int visible_count_of_things;
  int total_saved_things;
  struct {
    bool should_render;
    Color mouse_color;
    Vector2 mouse_start_position;
    Vector2 mouse_end_position;
    Vector2 mouse_direction;
    Vector2 mouse_arrow_tip;
    Vector2 mouse_arrow_left_base_tip;
    Vector2 mouse_arrow_right_base_tip;
  } saved_things[MAX_THINGS];

} State_Manager;

typedef struct Pointer {
  int x;
  int y;
  float radius;
} Pointer;

// -----------------------------------
// Modifiable Global State:
static Color POINTER_COLOR = RED;

struct Pointer pointer = {
  .x      = 0,
  .y      = 0,
  .radius = POINTER_SIZE
};

struct State_Manager sm = {
  .is_reset_flag              = true,  // It defaults to true as the initial state. Changes to false in draw_thing() and back to true in save_thing() or read_input().
  .is_mouse_button_down       = false,
  .mouse_start_position       = (Vector2){0},
  .mouse_end_position         = (Vector2){0},
  .mouse_direction            = (Vector2){0},
  .mouse_arrow_tip            = (Vector2){0},
  .mouse_arrow_left_base_tip  = (Vector2){0},
  .mouse_arrow_right_base_tip = (Vector2){0},
};
// ----------------------------------

void init_raylib(int window_width, int window_height, char *window_name, bool set_maximum_window_size) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);
  InitWindow(window_width, window_height, window_name);
  int monitor = GetCurrentMonitor();
  if (set_maximum_window_size) {
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    SetWindowPosition(GetMonitorPosition(monitor).x, GetMonitorPosition(monitor).y);
  }
  SetTargetFPS(60);
}

void render_saved_things() {
  if (sm.visible_count_of_things > 0) {
    for (int i = 0; i < MAX_THINGS; i++) {
      if (sm.saved_things[i].should_render) {
        DrawLineEx(sm.saved_things[i].mouse_start_position, sm.saved_things[i].mouse_end_position, POINTER_SIZE, sm.saved_things[i].mouse_color);
        DrawTriangle(sm.saved_things[i].mouse_arrow_tip, sm.saved_things[i].mouse_arrow_left_base_tip, sm.saved_things[i].mouse_arrow_right_base_tip, sm.saved_things[i].mouse_color);
      }
    }
  }
}

void save_thing() {
  if (!sm.is_mouse_button_down) {
    if (!sm.is_reset_flag) {
      if (sm.visible_count_of_things < MAX_THINGS) {
        // Save state:
        sm.saved_things[sm.visible_count_of_things].mouse_start_position       = sm.mouse_start_position;
        sm.saved_things[sm.visible_count_of_things].mouse_end_position         = sm.mouse_end_position;
        sm.saved_things[sm.visible_count_of_things].mouse_direction            = sm.mouse_direction;
        sm.saved_things[sm.visible_count_of_things].mouse_arrow_tip            = sm.mouse_arrow_tip;
        sm.saved_things[sm.visible_count_of_things].mouse_arrow_left_base_tip  = sm.mouse_arrow_left_base_tip;
        sm.saved_things[sm.visible_count_of_things].mouse_arrow_right_base_tip = sm.mouse_arrow_right_base_tip;
        sm.saved_things[sm.visible_count_of_things].should_render              = true;
        sm.saved_things[sm.visible_count_of_things].mouse_color                = POINTER_COLOR;

        sm.visible_count_of_things++;
        sm.total_saved_things = sm.visible_count_of_things;
      }
      // Reset values back to initial state:
      sm.is_reset_flag              = true;
      sm.is_mouse_button_down       = false;
      sm.mouse_start_position       = (Vector2){0};
      sm.mouse_end_position         = (Vector2){0};
      sm.mouse_direction            = (Vector2){0};
      sm.mouse_arrow_tip            = (Vector2){0};
      sm.mouse_arrow_left_base_tip  = (Vector2){0};
      sm.mouse_arrow_right_base_tip = (Vector2){0};
    }
  }
}

void draw_thing() {
  if (!sm.is_mouse_button_down) {
    // Draw pointer:
    DrawCircle(pointer.x, pointer.y, pointer.radius, POINTER_COLOR);
  } else {
    // Draw Arrow:
    if (sm.mouse_start_position.x == 0 && sm.mouse_start_position.y == 0) { // check if start positions was reset.
      sm.mouse_start_position = GetMousePosition();
    }
    sm.mouse_end_position         = (Vector2){pointer.x, pointer.y};
    sm.mouse_direction            = Vector2Normalize(Vector2Subtract(sm.mouse_end_position, sm.mouse_start_position));
    sm.mouse_arrow_tip            = Vector2Add(sm.mouse_end_position, Vector2Scale(sm.mouse_direction, 10.0f));
    sm.mouse_arrow_left_base_tip  = Vector2Add(sm.mouse_arrow_tip, Vector2Rotate(Vector2Scale(sm.mouse_direction, -15.0f), 0.5f));
    sm.mouse_arrow_right_base_tip = Vector2Add(sm.mouse_arrow_tip, Vector2Rotate(Vector2Scale(sm.mouse_direction, -15.0f), -0.5f));
    sm.is_reset_flag              = false;

    DrawLineEx(sm.mouse_start_position, sm.mouse_end_position, POINTER_SIZE, POINTER_COLOR);
    DrawTriangle(sm.mouse_arrow_tip, sm.mouse_arrow_left_base_tip, sm.mouse_arrow_right_base_tip, POINTER_COLOR);
  }
}

void read_input() {
  sm.is_mouse_button_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

  // Clear screen with CMD+C:
  if (IsKeyDown(KEY_LEFT_SUPER) && IsKeyPressed(KEY_C)) {
    sm = (struct State_Manager){0};
    sm.is_reset_flag = true;
  }
  // Undo with CMD+Z:
  if (IsKeyDown(KEY_LEFT_SUPER) && IsKeyPressed(KEY_Z)) {
    if (sm.visible_count_of_things > 0) {
      sm.visible_count_of_things--;
      sm.saved_things[sm.visible_count_of_things].should_render = false;
    }
  }
  // Redo with CMD+Y:
  if (IsKeyDown(KEY_LEFT_SUPER) && IsKeyPressed(KEY_Y)) {
    if (sm.visible_count_of_things < sm.total_saved_things) {
      sm.saved_things[sm.visible_count_of_things].should_render = true;
      sm.visible_count_of_things++;
    }
  }
  // Change mouse pointer/arrow color:
  if (IsKeyPressed(KEY_R)) {
    POINTER_COLOR = RED;
  } else if (IsKeyPressed(KEY_G)) {
    POINTER_COLOR = GREEN;
  } else if (IsKeyPressed(KEY_B)) {
    POINTER_COLOR = BLUE;
  }
}

void do_one_frame() {
  pointer.x = GetMousePosition().x;
  pointer.y = GetMousePosition().y;

  draw_thing();
  save_thing();
  render_saved_things();
  read_input();
}

void main_loop() {
  HideCursor();
  SetWindowFocused();
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLANK);
    do_one_frame();
    EndDrawing();
  }
  CloseWindow();
}

int main() {
  init_raylib(800, 450, "pointr", true);
  main_loop();

  return 0;
}
