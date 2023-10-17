//#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

int SCREEN_WIDTH, SCREEN_HEIGHT;

// TODO: code enemies
// TODO: create bullets
// TODO: check for hits
// TODO: new animation cycle

struct Player {
  int x;
  int y;

  char skin;
};

struct Player player;

int main() {
  bool quit = false;

  player.x = 0;
  player.y = 0;
  player.skin = '#';

  initscr();

  curs_set(0);  
  noecho();
  getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
  //cbreak();

  while (!quit) {
    clear();
    mvprintw(player.y, player.x, &player.skin);
    mvprintw(20, 20, "%d, %d", player.x, player.y);

    char c = getch();

    switch (c) {
      case 'q':
        quit = true; 
        break;
      case 'j':
        if (player.y != 0) {
          player.y -= 1;
        }
        break;
      case 'k':
        if (player.y < SCREEN_HEIGHT-1) {
          player.y += 1;
        }
        break;
      case 'h':
        if (player.x != 0) {
          player.x -= 1;
        }
        break;
      case 'l':
        if (player.x < SCREEN_WIDTH-1) {
          player.x += 1;
        }
        break;
    }

    refresh();
  }
  endwin();

  return 0;
}
