//#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

int SCREEN_WIDTH, SCREEN_HEIGHT;

// TODO: code enemies
// TODO: check for hits
// TODO: new animation cycle

struct Player {
  int x;
  int y;

  char skin;
  struct bullet* bullets;
};

struct bullet {
  int x;
  int y;
  
  char skin;
  struct bullet* next_bullet;  
};

struct Player player;

struct bullet* get_last_bullet(struct bullet *b) {
  while (b->next_bullet != 0) {
    b  = b->next_bullet; 
  }
  return b;
}

struct bullet* pop_bullet(struct Player *p, struct bullet *b) {
  if (b->next_bullet != 0) {
    p->bullets = b->next_bullet;
  } else {
    p->bullets = 0;
  }
  free(b);
  return p->bullets;
}

void shoot(struct Player *player) {
  struct bullet *current_bullet;
  struct bullet *next_bullet;

  if (player->bullets != 0) {
    current_bullet = get_last_bullet(player->bullets); 

    next_bullet = (struct bullet*) malloc(sizeof(struct bullet));
    current_bullet->next_bullet  = next_bullet;
  } else {
    next_bullet = (struct bullet*) malloc(sizeof(struct bullet));
    player->bullets  = next_bullet;
  }

  next_bullet->next_bullet = 0;
  next_bullet->x = player->x;
  next_bullet->y = player->y-1;
  next_bullet->skin = '.';
}

void draw_game(struct Player *player) {
  mvprintw(player->y, player->x, &(player->skin)); 

  if (player->bullets != 0) {
    struct bullet *current_bullet = player->bullets;

    while (current_bullet != 0) {
      mvprintw(current_bullet->y, current_bullet->x, &(current_bullet->skin)); 
      current_bullet = current_bullet->next_bullet;
    }
  } 
}

void update_coords(struct Player *player) {
  if (player->bullets != 0) {
    struct bullet *current_bullet = player->bullets;
    while (current_bullet != 0) {
      current_bullet->y -= 1;
      current_bullet = current_bullet->next_bullet;
    }
  } 
}

void clean_object_collision(struct Player *player) {
  if (player->bullets != 0) {
    struct bullet *current_bullet = player->bullets;
    while (current_bullet->y < 0) {
      current_bullet = pop_bullet(player, current_bullet);
      if (current_bullet == 0) {
        break;
      }
    }
    
  }
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
  player.bullets = 0;

  while (!quit) {
    clear();

    draw_game(&player);
    //mvprintw(player.y, player.x, &player.skin);

    if (player.bullets != 0) {
      mvprintw(0, 0, "%p", get_last_bullet(player.bullets));
    }

    c = 'e';
    while ((tmp = getch()) != EOF) {
      c = tmp;
    }
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

    update_coords(&player);
    clean_object_collision(&player); // collisions of objects that do not terminate game (no player collision)    

    refresh();
    napms(80);
  }
  endwin();

  return 0;
}
