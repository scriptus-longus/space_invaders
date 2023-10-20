//#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>


#define N_INVADERS 55
int SCREEN_WIDTH, SCREEN_HEIGHT;

// TODO: check for hits
// TODO: new animation cycle

struct Player {
  signed int x;
  signed int y;

  char skin;
  struct bullet* bullets;
};

struct Invaders {
  int x;   // top left
  int y;
 
  unsigned int mov_dir;

  char skin;
  int status[N_INVADERS]; 
};

struct bullet {
  signed int x;
  signed int y;
  
  char skin;
  struct bullet* next_bullet;  
};

struct Invaders invaders;
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

void draw_game(struct Player *player, struct Invaders *invaders) {
  mvprintw(player->y, player->x, &(player->skin)); 

  if (player->bullets != 0) {
    struct bullet *current_bullet = player->bullets;

    while (current_bullet != 0) {
      mvprintw(current_bullet->y, current_bullet->x, &(current_bullet->skin)); 
      current_bullet = current_bullet->next_bullet;
    }
  } 

  for (int i = 0; i < N_INVADERS; i++) {
    if (invaders->status[i] == 1) {
      mvprintw(invaders->y + (i/11)*2, invaders->x + (i % 11)*2, &(invaders->skin));
    }
  }
}

void update_coords(int W, int H, struct Player *player, struct Invaders *invaders) {
  if (player->bullets != 0) {
    struct bullet *current_bullet = player->bullets;
    while (current_bullet != 0) {
      current_bullet->y -= 1;
      current_bullet = current_bullet->next_bullet;
    }
  }
  if (invaders->x <= 0 || invaders->x + 2*11 >= W) {
    invaders->mov_dir *= -1;
    invaders->y += 1;
  }
  invaders->x += invaders->mov_dir*1;
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
  player.bullets = 0;

  invaders.skin = '@';
  for (int i = 0; i < N_INVADERS; i++) {
    invaders.status[i] = 1;
  }

  initscr();

  curs_set(0);  
  nodelay(stdscr, true);
  noecho();
  cbreak();

  getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
  player.x = SCREEN_WIDTH/2;
  player.y = (SCREEN_HEIGHT/4) *3;
  invaders.x = SCREEN_WIDTH/2 - 10;
  invaders.y = (SCREEN_HEIGHT/4);
  invaders.mov_dir = -1;

  while (!quit) {
    clear();

    draw_game(&player, &invaders);
    mvprintw(invaders.y, invaders.x, &player.skin);

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

    update_coords(SCREEN_WIDTH, SCREEN_HEIGHT, &player, &invaders);
    clean_object_collision(&player); // collisions of objects that do not terminate game (no player collision)    

    refresh();
    napms(80);
  }
  endwin();

  return 0;
}
