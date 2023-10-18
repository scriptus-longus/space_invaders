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
  int *bullets;
};

struct bullet {
  int x;
  int y;
  
  char skin;
  int *next_bullet;  
};

struct Player player;

void shoot(struct Player *player) {
}

int main() {
  bool quit = false;
  char c;
  char tmp;
  player.skin = '#';

  initscr();

  curs_set(0);  
  nodelay(stdscr, true);
  noecho();
  cbreak();

  getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
  player.x = SCREEN_WIDTH/2;
  player.y = (SCREEN_HEIGHT/3) *2;

  while (!quit) {
    clear();
    mvprintw(player.y, player.x, &player.skin);

    c = 'e';
    while ((tmp = getch()) != EOF) {
      c = tmp;
    }

    mvprintw(0,0, "%c\n",  c);
    switch (c) {
      case 'q':
        quit = true; 
        break;
      case ' ':
        shoot(&player);
        break;
      case 'j':
        if (player.x != 0) {
          player.x -= 1;
        }
        break;
      case 'k':
        if (player.x < SCREEN_WIDTH-1) {
          player.x += 1;
        }
        break;
      default:
        break;
    }

    refresh();
    napms(80);
  }
  endwin();

  return 0;
}
