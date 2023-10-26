//#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define N_INVADERS 55
#define PROB_MAX_VALUE 6

int SCREEN_WIDTH, SCREEN_HEIGHT;
int SCORE;
bool GAME_OVER;
 
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
  struct bullet* bullets;
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

struct bullet* pop_bullet(struct bullet** p, struct bullet *b) {
  struct bullet *top_bullet = *p;

  if (b->next_bullet != 0) {
    *p = b->next_bullet;
  } else {
    *p = 0;
  }
  free(b);

  return top_bullet->next_bullet;
  
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

void invaders_fire(struct Invaders *invaders) {
  int r = rand() % PROB_MAX_VALUE;
  if (r == 0) {
    struct bullet *current_bullet;
    struct bullet *next_bullet;

    if (invaders->bullets != 0) {
      current_bullet = get_last_bullet(invaders->bullets); 

      next_bullet = (struct bullet*) malloc(sizeof(struct bullet));
      current_bullet->next_bullet  = next_bullet;
    } else {
      next_bullet = (struct bullet*) malloc(sizeof(struct bullet));
      invaders->bullets  = next_bullet;
    }

    next_bullet->next_bullet = 0;
    next_bullet->x = invaders->x + (rand() % 11)*2;
    next_bullet->y = invaders->y + rand() % 5;
    next_bullet->skin = '.';
  }
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

  if (invaders->bullets != 0) {
    struct bullet *current_bullet = invaders->bullets;

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
  mvprintw(0,SCREEN_WIDTH-18, "Score: %d", SCORE);
}

void update_coords(struct Player *player, struct Invaders *invaders) {
  if (player->bullets != 0) {
    struct bullet *current_bullet = player->bullets;
    while (current_bullet != 0) {
      current_bullet->y -= 1;
      current_bullet = current_bullet->next_bullet;
    }
  }

  if (invaders->bullets != 0) {
    struct bullet *current_bullet = invaders->bullets;
    while (current_bullet != 0) {
      current_bullet->y += 1;
      current_bullet = current_bullet->next_bullet;
    }
  }

  if (invaders->x <= 0 || invaders->x + 2*11 >= SCREEN_WIDTH) {
    invaders->mov_dir *= -1;
    invaders->y += 1;
  }
  invaders->x += invaders->mov_dir*1;
}


int diff(int a, int b) {
  return abs(a - b);
}

void unlink_bullet(struct bullet *prev_bullet, struct bullet *bullet) {
  prev_bullet->next_bullet = bullet->next_bullet;
  free(bullet);
}

void clean_object_collision(struct Player *player, struct Invaders *invaders) {
  if (player->bullets != 0) {
    struct bullet *current_bullet = player->bullets;
    struct bullet *prev_bullet = 0;
    bool free_current_bullet = false;
    int x_diff;
    int y_diff;

    current_bullet = player->bullets;

    while (current_bullet != 0) {
      // remove all bullets that are offscreen 
      if (current_bullet->y < 0) {
        current_bullet = pop_bullet(&player->bullets, current_bullet);
        if (current_bullet == 0) {
          break;
        }
      }

      // compute distance to invader block
      x_diff = current_bullet->x - invaders->x;
      y_diff = current_bullet->y - invaders->y;     
      if (( x_diff < 11*2 && x_diff >= 0 ) && (y_diff < 5*2 && y_diff >= 0)) {
        if (invaders->status[(y_diff/2)*11 + x_diff/2] != 0) {
          invaders->status[(y_diff/2)*11 + x_diff/2] = 0;
          SCORE += 100;
      
          // unlink bullet from linked list
          if (prev_bullet == 0) {
            player->bullets = current_bullet->next_bullet;
          } else {
            prev_bullet->next_bullet = current_bullet->next_bullet;
          }
          free_current_bullet = true;
        }  
      }
      // free the unlinked bullet if necessary
      if (free_current_bullet) {
        if (prev_bullet == 0) {
          free(current_bullet);
          current_bullet  = player->bullets;
        } else {
          free(current_bullet);
          current_bullet = prev_bullet->next_bullet;
        }
        free_current_bullet = false;
      } else {
        prev_bullet = current_bullet;
        current_bullet = prev_bullet->next_bullet;
      }
    }
  }
  if (invaders->bullets != 0) {
    struct bullet *current_bullet = invaders->bullets;
    while (current_bullet->y > SCREEN_HEIGHT) {
      current_bullet = pop_bullet(&invaders->bullets, current_bullet);
    }

    while (current_bullet != 0) {
      if (current_bullet->x == player->x && current_bullet->y == player->y) {
        mvprintw(0,0, "game over");
        GAME_OVER = true;
      }
      current_bullet = current_bullet->next_bullet;
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
  SCORE = 0;
  player.x = SCREEN_WIDTH/2;
  player.y = (SCREEN_HEIGHT/4) *3;
  invaders.x = SCREEN_WIDTH/2 - 10;
  invaders.y = (SCREEN_HEIGHT/4);
  invaders.mov_dir = -1;

  while (!quit) {
    clear();

    draw_game(&player, &invaders);

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
          player.x -= 2;
        }
        break;
      case 'k':
        if (player.x < SCREEN_WIDTH-1) {
          player.x += 2;
        }
        break;
      default:
        break;
    }

    update_coords(&player, &invaders);
    invaders_fire(&invaders);
    clean_object_collision(&player, &invaders); // collisions of objects that do not terminate game (no player collision)    
  
    if (SCORE == 100*N_INVADERS) {
      GAME_OVER = true;
    }
 
    refresh();

    if (GAME_OVER) {
      quit = true;
    }

    napms(80);
  }

  if (GAME_OVER) { 
    while (true) {
      clear();
      mvprintw(SCREEN_WIDTH/2-4, SCREEN_HEIGHT/2, "GAME OVER");
  
      c = getch();
      if (c == 'q') {
        break;
      }
    }
  }

  endwin();
  return 0;
}
