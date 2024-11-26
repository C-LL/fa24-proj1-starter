#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"
// const unsigned int DEFAULT_BOARD_WIDTH = 20;
// const unsigned int DEFAULT_BOARD_HEIGHT = 18;
#define BUFFER_SIZE 128
#define ROWS 18

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row,
                         unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.
  // game_state_t *g = malloc(sizeof(game_state_t));
  // g->num_rows = DEFAULT_BOARD_HEIGHT;
  // g->snakes = malloc(sizeof(snake_t));
  // snake_t *s0 = g->snakes;
  // s0->tail_row = 2;
  // s0->tail_col = 2;
  // s0->head_row = 2;
  // s0->head_col = 4;
  // s0->live = true;
  // g->board = malloc(sizeof(char *) * DEFAULT_BOARD_HEIGHT);
  // g->num_snakes = 1;

  // char **board = g->board;
  // for (int i = 0; i < DEFAULT_BOARD_HEIGHT; i++) {
  //   board[i] = malloc(sizeof(char) * DEFAULT_BOARD_WIDTH);
  //   for (int j = 0; j < DEFAULT_BOARD_WIDTH; j++) {
  //     if (i == 0 || j == 0 || i == DEFAULT_BOARD_HEIGHT ||
  //         j == DEFAULT_BOARD_WIDTH) {
  //       g->board[i][j] = '#';
  //     } else {
  //       g->board[i][j] = ' ';
  //     }
  //   }
  // }

  // g->board[s0->head_row][s0->head_col] = 'D';
  // g->board[s0->tail_row][s0->tail_col] = 'd';
  // g->board[s0->tail_row][s0->tail_col + 1] = '>';
  // g->board[2][9] = '*';

  // return g;

  game_state_t *state = malloc(sizeof(game_state_t));
  state->num_rows = 18;
  state->board = malloc(sizeof(char *) * 18);
  state->num_snakes = 1;
  state->snakes = malloc(sizeof(snake_t));

  // create board
  char **board = state->board;
  for (int i = 0; i < 18; i++) {
    board[i] = malloc(sizeof(char) * 21);
    board[i][0] = board[i][19] = '#';
    for (int j = 1; j < 19; j++) {
      board[i][j] = (i == 0 || i == 17) ? '#' : ' ';
    }
    board[i][20] = '\0';
  }
  board[2][2] = 'd';
  board[2][3] = '>';
  board[2][4] = 'D';
  board[2][9] = '*';

  // create snakes
  snake_t *snake = state->snakes;
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->live = true;

  return state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  if (!state)
    return;
  if (!state->board) {
    for (int i = 0; i < DEFAULT_BOARD_HEIGHT; i++) {
      free(state->board[i]);
    }
  }
  if (!state->snakes) {
    free(state->snakes);
  }
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row,
                         unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.ASDFGHJKL
  if (!(c == 'w' || c == 'a' || c == 's' || c == 'd')) {
    return false;
  }
  return true;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (!(c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x')) {
    return false;
  }
  return true;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' ||
      c == '>') {
    return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^')
    return 'w';
  if (c == '<')
    return 'a';
  if (c == 'v')
    return 's';
  if (c == '>')
    return 'd';
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W')
    return '^';
  if (c == 'A')
    return '<';
  if (c == 'S')
    return 'v';
  if (c == 'D')
    return '>';
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S')
    return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W')
    return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D')
    return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A')
    return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake
  is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  char c = get_board_at(state, state->snakes[snum].head_row,
                        state->snakes[snum].head_col);
  return get_board_at(state, get_next_row(state->snakes[snum].head_row, c),
                      get_next_col(state->snakes[snum].head_col, c));
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the
  head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  char c = get_board_at(state, state->snakes[snum].head_row,
                        state->snakes[snum].head_col);
  set_board_at(state, state->snakes[snum].head_row,
               state->snakes[snum].head_col, head_to_body(c));
  if (c == 'W') {
    set_board_at(state, state->snakes[snum].head_row - 1,
                 state->snakes[snum].head_col, 'W');
    state->snakes[snum].head_row--;
  }
  if (c == 'A') {
    set_board_at(state, state->snakes[snum].head_row,
                 state->snakes[snum].head_col - 1, 'A');
    state->snakes[snum].head_col--;
  }
  if (c == 'S') {
    set_board_at(state, state->snakes[snum].head_row + 1,
                 state->snakes[snum].head_col, 'S');
    state->snakes[snum].head_row++;
  }
  if (c == 'D') {
    set_board_at(state, state->snakes[snum].head_row,
                 state->snakes[snum].head_col + 1, 'D');
    state->snakes[snum].head_col++;
  }
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  char c = get_board_at(state, state->snakes[snum].tail_row,
                        state->snakes[snum].tail_col);
  set_board_at(state, state->snakes[snum].tail_row,
               state->snakes[snum].tail_col, ' ');
  if (c == 'w') {
    char body = get_board_at(state, state->snakes[snum].tail_row - 1,
                             state->snakes[snum].tail_col);
    set_board_at(state, state->snakes[snum].tail_row - 1,
                 state->snakes[snum].tail_col, body_to_tail(body));
    state->snakes[snum].tail_row--;
  }
  if (c == 'a') {
    char body = get_board_at(state, state->snakes[snum].tail_row,
                             state->snakes[snum].tail_col - 1);
    set_board_at(state, state->snakes[snum].tail_row,
                 state->snakes[snum].tail_col - 1, body_to_tail(body));
    state->snakes[snum].tail_col--;
  }
  if (c == 's') {
    char body = get_board_at(state, state->snakes[snum].tail_row + 1,
                             state->snakes[snum].tail_col);
    set_board_at(state, state->snakes[snum].tail_row + 1,
                 state->snakes[snum].tail_col, body_to_tail(body));
    state->snakes[snum].tail_row++;
  }
  if (c == 'd') {
    char body = get_board_at(state, state->snakes[snum].tail_row,
                             state->snakes[snum].tail_col + 1);
    set_board_at(state, state->snakes[snum].tail_row,
                 state->snakes[snum].tail_col + 1, body_to_tail(body));
    state->snakes[snum].tail_col++;
  }
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for (unsigned int snum = 0; snum < state->num_snakes; ++snum) {
    if (!state->snakes[snum].live)
      continue;
    if (next_square(state, snum) == '*') {
      update_head(state, snum);
      add_food(state);
    } else if (next_square(state, snum) == ' ') {
      update_head(state, snum);
      update_tail(state, snum);
    } else {
      state->snakes[snum].live = false;
      set_board_at(state, state->snakes[snum].head_row,
                   state->snakes[snum].head_col, 'x');
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  if (!fp)
    return NULL;
  int buffer_size = BUFFER_SIZE;
  char *buffer = malloc(buffer_size);

  int position = 0;
  char c;
  while (c = fgetc(fp) != EOF) {
    if (position > buffer_size - 1) {
      buffer_size *= 2;
      buffer = realloc(buffer, buffer_size);
    }
    if (c == '\n')
      break;
    buffer[position++] = c;
  }
  buffer[position] = '\0';
  return buffer;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  game_state_t *state = malloc(sizeof(game_state_t));
  state->snakes = NULL;
  state->num_rows = 0;
  state->num_snakes = 0;
  char *line;
  char **board = malloc(sizeof(char *) * ROWS);
  int max_len = 0;
  int rows = 0;
  while ((line = read_line(fp)) != NULL) {
    if (strlen(line) > max_len) {
      max_len = strlen(line);
    }
    if (rows > ROWS) {
      board = realloc(board, sizeof(char *) * (2 * ROWS));
    }
    board[rows++] = line;
  }
  state->board = malloc(sizeof(char) * rows);
  for (size_t i = 0; i < rows; i++) {
    strcpy(state->board[i], board[i]);
  }
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  return NULL;
}
