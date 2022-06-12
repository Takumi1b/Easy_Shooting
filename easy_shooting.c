/*
  easy_shooting.c
  最終課題:アニメ-ションの作成
  条件:HandyGraphicを使う
  配列,関数,構造体等の授業で取り扱った内容を用いる
  g1853509 Takumi Tanabe
  2019/1/15
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <handy.h>

#define WINDOWSIZE 640
#define RIGHT_LIMIT 590
#define LEFT_LIMIT 30
#define DISPLAY_ENEMY_MAX 30
#define BLOCKS 30
#define EFFECTS 30


typedef struct PLAYER{
    double x, y;
    double speed;
    int life;
    int direction;//向き
    int move_flag;//動けるか否か
    int left_flag;//左に動けるか否か
    int right_flag;//右に動けるか否か
    int reload_flag;//リロードしてるか否か
    //描画用
    int draw_normal;
    int draw_right;
    int draw_left;
    int draw_shot;
    int draw_reload;
    
    int shot_direction;//ショットの向き
    double player_block_length[30];//足場までの長さ
    int revival_block;//復活させるブロックの番号
}PLAYER;

PLAYER player;

typedef struct PLAYER_SHOT{
    double x, y;
    int initialize_flag;
    int move_flag;
    int move_type;//ショットの種類
    double right_shot_speed_x;
    double left_shot_speed_x;
    double shot_speed_y;
    int draw_flag;
    double range;
    int reflect_flag;//反射してるか否か
}PLAYER_SHOT;

PLAYER_SHOT player_shot;


typedef struct ENEMY{
    double x, y;
    double speed;
    int initialize_flag;
    int move_flag;
    int life;
    double range;
    int draw_enemy_flag;
    
}ENEMY;

ENEMY enemy[DISPLAY_ENEMY_MAX];
ENEMY block_enemy[DISPLAY_ENEMY_MAX];
ENEMY block_10_enemy[DISPLAY_ENEMY_MAX];

typedef struct BLOCK{
    double x,y;
    int initialize_flag;
    int delete_flag;//削除するか否か
    int on_player_flag;//プレイヤーが上にのってるか否か
}BLOCK;

BLOCK block[BLOCKS];

typedef struct EFFECT{
    double ex, ey;
    double sx1, sy1, sx2, sy2, sx3, sy3;
    int explosion_flag;
    int score_effect;
}EFFECT;

EFFECT effect[EFFECTS];

typedef struct PLAYER_SHOT_BAR{
    double x, y;
    double dx, dy;
    int speed;
    int move_flag;
    int shot_type;//ショットの種類
    
}PLAYER_SHOT_BAR;

PLAYER_SHOT_BAR player_shot_bar;

typedef struct REST_SHOT{
    double x, y;
    int shot_count;//残弾数カウント
    int shot_flag;
    int display_rest_shot;
}REST_SHOT;

REST_SHOT rest_shot;

typedef struct SCORE{
    double x ,y;
    double hx, hy;
    int score_flag;
    int count_score;
    int speed_change_flag;//スコアによるゲームスピードの変更
    int high_score;
    
}SCORE;

SCORE score;

typedef struct CURSOR{
    double tx, ty;
    double ex, ey;
    int title_flag;
    int end_flag;
}CURSOR;

CURSOR cursor;

typedef struct TEXT{
    double x, y[4];
    int text_state[4];
    int enemy_text;
    int shot_text;
    int reload_text;
}TEXT;

TEXT text;

void draw_title(int wid);
int move_title(int game_state, hgevent *event, int wid);

void draw_how_to_play(int wid, int img);

void draw_game_screen(int wid);

void initialize_cursor();
void move_cursor(hgevent *event);
void draw_cursor(int layers);

void initialize_player();
void move_player(hgevent *event);
void draw_player(int layres, int img);

void initialize_player_shot();
void move_player_shot();
void draw_player_shot(int layres, int img);
void set_player_shot(int shot_type);

void initialize_enemy();
void move_enemy();
void draw_enemy(int layers, int img);
void set_enemy();

void move_block_enemy();
void draw_block_enemy(int layers, int img);
void set_block_enemy();

void move_block_10_enemy();
void draw_block_10_enemy(int layers, int img);
void set_block_10_enemy();

void initialize_block();
void move_block();
void draw_block(int layers, int img);

void initialize_effect();
void move_effect();
void draw_effect(int layers, int img);

void initialize_player_shot_bar();
void move_player_shot_bar();
void draw_player_shot_bar(int layers);

void initialize_rest_shot();
void draw_rest_shot(int layers);


void initialize_score_board();
void draw_score_board(int layers);

void initialize_text_board();
void move_text_board();
void draw_text_board(int layers);

//プレイヤーと抜けてる床までの最小の値をとる
int min_player_block_length(double player_block_length[]);

//当たり判定用関数-------------
int pythago_theorem(double x, double x2, double y, double y2, double range, double range2);
void colision_ditection();
void colision_player_enemy();
void colision_player_block_enemy();
void colision_player_block_10_enemy();
void colision_player_shot_enemy();
void colision_player_shot_block_enemy();
void colision_player_shot_block_10_enemy();
void colision_enemy_block();
void colision_block_enemy_block();
void colision_block_10_enemy_block();
void check_enemy_life();
void check_block_enemy_life();
void check_block_10_enemy_life();
void check_player_block();
int check_player_life();
//-----------------------------

void draw_gameover(int layers);

int move_end(int game_state, hgevent *event, int wid);
void draw_end(int layers);

void timer();
void draw_timer(int layers);
int count_delete_block();
void change_speed();
void game_speed();
void draw_high_score();
void high_score_write();

int gamecount;
int enemy_timer = 199;
int block_enemy_timer = 307;
int block_10_enemy_timer = 1009;

int main(){
    hgevent *event;
    int wid = HgWOpen(400, 400, WINDOWSIZE, WINDOWSIZE);
    HgWSetWidth(wid, 1);
    HgWSetColor(wid, HG_BLACK);
    doubleLayer Game_layer;
    int game_layer;
    int layer1;
    int game_state = 0;
    int title_draw = 0;
    int back_draw = 0;
    int pause_flag = 0;
    int player_img;
    int enemy_img;
    int shot_img;
    int screen_etc_img;
    int how_to_play_img;
    Game_layer = HgWAddDoubleLayer(wid);
    layer1 = HgWAddLayer(wid);
    HgSetEventMask(HG_KEY_DOWN | HG_KEY_UP);
    initialize_cursor();
    
    for(;;){
        event = HgEventNonBlocking();
        switch(game_state){
        case 0:
            if(title_draw == 0){
                draw_title(wid);
                title_draw = 1;
            }
            game_layer = HgLSwitch(&Game_layer);
            HgLClear(game_layer);
            move_cursor(event);
            draw_cursor(game_layer);
            game_state = move_title(game_state, event, wid);
            break;
        case 1:
            HgWClear(wid);
            high_score_write();
            gamecount = 0;
            initialize_player();
            initialize_player_shot();
            initialize_enemy();
            initialize_block();
            initialize_effect();
            initialize_player_shot_bar();
            initialize_rest_shot();
            initialize_score_board();
            initialize_text_board();
            enemy_timer = 199;
            block_enemy_timer = 307;
            block_10_enemy_timer = 1009;
            player_img = HgImageLoad("./img/player.png");
            enemy_img = HgImageLoad("./img/enemy.png");
            shot_img = HgImageLoad("./img/shot.png");
            screen_etc_img = HgImageLoad("./img/screen_etc.png");
            draw_game_screen(wid);
            game_state = 2;
            break;
        case 2:
            game_layer = HgLSwitch(&Game_layer);
            HgLClear(game_layer);

            count_delete_block();
            game_speed();
        
            set_enemy();
            set_block_enemy();
            set_block_10_enemy();

            move_player_shot_bar();
            move_player(event);
            move_enemy();
            move_block_enemy();
            move_block_10_enemy();
            move_player_shot();
            move_effect();
            move_text_board();
        
            colision_ditection();
            game_state = check_player_life();

            draw_player(game_layer, player_img);
            draw_enemy(game_layer, enemy_img);
            draw_block_enemy(game_layer, enemy_img);
            draw_block_10_enemy(game_layer, enemy_img);
            draw_block(game_layer, screen_etc_img);
            draw_effect(game_layer, screen_etc_img);
            draw_player_shot_bar(game_layer);
            draw_player_shot(game_layer, shot_img);
            draw_rest_shot(game_layer);
            draw_score_board(game_layer);
            draw_text_board(game_layer);

            timer();
        
        
            if(event != NULL){
                if(event->type == HG_KEY_DOWN){
                    if(event->ch == 'p'){
                        game_state = 3;
                    }
                }
            }
            
            break;
        case 3:
            if(pause_flag == 0){
                HgWSetColor(layer1, HG_BLACK);
                HgWText(layer1, 300, 300, "pause");
                pause_flag = 1;
            }
            if(event != NULL){
                if(event->type == HG_KEY_DOWN){
                    if(event->ch == 'p'){
                        game_state = 2;
                        pause_flag = 0;
                        HgLClear(layer1);
                    }
                }
            }
            break;
        case 4:
            game_layer = HgLSwitch(&Game_layer);
            HgLClear(game_layer);

            count_delete_block();
            game_speed();

            set_enemy();
            set_block_enemy();
            set_block_10_enemy();

            move_enemy();
            move_block_enemy();
            move_block_10_enemy();
            move_effect();

            colision_ditection();
            
            draw_enemy(game_layer, enemy_img);
            draw_block_enemy(game_layer, enemy_img);
            draw_block_10_enemy(game_layer, enemy_img);
            draw_block(game_layer, screen_etc_img);
            draw_effect(game_layer, screen_etc_img);
            draw_player_shot_bar(game_layer);
            draw_rest_shot(game_layer);
            draw_score_board(game_layer);
            draw_text_board(game_layer);
            timer();
            if(back_draw < 100){
                draw_gameover(layer1);
                back_draw++;
            }else if(back_draw == 100){
                if(event != NULL){
                    if(event->type == HG_KEY_DOWN){
                        game_state = 5;
                        back_draw = 0;
                        HgLClear(layer1);
                    }
                }
            }
            break;
        case 5:
            gamecount = 0;
            HgImageUnload(player_img);
            HgImageUnload(enemy_img);
            HgImageUnload(shot_img);
            HgImageUnload(screen_etc_img);
            game_state = 6;
            
        case 6:
            gamecount++;
            game_layer = HgLSwitch(&Game_layer);
            HgLClear(game_layer);
            draw_end(game_layer);
            if(gamecount >= 320){
                move_cursor(event);
                draw_cursor(game_layer);
                game_state = move_end(game_state, event, wid);
            }
            break;
        case 7:
            high_score_write();
            initialize_cursor();
            HgWClear(wid);
            title_draw = 0;
            game_state = 0;
            break;
        case 8:
            how_to_play_img = HgImageLoad("./img/how_to_play.png");
            draw_how_to_play(wid, how_to_play_img);
            if(event != NULL){
                if(event->type == HG_KEY_DOWN){
                    if(event->ch == 13){
                        title_draw = 0;
                        HgWClear(wid);
                        HgImageUnload(how_to_play_img);
                        game_state = 0;
                    }
                }
            }
            break;
        default:
            break;
        }

        if(game_state == 100){
            high_score_write();
            break;
        }
        
    }
    HgCloseAll();
    return 0;
    
}

int move_title(int game_state, hgevent *event, int wid){
    if(cursor.title_flag == 0){
        cursor.title_flag = 1;
    }
    if(cursor.end_flag == 1){
        cursor.end_flag = 0;
    }
    if(event != NULL){
        if(event->type == HG_KEY_DOWN){
            if(event->ch == 13){
                if(cursor.ty == 200){
                    HgWClear(wid);
                    game_state = 1;
                }else if(cursor.ty == 180){
                    HgWClear(wid);
                    game_state = 8;
                }else if(cursor.ty == 160){
                    HgWClear(wid);
                    game_state = 100;
                }
            }
        }
    }
    return game_state;
}

void draw_title(int wid){
    int title_img = HgImageLoad("./img/title.png");
    HgWImageDrawRect(wid, 0, 0, 640, 640, title_img, 0, 0, 640, 640);
    HgWSetColor(wid, HG_WHITE);
    HgWText(wid, 280, 200, "START");
    HgWText(wid, 280, 180, "HOW TO PLAY");
    HgWText(wid, 280, 160, "EXIT");
    HgImageUnload(title_img);
}

void draw_how_to_play(int wid, int img){
    HgWImageDrawRect(wid, 0, 0, WINDOWSIZE, WINDOWSIZE, img, 0, 0, WINDOWSIZE, WINDOWSIZE);
}

void draw_game_screen(int wid){
    int etc_img = HgImageLoad("./img/screen_etc.png");
    int back_img = HgImageLoad("./img/back_screen.png");
    for(int i=0; i<13; i++){
        HgWImageDrawRect(wid, 0, 150+i*40, 20, 20, etc_img, 0, 288, 32, 32);
        HgWImageDrawRect(wid, WINDOWSIZE-20, 150+i*40, 20, 20, etc_img, 32, 288, 32, 32);
    }
    for(int i=0; i<13; i++){
        HgWImageDrawRect(wid, 0, 150+i*40+20, 20, 20, etc_img, 64, 288, 32, 32);
        HgWImageDrawRect(wid, WINDOWSIZE-20, 150+i*40+20, 20, 20, etc_img, 96, 288, 32, 32);
    }
    HgWImageDrawRect(wid, 0, 130, 20, 20, etc_img, 128, 288, 32, 32);
    HgWImageDrawRect(wid, 0, 110, 20, 20, etc_img, 64, 288, 32, 32);
    HgWImageDrawRect(wid, WINDOWSIZE-20, 130, 20, 20, etc_img, 128, 288, 32, 32); 
    HgWImageDrawRect(wid, WINDOWSIZE-20, 110, 20, 20, etc_img, 96, 288, 32, 32);

    HgWImageDrawRect(wid, 20, 130, 600, 510, back_img, 0, 0, 600, 490);
    
    HgWSetFillColor(wid, HG_DGRAY);
    HgWBoxFill(wid, 0, 0, WINDOWSIZE, 110, 0);
    HgWSetFillColor(wid, HG_BLACK);
    HgWBoxFill(wid, 20, 110, 600, 20, 0);
    HgWBoxFill(wid, 5, 5, WINDOWSIZE-10, 100, 1);
    HgWSetWidth(wid, 3);
    HgWSetColor(wid, HG_WHITE);
    HgWText(wid, WINDOWSIZE-440, WINDOWSIZE-20, "Score:");
    HgWText(wid, WINDOWSIZE-280, WINDOWSIZE-20, "High Score:");
    HgWBox(wid, (WINDOWSIZE)/2, 10, (WINDOWSIZE-20)/2, 90);//coment
    HgWBox(wid, (WINDOWSIZE)/2+5, 15, (WINDOWSIZE-20)/4-25, 80);
    HgWBox(wid, (WINDOWSIZE)/2+(WINDOWSIZE-20)/4-15, 15, (WINDOWSIZE-20)/4+10, 80);
    HgWText(wid,  (WINDOWSIZE)/2+(WINDOWSIZE-20)/4-5, 70,  "・移動: a/d, ←/→");
    HgWText(wid,  (WINDOWSIZE)/2+(WINDOWSIZE-20)/4-5,  53,  "・ショット: l , ↑");
    HgWText(wid,  (WINDOWSIZE)/2+(WINDOWSIZE-20)/4-5,  37,  "・リロード: k , ↓");
    HgWText(wid,  (WINDOWSIZE)/2+(WINDOWSIZE-20)/4-5, 20 ,  "・ポーズ: p");
    
    
    HgWBox(wid, 20, 65, (WINDOWSIZE-20)/2-35, 20);//shot_bar
    HgWSetFillColor(wid, HG_BLUE);
    HgWBoxFill(wid, 280, 67, 13, 16, 0);
    HgWBoxFill(wid, 22, 67, 12, 16, 0);
    HgWBoxFill(wid, 150, 67, 13, 16, 0);
    HgWSetFillColor(wid, HG_RED);
    HgWBoxFill(wid, 80, 67, 13, 16, 0);
    HgWBoxFill(wid, 210, 67, 13, 16, 0);

    HgWSetFillColor(wid, HG_WHITE);
    for(int j=0; j<8; j++){
        HgWBox(wid, 75+j*25, 20, 25, 20);//rest_shot
        HgWBoxFill(wid, 78.5+j*25, 24, 18, 12, 0);
    }
    HgWText(wid,20 ,20,"残弾数");
    HgImageUnload(etc_img);
    HgImageUnload(back_img);
}

void initialize_cursor(){
    cursor.tx = 260;
    cursor.ty = 200;
    cursor.ex = 260;
    cursor.ey = 200;
    cursor.title_flag = 1;
    cursor.end_flag = 0;
}

void move_cursor(hgevent *event){
    if(event != NULL){
        if(event->type == HG_KEY_DOWN){
            if(event->ch == 'w' || event->ch == HG_U_ARROW){
                if(cursor.title_flag == 1){
                    if(cursor.ty == 200){
                        cursor.ty = 160;
                    }else if(cursor.ty == 180){
                        cursor.ty = 200;
                    }else if(cursor.ty == 160){
                        cursor.ty = 180;
                    }
                }else if(cursor.end_flag == 1){
                    if(cursor.ey == 200){
                        cursor.ey = 160;
                    }else if(cursor.ey == 180){
                        cursor.ey = 200;
                    }else if(cursor.ey == 160){
                        cursor.ey = 180;
                    }
                }
            }
            if(event->ch == 's' || event->ch == HG_D_ARROW){
                if(cursor.title_flag == 1){
                    if(cursor.ty == 200){
                        cursor.ty = 180;
                    }else if(cursor.ty == 180){
                        cursor.ty = 160;
                    }else if(cursor.ty == 160){
                        cursor.ty = 200;
                    }
                }else if(cursor.end_flag == 1){
                    if(cursor.ey == 200){
                        cursor.ey = 180;
                    }else if(cursor.ey == 180){
                        cursor.ey = 160;
                    }else if(cursor.ey == 160){
                        cursor.ey = 200;
                    }
                }
            }
        }
    }
}

void draw_cursor(int layers){
    if(cursor.title_flag == 1){
        HgWSetColor(layers, HG_WHITE);
        HgWText(layers, cursor.tx, cursor.ty, "＞");
    }else if(cursor.end_flag == 1){
        HgWSetColor(layers, HG_WHITE);
        HgWText(layers, cursor.ex, cursor.ey, "＞");
    }
}

void initialize_player(){
    
    player.x = 300;
    player.y = 150;
    player.speed = 10;
    player.life = 1;
    player.direction = 0;
    player.move_flag = 0;
    player.left_flag = 0;
    player.right_flag = 0;
    player.reload_flag = 0;
    player.draw_normal = -1;
    player.draw_right = -1;
    player.draw_left = -1;
    player.draw_shot = -1;
    player.draw_reload = -1;
    player.shot_direction = 0;
    player.revival_block = 0;
    for(int i=0; i<BLOCKS; i++){
        player.player_block_length[i] = 0;
    }
    
}

void initialize_player_shot(){
    player_shot.x = 10;
    player_shot.y = 20;
    player_shot.right_shot_speed_x = 0;
    player_shot.left_shot_speed_x = 0;
    player_shot.shot_speed_y = 0;
    player_shot.initialize_flag = 0;
    player_shot.move_flag = 0;
    player_shot.move_type = 0;
    player_shot.draw_flag = -1;
    player_shot.range = 10;
    player_shot.reflect_flag = 1;
    
}

void initialize_enemy(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        enemy[i].x = 0;
        enemy[i].y = 670;
        enemy[i].speed = 0;
        enemy[i].initialize_flag = 0;
        enemy[i].move_flag = 0;
        enemy[i].life = 1;
        enemy[i].range = 10;
        enemy[i].draw_enemy_flag = 0;
        block_enemy[i].x = 0;
        block_enemy[i].y = 670;
        block_enemy[i].speed = 0;
        block_enemy[i].initialize_flag = 0;
        block_enemy[i].move_flag = 0;
        block_enemy[i].life = 1;
        block_enemy[i].range = 10;
        block_enemy[i].draw_enemy_flag = 0;
        block_10_enemy[i].x = 0;
        block_10_enemy[i].y = 670;
        block_10_enemy[i].speed = 0;
        block_10_enemy[i].initialize_flag = 0;
        block_10_enemy[i].move_flag = 0;
        block_10_enemy[i].life = 1;
        block_10_enemy[i].range = 10;
        block_10_enemy[i].draw_enemy_flag = 0;
    }
    
}

void initialize_block(){
    for(int i=0; i<BLOCKS; i++){
        block[i].x = 20+i*20;
        block[i].y = 130;
        block[i].initialize_flag = 0;
        block[i].delete_flag = 0;
        block[i].on_player_flag = 0;
    }
}

void initialize_effect(){
    for(int i=0; i<EFFECTS; i++){
        effect[i].ex = 0;
        effect[i].ey = 140;
        effect[i].sx1 = -1;
        effect[i].sy1 = -1;
        effect[i].sx2 = -1;
        effect[i].sy2 = -1;
        effect[i].sx3 = -1;
        effect[i].sy3 = -1;
        effect[i].explosion_flag = -1;
        effect[i].score_effect = -1;
    }
}

void initialize_player_shot_bar(){
    player_shot_bar.x = 40;
    player_shot_bar.y =  65;
    player_shot_bar.dx = 10;
    player_shot_bar.dy = 0;
    player_shot_bar.speed = 2;
    player_shot_bar.move_flag = 0;
    player_shot_bar.shot_type = 0;
}

void initialize_rest_shot(){
    rest_shot.x = 78.5 + 7*25;
    rest_shot.y = 24;
    rest_shot.shot_count = 1;
    rest_shot.shot_flag = 0;
    rest_shot.display_rest_shot = 0;
}

void initialize_score_board(){
    score.x = WINDOWSIZE-390;
    score.y = WINDOWSIZE-20;
    score.hx = WINDOWSIZE-200;
    score.hy = WINDOWSIZE-20;
    score.score_flag = 0;
    score.count_score = 0;
    score.speed_change_flag = 0;
    FILE *fp;
    fp = fopen("./high_score.txt", "r");
    fscanf(fp, "%d\n", &score.high_score);
    fclose(fp);
}

void initialize_text_board(){
    text.x = (WINDOWSIZE)/2 + 15;
    text.enemy_text = -1;
    text.shot_text = -1;
    text.reload_text = -1;
    for(int i=0; i<4; i++){
        text.y[i] = 65 - i*20;
        text.text_state[i] = -1;
    }
}
    
    

void move_player(hgevent *event){
    if(event != NULL){
        if(event->type == HG_KEY_DOWN){
            if(event->ch == 'a' || event->ch == HG_L_ARROW){
                player.draw_normal = -1;
                player.draw_shot = -1;
                player.draw_left++;
                if(player.draw_left == 17){
                    player.draw_left = 0;
                }
                if(player.x > LEFT_LIMIT){
                    if(player.move_flag == 0 && player.left_flag == 0){
                        player.x -= player.speed;
                        player.direction = 1;  
                    }
                    if(player.right_flag == 1){
                        player.right_flag = 0;
                        player.direction = 1;
                    }
                }else if(player.x <= LEFT_LIMIT){
                    if(player.move_flag == 0 && player.left_flag == 0){
                        player.x = LEFT_LIMIT-10;
                        player.direction = 1;
                    }
                    if(player.right_flag == 1){
                        player.right_flag = 0;
                        player.direction = 1;
                    }
                }
            }
            if(event->ch == 'd' || event->ch == HG_R_ARROW){
                player.draw_normal = -1;
                player.draw_shot = -1;
                player.draw_right++;
                if(player.draw_right == 17){
                    player.draw_right = 0;
                }
                if(player.x < RIGHT_LIMIT){
                    if(player.move_flag == 0 && player.right_flag == 0){
                        player.x += player.speed;
                        player.direction = 0;
                    }
                    if(player.left_flag == 1){
                        player.left_flag = 0;
                        player.direction = 0;
                    }
                }else if(player.x >= RIGHT_LIMIT){
                    if(player.move_flag == 0 && player.right_flag == 0){
                        player.x = RIGHT_LIMIT+10;
                        player.direction = 0;
                    }
                    if(player.left_flag == 1){
                        player.left_flag = 0;
                        player.direction = 0;
                    }
                }
            }
            if(player_shot.move_flag == 0){
                if(event->ch == 'l' || event->ch == HG_U_ARROW){
                    player.draw_shot = 0;
                    if(player.move_flag == 0){
                        player.move_flag = 1;
                        if(player_shot_bar.move_flag == 0){
                            player_shot_bar.x = 40;
                        }
                        if(rest_shot.display_rest_shot < 8){
                            player_shot_bar.move_flag = 1;
                        }else{
                            text.shot_text = 3;
                        }
                    }
                }
            }
            if(player_shot.move_flag == 0){
                if(event->ch == 'k' || event->ch == HG_D_ARROW){
                    player.draw_right = -1;
                    player.draw_left = -1;
                    player.draw_shot = -1;
                    if(player.move_flag == 0){
                        rest_shot.shot_count = 1;
                        rest_shot.shot_flag = 0;
                        rest_shot.display_rest_shot = 0;
                        player.reload_flag = 1;
                        text.reload_text = 0;
                    }
                }
            }
        }else{
            player.draw_right = -1;
            player.draw_left = -1;
        }
        
        if(event->type == HG_KEY_UP){
            if(player_shot_bar.move_flag == 1){
                if(player_shot.move_flag == 0){
                    if(event->ch == 'l' || event->ch == HG_U_ARROW){
                        player.draw_right = -1;
                        player.draw_left = -1;
                        player.draw_shot = 1;
                        player.shot_direction = player.direction;
                    
                        if(rest_shot.display_rest_shot < 9){
                            rest_shot.display_rest_shot++;
                        }
                        player_shot_bar.move_flag = 0;
                        set_player_shot(player_shot_bar.shot_type);
                    }
                }
            }
            if(event->ch == 'l' || event->ch == HG_U_ARROW){
                player.move_flag = 0;
            }
        }
        
    }else{
        if(player.draw_right == -1 &&  player.draw_left == -1){
            player.draw_normal++;
        }
    }
    if(player.reload_flag == 1){
        player.move_flag = 1;
        player.draw_reload++;
        if(player.draw_reload == 32){
            text.reload_text = 1;
            player.draw_reload = -1;
            player.reload_flag = 0;
            player.move_flag = 0;
        }
    }
    if(player.draw_normal == 32){
        player.draw_normal = 0;
    }
    
}

void set_player_shot(int shot_type){
    if(player_shot.move_flag == 0){
        if(rest_shot.shot_flag == 0){
            player_shot.move_type = shot_type;
            player_shot.move_flag = 1;
        }
    }
}        

void move_player_shot(){
    
    if(player_shot.move_flag == 1){
        switch(player_shot_bar.shot_type){
        case 0: case 1:// 0: normal_shot 1: trough_shot
            if(player_shot.initialize_flag == 0){
                player_shot.x = player.x;
                player_shot.y = player.y;
                player_shot.right_shot_speed_x = 10;
                player_shot.left_shot_speed_x = -10;
                player_shot.shot_speed_y = 10;
                player_shot.initialize_flag = 1;
                player_shot.draw_flag = -1;
                text.shot_text = player_shot_bar.shot_type;
            }else{
                if(player_shot.x < 610 && player_shot.x >20 && player_shot.y < 660 && rest_shot.shot_flag == 0){
                    if(player.shot_direction == 0){
                        player_shot.y += player_shot.shot_speed_y;
                        player_shot.x += player_shot.right_shot_speed_x;
                        player_shot.draw_flag++;
                    }else{
                        player_shot.y += player_shot.shot_speed_y;
                        player_shot.x += player_shot.left_shot_speed_x;
                        player_shot.draw_flag++;
                            
                    }
                    if(player_shot.draw_flag == 18){
                        player_shot.draw_flag = 0;
                    }
                }else{
                    if(rest_shot.shot_count < 8){
                        rest_shot.shot_count++;
                    }else if(rest_shot.shot_count == 8){
                        rest_shot.shot_flag = 1;
                    }
                        
                    player_shot.move_flag = 0;
                    player_shot.initialize_flag = 0;
                    player_shot.draw_flag = -1;
                }
            }
            break;
        case 2://refrect_shot
            if(player_shot.initialize_flag == 0){
                player_shot.x = player.x;
                player_shot.y = player.y;
                player_shot.right_shot_speed_x = 10;
                player_shot.left_shot_speed_x = -10;
                player_shot.shot_speed_y = 10;
                player_shot.initialize_flag = 1;
                player_shot.reflect_flag = 1;
                player_shot.draw_flag = -1;
                text.shot_text = player_shot_bar.shot_type;
            }else{
                if(player_shot.x < 620 && player_shot.x > 10 && player_shot.y < 660 && rest_shot.shot_flag == 0){
                    switch(player.shot_direction){
                    case 0:

                        if(player_shot.reflect_flag == 1){
                            if(player_shot.x >= 610){
                                player_shot.reflect_flag = 0;
                                player_shot.right_shot_speed_x *= (-1.0);
                            }
                        }

                        player_shot.x += player_shot.right_shot_speed_x;
                        player_shot.y += player_shot.shot_speed_y;
                        player_shot.draw_flag++;
                        if(player_shot.draw_flag == 18){
                            player_shot.draw_flag = 0;
                        }
                        break;
                            
                    case 1:
                        if(player_shot.reflect_flag == 1){
                            if(player_shot.x <= 20){
                                player_shot.reflect_flag = 0;
                                player_shot.left_shot_speed_x *= (-1.0);
                            }
                        }
                        player_shot.x += player_shot.left_shot_speed_x;
                        player_shot.y += player_shot.shot_speed_y;
                        player_shot.draw_flag++;
                        if(player_shot.draw_flag == 18){
                            player_shot.draw_flag = 0;
                        }
                        break;

                    default:
                        break;
                    }

                        
                }else{
                    if(rest_shot.shot_count < 8){
                        rest_shot.shot_count++;
                    }else if(rest_shot.shot_count == 8){
                        rest_shot.shot_flag = 1;
                    }
                    player_shot.move_flag = 0;
                    player_shot.initialize_flag = 0;
                    player_shot.draw_flag = -1;
                }
            }
            break;
        default:
            break;
            
        }
    }
}

void set_enemy(){
    if(gamecount%enemy_timer == enemy_timer-1){
        for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
            if(enemy[i].move_flag == 0){
                enemy[i].move_flag = 1;
                enemy[i].life = 1;
                break;
            }
        }
    }
}

void set_block_enemy(){
    if(gamecount%block_enemy_timer == block_enemy_timer-1){
        for(int i=0; i<BLOCKS; i++){
            if(block[i].delete_flag == 1){
                for(int j=0; i<DISPLAY_ENEMY_MAX; i++){
                    if(block_enemy[j].move_flag == 0){
                        block_enemy[j].move_flag = 1;
                        block_enemy[j].life = 1;
                        break;
                    }
                }
            }
        }
    }
}

void set_block_10_enemy(){
    if(score.count_score >=5000 && gamecount%block_10_enemy_timer == block_10_enemy_timer-1){
        if(count_delete_block() > 20){
            for(int j=0; j<DISPLAY_ENEMY_MAX; j++){
                if(block_10_enemy[j].move_flag == 0){
                    block_10_enemy[j].move_flag = 1;
                    block_10_enemy[j].life = 1;
                    break;
                    
                }
            }
        }
    }
}


void move_enemy(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(enemy[i].move_flag == 1){
            if(enemy[i].initialize_flag == 0){
                srandom(tv.tv_sec + tv.tv_usec);
                enemy[i].x = 20+((random()%30)*20);
                enemy[i].speed = (random()%3) * 1/2 ;
                enemy[i].y = 670;
                enemy[i].initialize_flag = 1;
                enemy[i].draw_enemy_flag = 0;
            }else{
                if(enemy[i].y >= 150){
                    enemy[i].y-= 2 + enemy[i].speed;
                    enemy[i].draw_enemy_flag++;
                    if(enemy[i].draw_enemy_flag == 64){
                        enemy[i].draw_enemy_flag = 0;
                    }
                }else{
                    enemy[i].move_flag = 0;
                    enemy[i].initialize_flag = 0;
                }
            }
        }
    }
}

void move_block_enemy(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_enemy[i].move_flag == 1){
            if(block_enemy[i].initialize_flag == 0){
                srandom(tv.tv_sec + tv.tv_usec);
                block_enemy[i].x = 20+((random()%30)*20);
                block_enemy[i].y = 670;
                block_enemy[i].speed = (random()%3) * 1/2 ;
                block_enemy[i].initialize_flag = 1;
                block_enemy[i].draw_enemy_flag = 0;
            }else{
                if(block_enemy[i].y >= 150){
                    block_enemy[i].y -= 2 + block_enemy[i].speed;
                    block_enemy[i].draw_enemy_flag++;
                    if(block_enemy[i].draw_enemy_flag == 64){
                        block_enemy[i].draw_enemy_flag = 0;
                    }
                }else{
                    block_enemy[i].move_flag = 0;
                    block_enemy[i].initialize_flag = 0;
                }
            }
        }
    }
}

void move_block_10_enemy(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_10_enemy[i].move_flag == 1){
            if(block_10_enemy[i].initialize_flag == 0){
                srandom(tv.tv_sec + tv.tv_usec);
                block_10_enemy[i].x = 20+((random()%30)*20);
                block_10_enemy[i].y = 670;
                block_10_enemy[i].speed = (random()%3) * 1/2;
                block_10_enemy[i].initialize_flag = 1;
                block_10_enemy[i].draw_enemy_flag = 0;
            }else{
                if(block_10_enemy[i].y >=  150){
                    block_10_enemy[i].y -= 2 + block_10_enemy[i].speed;
                    block_10_enemy[i].draw_enemy_flag++;
                    if(block_10_enemy[i].draw_enemy_flag == 64){
                        block_10_enemy[i].draw_enemy_flag = 0;
                    }
                }else{
                    block_10_enemy[i].move_flag = 0;
                    block_10_enemy[i].initialize_flag = 0;
                }
            }
        }
    }
}

void move_player_shot_bar(){
    if(player_shot_bar.move_flag == 1){
        if(player_shot_bar.x < 30 || player_shot_bar.x > 280){
            player_shot_bar.dx *= (-1.0); 
        }
        player_shot_bar.x += player_shot_bar.dx;
    }else{
        if((player_shot_bar.x >= 280 && player_shot_bar.x <= 290) || (player_shot_bar.x >= 20 && player_shot_bar.x <= 30) || (player_shot_bar.x >= 150 && player_shot_bar.x <= 160)){
            player_shot_bar.shot_type = 1;
        }else if((player_shot_bar.x >= 80 && player_shot_bar.x <= 90) || (player_shot_bar.x >= 210 && player_shot_bar.x <= 220)){
            player_shot_bar.shot_type = 2;
        }else{
            player_shot_bar.shot_type = 0;
        }
    }
    if(player_shot.move_flag == 0){
        player_shot_bar.shot_type = -1;
    }
}

void move_effect(){
    for(int i=0; i<EFFECTS; i++){
        if(effect[i].explosion_flag >= 0){
            effect[i].explosion_flag++;
            if(effect[i].explosion_flag == 18){
                effect[i].explosion_flag = -1;
                effect[i].ex = 0;
            }
        }
        if(effect[i].score_effect >= 0){
            effect[i].score_effect++;
            if(effect[i].score_effect == 18){
                effect[i].score_effect = -1;
                effect[i].sx1 = -1;
                effect[i].sy1 = -1;
                effect[i].sx2 = -1;
                effect[i].sy2 = -1;
                effect[i].sx3 = -1;
                effect[i].sy3 = -1;
            }
        }
    }
}

void move_text_board(){
    for(int i=0; i<4; i++){
        if(text.text_state[i] == -1){
            if(text.enemy_text != -1){
                if(text.enemy_text == 0){
                    text.text_state[i] = 0;//敵を倒した
                    text.enemy_text = -1;
                    break;
                }
            }
            if(text.shot_text != -1){
                if(text.shot_text == 0){
                    text.text_state[i] = 1;//通常弾
                    text.shot_text = -1;
                    break;
                }else if(text.shot_text == 1){
                    text.text_state[i] = 2;//貫通弾
                    text.shot_text = -1;
                    break;
                }else if(text.shot_text == 2){
                    text.text_state[i] = 3;//反射弾
                    text.shot_text = -1;
                    break;
                }else if(text.shot_text == 3){
                    text.text_state[i] = 4;//弾がありません
                    text.shot_text = -1;
                    break;
                }
            }
            if(text.reload_text != -1){
                if(text.reload_text == 0){
                    text.text_state[i] = 5;//リロ-ド中...
                    text.reload_text = -1;
                    break;
                }else if(text.reload_text == 1){
                    text.text_state[i] = 6;//リロ-ド完了
                    text.reload_text = -1;
                    break;
                }
            }
        }
    }
    if(text.text_state[3] != -1){
        for(int j=1; j<4; j++){
            text.text_state[j-1] = text.text_state[j];
        }
        text.text_state[3] = -1;
    }
}

void draw_player(int layers, int img){
    if(player.draw_shot == -1){
        if(player.draw_reload == -1){
            if(player.direction == 1){
                if(player.draw_left == -1){
                    if(player.draw_normal != -1){
                        if(player.draw_normal >= 0 && player.draw_normal < 17){
                            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 0, 128, 20, 32);
                        }else if(player.draw_normal >= 17 && player.draw_normal < 37){
                            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 20, 128, 20, 32);
                        }
                    }
                }else if(player.draw_left >= 0 && player.draw_left < 5){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 20, 64, 20, 32);
                }else if(player.draw_left >= 5 && player.draw_left < 9){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 0, 64, 20, 32);
                }else if(player.draw_left >= 9 && player.draw_left < 13){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 40, 64, 20, 32);
                }else if(player.draw_left >= 13 && player.draw_left < 17){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 0, 64, 20, 32);
                }
                
            }else if(player.direction == 0){
                if(player.draw_right == -1){
                    if(player.draw_normal != -1){
                        if(player.draw_normal >= 0 && player.draw_normal < 17){
                            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 0, 96, 20, 32);
                        }else if(player.draw_normal >= 17 && player.draw_normal < 33){
                            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 20, 96, 20, 32);
                        }
                    }
                }else if(player.draw_right >= 0 && player.draw_right < 5){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 20, 32, 20, 32);
                }else if(player.draw_right >= 5 && player.draw_right < 9){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 0, 32, 20, 32);
                }else if(player.draw_right >= 9 && player.draw_right < 13){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 40, 32, 20, 32);
                }else if(player.draw_right >= 13 && player.draw_right < 17){
                    HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 0, 32, 20, 32);
                }
            }
        }else if(player.draw_reload >= 0 && player.draw_reload < 17){
            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 80, 128, 20, 32);
        }else if(player.draw_reload >= 17 && player.draw_reload < 33){
            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 80, 96, 20, 32);
        }
    }else if(player.draw_shot == 0){
        if(player.direction == 1){
            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 60, 64, 20, 32);
        }else if(player.direction == 0){
            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 60, 32, 20, 32);
        }
    }else if(player.draw_shot == 1){
        if(player.direction == 1){
            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 80, 64, 20, 32);
        }else if(player.direction == 0){
            HgWImageDrawRect(layers, player.x, player.y, 20, 32, img, 80, 32, 20, 32);
        }
    }
}

void draw_player_shot(int layers, int img){
    if(player_shot.move_flag == 1 && player_shot.initialize_flag == 1){
        if(rest_shot.shot_flag == 0){
            if(player_shot_bar.shot_type == 0){
                if(player.shot_direction == 1){
                    if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 0, 64, 32, 32);
                    }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 32, 64, 32, 32);
                    }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 64, 64, 32, 32);
                    }
                }else if(player.shot_direction == 0){
                    if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 96, 64, 32, 32);
                    }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 128, 64, 32, 32);
                    }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 160, 64, 32, 32);
                    }
                }
            }else if(player_shot_bar.shot_type == 1){
                if(player.shot_direction == 1){
                    if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 0, 0, 32, 32);
                    }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 32, 0, 32, 32);
                    }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 64, 0, 32, 32);
                    }
                }else if(player.shot_direction == 0){
                    if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 96, 0, 32, 32);
                    }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 128, 0, 32, 32);
                    }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                        HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 160, 0, 32, 32);
                    }
                }
            }else if(player_shot_bar.shot_type == 2){
                if(player.shot_direction == 1){
                    if(player_shot.reflect_flag == 1){
                        if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 0, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 32, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 64, 32, 32, 32);
                        }
                    }else if(player_shot.reflect_flag == 0){
                        if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 96, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 128, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 160, 32, 32, 32);
                        }
                    }
                        
                }else if(player.shot_direction == 0){
                    if(player_shot.reflect_flag == 1){
                        if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 96, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 128, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 160, 32, 32, 32);
                        }
                    }else if(player_shot.reflect_flag == 0){
                        if(player_shot.draw_flag >= 0 && player_shot.draw_flag < 6){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 0, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 6 && player_shot.draw_flag < 12){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 32, 32, 32, 32);
                        }else if(player_shot.draw_flag >= 12 && player_shot.draw_flag < 18){
                            HgWImageDrawRect(layers, player_shot.x, player_shot.y, 10, 10, img, 64, 32, 32, 32);
                        }
                    }
                }
            }
        }
    }
}

void draw_enemy(int layers, int img){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(enemy[i].move_flag == 1 && enemy[i].initialize_flag == 1){
            if(enemy[i].draw_enemy_flag >= 0 && enemy[i].draw_enemy_flag < 9){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 0, 128, 32, 32);
            }else if(enemy[i].draw_enemy_flag >= 9 && enemy[i].draw_enemy_flag < 17){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 32, 128, 32, 32);
            }else if(enemy[i].draw_enemy_flag >= 17 && enemy[i].draw_enemy_flag < 25){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 64, 128, 32, 32);
            }else if(enemy[i].draw_enemy_flag >= 25 && enemy[i].draw_enemy_flag < 33){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 96, 128, 32, 32);
            }else if(enemy[i].draw_enemy_flag >= 33 && enemy[i].draw_enemy_flag < 41){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 128, 128, 32, 32);
            }else if(enemy[i].draw_enemy_flag >= 41 && enemy[i].draw_enemy_flag < 49){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 0, 128, 32, 32);
            }else if(enemy[i].draw_enemy_flag >= 49 && enemy[i].draw_enemy_flag < 57){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 32, 96, 32, 32);
            }else if(enemy[i].draw_enemy_flag >= 57 && enemy[i].draw_enemy_flag < 65){
                HgWImageDrawRect(layers, enemy[i].x, enemy[i].y, 20, 20, img, 64, 96, 32, 32);
            }
        }
    }
}

void draw_block_enemy(int layers, int img){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_enemy[i].move_flag == 1 && block_enemy[i].initialize_flag == 1){
            if(block_enemy[i].draw_enemy_flag >= 0 && block_enemy[i].draw_enemy_flag < 9){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 96, 96, 32, 32);
            }else if(block_enemy[i].draw_enemy_flag >= 9 && block_enemy[i].draw_enemy_flag < 17){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 128, 96, 32, 32);
            }else if(block_enemy[i].draw_enemy_flag >= 17 && block_enemy[i].draw_enemy_flag < 25){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 0, 64, 32, 32);
            }else if(block_enemy[i].draw_enemy_flag >= 25 && block_enemy[i].draw_enemy_flag < 33){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 32, 64, 32, 32);
            }else if(block_enemy[i].draw_enemy_flag >= 33 && block_enemy[i].draw_enemy_flag < 41){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 64, 64, 32, 32);
            }else if(block_enemy[i].draw_enemy_flag >= 41 && block_enemy[i].draw_enemy_flag < 49){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 96, 64, 32, 32);
            }else if(block_enemy[i].draw_enemy_flag >= 49 && block_enemy[i].draw_enemy_flag < 57){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 128, 64, 32, 32);
            }else if(block_enemy[i].draw_enemy_flag >= 57 && block_enemy[i].draw_enemy_flag < 65){
                HgWImageDrawRect(layers, block_enemy[i].x, block_enemy[i].y, 20, 20, img, 0, 32, 32, 32);
            }
        }
    }
}

void draw_block_10_enemy(int layers, int img){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_10_enemy[i].move_flag == 1 && block_10_enemy[i].initialize_flag == 1){
            if(block_10_enemy[i].draw_enemy_flag >= 0 && block_10_enemy[i].draw_enemy_flag < 9){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 32, 32, 32, 32);
            }else if(block_10_enemy[i].draw_enemy_flag >= 9 && block_10_enemy[i].draw_enemy_flag < 17){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 64, 32, 32, 32);
            }else if(block_10_enemy[i].draw_enemy_flag >= 17 && block_10_enemy[i].draw_enemy_flag < 25){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 96, 32, 32, 32);
            }else if(block_10_enemy[i].draw_enemy_flag >= 25 && block_10_enemy[i].draw_enemy_flag < 33){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 128, 32, 32, 32);
            }else if(block_10_enemy[i].draw_enemy_flag >= 33 && block_10_enemy[i].draw_enemy_flag < 41){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 0, 0, 32, 32);
            }else if(block_10_enemy[i].draw_enemy_flag >= 41 && block_10_enemy[i].draw_enemy_flag < 49){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 32, 0, 32, 32);
            }else if(block_10_enemy[i].draw_enemy_flag >= 49 && block_10_enemy[i].draw_enemy_flag < 57){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 64, 0, 32, 32);
            }else if(block_10_enemy[i].draw_enemy_flag >= 57 && block_10_enemy[i].draw_enemy_flag < 65){
                HgWImageDrawRect(layers, block_10_enemy[i].x, block_10_enemy[i].y, 20, 20, img, 96, 0, 32, 32);
            }
        }
    }
}


void draw_block(int layers, int img){
    for(int i=0; i<BLOCKS; i++){
        if(block[i].delete_flag == 0){
            HgWImageDrawRect(layers, block[i].x, block[i].y, 20, 20, img, 128, 288, 32, 32);
        }
    }
}

void draw_effect(int layers, int img){
    for(int i=0; i<EFFECTS; i++){
        if(effect[i].explosion_flag != -1){
            if(effect[i].explosion_flag >= 0 && effect[i].explosion_flag < 6){
                HgWImageDrawRect(layers, effect[i].ex, effect[i].ey, 20, 20, img, 160, 288, 32, 32);
            }else if(effect[i].explosion_flag >= 6 && effect[i].explosion_flag < 12){
                HgWImageDrawRect(layers, effect[i].ex, effect[i].ey, 20, 20, img, 192, 288, 32, 32);
            }else if(effect[i].explosion_flag >= 12 && effect[i].explosion_flag < 18){
                HgWImageDrawRect(layers, effect[i].ex, effect[i].ey, 20, 20, img, 224, 288, 32, 32);
            }
        }
        if(effect[i].score_effect != -1){
            if(effect[i].score_effect >= 0 && effect[i].score_effect < 18){
                if(effect[i].sx1 != -1 && effect[i].sy1 != -1){
                    if(effect[i].sy1 > WINDOWSIZE - 110){
                        HgWSetColor(layers, HG_WHITE);
                        HgWText(layers, effect[i].sx1, effect[i].sy1, "1000");
                    }else if(effect[i].sy1 <= WINDOWSIZE - 110 && effect[i].sy1 > WINDOWSIZE - 210){
                        HgWSetColor(layers, HG_GREEN);
                        HgWText(layers, effect[i].sx1, effect[i].sy1, "500");
                    }else if(effect[i].sy1 <= WINDOWSIZE - 210 && effect[i].sy1 > WINDOWSIZE - 370){
                        HgWSetColor(layers, HG_BLUE);
                        HgWText(layers, effect[i].sx1, effect[i].sy1, "100");
                    }else{
                        HgWSetColor(layers, HG_RED);
                        HgWText(layers, effect[i].sx1, effect[i].sy1, "50");
                    }
                }
                if(effect[i].sx2 != -1 && effect[i].sy2 != -1){
                    if(effect[i].sy2 > WINDOWSIZE - 110){
                        HgWSetColor(layers, HG_WHITE);
                        HgWText(layers, effect[i].sx2, effect[i].sy2, "1000");
                    }else if(effect[i].sy2 <= WINDOWSIZE - 110 && effect[i].sy2 > WINDOWSIZE - 210){
                        HgWSetColor(layers, HG_GREEN);
                        HgWText(layers, effect[i].sx2, effect[i].sy2, "500");
                    }else if(effect[i].sy2 <= WINDOWSIZE - 210 && effect[i].sy2 > WINDOWSIZE - 370){
                        HgWSetColor(layers, HG_BLUE);
                        HgWText(layers, effect[i].sx2, effect[i].sy2, "100");
                    }else{
                        HgWSetColor(layers, HG_RED);
                        HgWText(layers, effect[i].sx2, effect[i].sy2, "50");
                    }
                }
                if(effect[i].sx3 != -1 && effect[i].sy3 != -1){
                    if(effect[i].sy3 > WINDOWSIZE - 110){
                        HgWSetColor(layers, HG_WHITE);
                        HgWText(layers, effect[i].sx3, effect[i].sy3, "1000");
                    }else if(effect[i].sy3 <= WINDOWSIZE - 110 && effect[i].sy3 > WINDOWSIZE - 210){
                        HgWSetColor(layers, HG_GREEN);
                        HgWText(layers, effect[i].sx3, effect[i].sy3, "500");
                    }else if(effect[i].sy3 <= WINDOWSIZE - 210 && effect[i].sy3 > WINDOWSIZE - 370){
                        HgWSetColor(layers, HG_BLUE);
                        HgWText(layers, effect[i].sx3, effect[i].sy3, "100");
                    }else{
                        HgWSetColor(layers, HG_RED);
                        HgWText(layers, effect[i].sx3, effect[i].sy3, "50");
                    }
                }
                
            }
        }
    }
    
    
}  

void draw_player_shot_bar(int layers){
    if(player_shot_bar.move_flag == 0){
        HgWSetFillColor(layers, HG_GREEN);
        HgWBoxFill(layers, player_shot_bar.x, player_shot_bar.y+1,  4, 18, 0);
    }else{
        HgWSetFillColor(layers, HG_WHITE);
        HgWBoxFill(layers, player_shot_bar.x, player_shot_bar.y+1,  4, 18, 0);
    }
    
}

void draw_rest_shot(int layers){
    int i;
    HgWSetFillColor(layers, HG_BLACK);
    if(rest_shot.display_rest_shot == 8){
        for(i=8; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }else if(rest_shot.display_rest_shot == 7){
        for(i=7; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }else if(rest_shot.display_rest_shot == 6){
        for(i=6; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }else if(rest_shot.display_rest_shot == 5){
        for(i=5; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }else if(rest_shot.display_rest_shot == 4){
        for(i=4; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }else if(rest_shot.display_rest_shot == 3){
        for(i=3; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }else if(rest_shot.display_rest_shot == 2){
        for(i=2; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }else if(rest_shot.display_rest_shot == 1){
        for(i=1; i>0; i--){
            HgWBoxFill(layers, 278.5-i*25, 24, 18, 12, 0);
        }
    }
    
}

void draw_score_board(int layers){
    HgWSetColor(layers, HG_WHITE);
    HgWText(layers, score.x, score.y, "%d", score.count_score);
    HgWText(layers, score.hx, score.hy, "%d", score.high_score);
}

void draw_text_board(int layers){
    HgWSetColor(layers, HG_WHITE);
    for(int i=0; i<4; i++){
        if(text.text_state[i] != -1){
            if(text.text_state[i] == 0){
                HgWText(layers, text.x, text.y[i], "・敵を倒した");
            }
            if(text.text_state[i] == 1){
                HgWText(layers, text.x, text.y[i], "・通常ショット");
            }
            if(text.text_state[i] == 2){
                HgWText(layers, text.x, text.y[i], "・貫通ショット");
            }
            if(text.text_state[i] == 3){
                HgWText(layers, text.x, text.y[i], "・反射ショット");
            }
            if(text.text_state[i] == 4){
                HgWText(layers, text.x, text.y[i], "・弾がありません");
            }
            if(text.text_state[i] == 5){
                HgWText(layers, text.x, text.y[i], "・リロード中...");
            }
            if(text.text_state[i] == 6){
                HgWText(layers, text.x, text.y[i], "・リロード完了");
            }
        }
    }
}

int pythago_theorem(double x, double x2, double y, double y2, double range, double range2){
    double x3 = x - x2;
    double y3 = y - y2;
    double range3 = range + range2;
    if(x3*x3 + y3*y3 < range3*range3){
        return 1;
    }else{
        return 0;
    }  
}

void colision_ditection(){
    colision_enemy_block();
    colision_block_enemy_block();
    colision_block_10_enemy_block();
    colision_player_enemy();
    colision_player_block_enemy();
    colision_player_block_10_enemy();
    colision_player_shot_enemy();
    colision_player_shot_block_enemy();
    colision_player_shot_block_10_enemy();
    
    check_enemy_life();
    check_block_enemy_life();
    check_block_10_enemy_life();
    check_player_block();
}


void colision_player_enemy(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(enemy[i].move_flag == 1 && enemy[i].initialize_flag == 1 && player.life > 0){
            if((player.y+30 >= enemy[i].y) && ((enemy[i].x+5 >= player.x && enemy[i].x+5 <= player.x+20) || (enemy[i].x+15 >= player.x && enemy[i].x+15 <= player.x+20))){
                player.life = 0;
                enemy[i].move_flag = 0;
                enemy[i].initialize_flag = 0;
            }
        }
    }       
}

void colision_player_block_enemy(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_enemy[i].move_flag == 1 && block_enemy[i].initialize_flag == 1 && player.life > 0){
            if((player.y+30 >= block_enemy[i].y) && ((block_enemy[i].x+5 >= player.x && block_enemy[i].x+5 <= player.x+20) || (block_enemy[i].x+15 >= player.x && block_enemy[i].x+15 <= player.x+20))){
                player.life = 0;
                block_enemy[i].move_flag = 0;
                block_enemy[i].initialize_flag = 0;
            }
        }
    }
}

void colision_player_block_10_enemy(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_10_enemy[i].move_flag == 1 && block_10_enemy[i].initialize_flag == 1 && player.life > 0){
            if((player.y+30 >= block_10_enemy[i].y) && ((block_10_enemy[i].x+5 >= player.x && block_10_enemy[i].x+5 <= player.x+20) || (block_10_enemy[i].x+15 >= player.x && block_10_enemy[i].x+15 <= player.x+20))){
                player.life = 0;
                block_10_enemy[i].move_flag = 0;
                block_10_enemy[i].initialize_flag = 0;
            }
        }
    }
}



void colision_player_shot_enemy(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(enemy[i].move_flag == 1 && enemy[i].initialize_flag == 1){
            if(player_shot.move_flag == 1 && player_shot.initialize_flag == 1){
                if(player_shot.x < 610 && player_shot.x >20 && player_shot.y < 660){
                    if(pythago_theorem(enemy[i].x+10, player_shot.x+5, enemy[i].y+10, player_shot.y+5, enemy[i].range, player_shot.range) == 1){
                        enemy[i].life = 0;
                        score.score_flag = 1;
                        if(player_shot_bar.shot_type != 1){
                            player_shot.move_flag = 0;
                            player_shot.initialize_flag = 0;
                            if(rest_shot.shot_count < 8){
                                rest_shot.shot_count++;
                            }else if(rest_shot.shot_count == 8){
                                rest_shot.shot_flag = 1;
                                
                            }
                        } 
                    }
                }
            }
        }
    }
}

void colision_player_shot_block_enemy(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_enemy[i].move_flag == 1 && block_enemy[i].initialize_flag == 1){
            if(player_shot.move_flag == 1 && player_shot.initialize_flag == 1){
                if(player_shot.x < 610 && player_shot.x >20 && player_shot.y < 660){
                    if(pythago_theorem(block_enemy[i].x+10, player_shot.x+5, block_enemy[i].y+10, player_shot.y+5, block_enemy[i].range, player_shot.range) == 1){    
                        block_enemy[i].life = 0;
                        score.score_flag = 1;
                        if(player_shot_bar.shot_type != 1){
                            player_shot.move_flag = 0;
                            player_shot.initialize_flag = 0;
                            if(rest_shot.shot_count < 8){
                                rest_shot.shot_count++;
                            }else if(rest_shot.shot_count == 8){
                                rest_shot.shot_flag = 1;
                            }
                        }
                            
                        for(int k=0; k<BLOCKS; k++){
                            if(block[k].delete_flag == 1){
                                player.player_block_length[k] = (player.x+10 - block[k].x+10) * (player.x+10 - block[k].x+10);   
                            }
                        }

                        for(int l=0; l<BLOCKS; l++){
                            if(block[l].delete_flag == 1){
                                player.revival_block = min_player_block_length(player.player_block_length);
                            }  
                        }

                        block[player.revival_block].delete_flag = 0;                            
                           
                    }
                }
            }
        }
    }
}

void colision_player_shot_block_10_enemy(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_10_enemy[i].move_flag == 1 && block_10_enemy[i].initialize_flag == 1){
            if(player_shot.move_flag == 1 && player_shot.initialize_flag == 1){
                if(player_shot.x < 610 && player_shot.x >20 && player_shot.y < 660){
                    if(pythago_theorem(block_10_enemy[i].x+10, player_shot.x+5, block_10_enemy[i].y+10, player_shot.y+5, block_10_enemy[i].range, player_shot.range) == 1){
                        for(int k=0; k<DISPLAY_ENEMY_MAX; k++){
                            enemy[k].life = 0;
                            block_enemy[k].life = 0;
                            block_10_enemy[k].life = 0;
                        }
                        score.score_flag = 1;

                            
                        if(player_shot_bar.shot_type != 1){
                            player_shot.move_flag = 0;
                            player_shot.initialize_flag = 0;
                            if(rest_shot.shot_count < 8){
                                rest_shot.shot_count++;
                            }else if(rest_shot.shot_count == 8){
                                rest_shot.shot_flag = 1;
                            }
                        }

                        for(int l=0; l<10; l++){
                            for(int m=0; m<BLOCKS; m++){
                                if(block[m].delete_flag == 1){
                                    player.player_block_length[m] = (player.x+10 - block[m].x+10) * (player.x+10 - block[m].x+10);
                                }
                            }

                            for(int n=0; n<BLOCKS; n++){
                                if(block[n].delete_flag == 1){
                                    player.revival_block = min_player_block_length(player.player_block_length);
                                }
                            }
                            
                            block[player.revival_block].delete_flag = 0;
                        }
                    }
                }
            }
        }
    }
}



void colision_enemy_block(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(enemy[i].move_flag == 1 && enemy[i].initialize_flag == 1){
            for(int j=0; j<BLOCKS; j++){
                if(block[j].delete_flag == 0){
                    if(enemy[i].y < block[j].y+20){
                        if(enemy[i].x == block[j].x){
                            enemy[i].move_flag = 0;
                            enemy[i].initialize_flag = 0;
                            block[j].delete_flag = 1;
                            effect[j].explosion_flag = 0;
                            effect[j].ex = block[j].x;
                        }
                    }
                }
            }
        }
    }
}

void colision_block_enemy_block(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_enemy[i].move_flag == 1 && block_enemy[i].initialize_flag == 1){
            for(int j=0; j<BLOCKS; j++){
                if(block[j].delete_flag == 0){
                    if(block_enemy[i].y < block[j].y+20){
                        if(block_enemy[i].x == block[j].x){
                            block_enemy[i].move_flag = 0;
                            block_enemy[i].initialize_flag = 0;
                            block[j].delete_flag = 1;
                            effect[j].explosion_flag = 0;
                            effect[j].ex = block[j].x;
                        }
                    }
                }
            }
        }
        
    }
}

void colision_block_10_enemy_block(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_10_enemy[i].move_flag == 1 && block_10_enemy[i].initialize_flag == 1){
            for(int j=0; j<BLOCKS; j++){
                if(block[j].delete_flag == 0){
                    if(block_10_enemy[i].y < block[j].y+20){
                        if(block_10_enemy[i].x == block[j].x){
                            block_10_enemy[i].move_flag = 0;
                            block_10_enemy[i].initialize_flag = 0;
                            block[j].delete_flag = 1;
                            effect[j].explosion_flag = 0;
                            effect[j].ex = block[j].x;
                        }
                    }
                }
            }
        }
    }
}

void check_enemy_life(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(enemy[i].life == 0){
            if(enemy[i].move_flag == 1 && enemy[i].initialize_flag == 1){
                effect[i].score_effect = 0;
                effect[i].sx1 = enemy[i].x+10;
                effect[i].sy1 = enemy[i].y+10;
                text.enemy_text = 0;
                enemy[i].move_flag = 0;
                enemy[i].initialize_flag = 0;
                if(score.score_flag == 1){
                    if(enemy[i].y > WINDOWSIZE - 110){
                        score.count_score += 1000;
                    }else if(enemy[i].y <= WINDOWSIZE - 110 && enemy[i].y > WINDOWSIZE - 210){
                        score.count_score += 500;
                    }else if(enemy[i].y <= WINDOWSIZE - 210 && enemy[i].y > WINDOWSIZE - 370){
                        score.count_score += 100;
                    }else{
                        score.count_score += 50;
                    }
                    score.score_flag = 0;
                }
            }
        }
    }
}

void check_block_enemy_life(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_enemy[i].life == 0){
            if(block_enemy[i].move_flag == 1 && block_enemy[i].initialize_flag == 1){
                effect[i].score_effect = 0;
                effect[i].sx2 = block_enemy[i].x+10;
                effect[i].sy2 = block_enemy[i].y+10;
                text.enemy_text = 0;
                block_enemy[i].move_flag = 0;
                block_enemy[i].initialize_flag = 0;
                if(score.score_flag == 1){
                    if(block_enemy[i].y > WINDOWSIZE - 110){
                        score.count_score += 1000;
                    }else if(block_enemy[i].y <= WINDOWSIZE - 110 && block_enemy[i].y > WINDOWSIZE - 210){
                        score.count_score += 500;
                    }else if(block_enemy[i].y <= WINDOWSIZE - 210 && block_enemy[i].y > WINDOWSIZE - 370){
                        score.count_score += 100;
                    }else{
                        score.count_score += 50;
                    }
                    score.score_flag = 0;
                }
            }
        }
    }
}

void check_block_10_enemy_life(){
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        if(block_10_enemy[i].life == 0){
            if(block_10_enemy[i].move_flag == 1 && block_10_enemy[i].initialize_flag == 1){
                effect[i].score_effect = 0;
                effect[i].sx3 = block_10_enemy[i].x+10;
                effect[i].sy3 = block_10_enemy[i].y+10;
                text.enemy_text = 0;
                block_10_enemy[i].move_flag = 0;
                block_10_enemy[i].initialize_flag = 0;
                if(block_10_enemy[i].y > WINDOWSIZE - 110){
                    score.count_score += 1000;
                }else if(block_10_enemy[i].y <= WINDOWSIZE - 110 && block_10_enemy[i].y > WINDOWSIZE - 210){
                    score.count_score += 500;
                }else if(block_10_enemy[i].y <= WINDOWSIZE - 210 && block_10_enemy[i].y > WINDOWSIZE - 370){
                    score.count_score += 100;
                }else{
                    score.count_score += 50;
                }
                score.score_flag = 0;
            }
        }
    }
}

void check_player_block(){
    for(int i=0; i<BLOCKS; i++){
        if(player.x+5 > block[i].x  &&  player.x+5 < block[i].x+20){
            block[i].on_player_flag = 1;
        }else if(player.x + 15 > block[i].x  &&  player.x + 15 < block[i].x+20){
            block[i].on_player_flag = 1;
        }else{
            block[i].on_player_flag = 0;
        }
        
        if(block[i].delete_flag == 1){
            if(player.move_flag == 0){
                if(block[i].on_player_flag == 1){
                    if(player.direction == 0){
                        player.x  = block[i].x - 20;
                        player.right_flag = 1;
                    }if(player.direction == 1){
                        player.x = block[i].x + 20;
                        player.left_flag = 1;
                    }
                        
                }
                    
            }
        }else{
            player.right_flag = 0;
            player.left_flag = 0;
        }
    }
}

int check_player_life(){
    int game_state = 2;
    if(player.life == 0){
        game_state = 4;
        return game_state;
    }
    return game_state;
}

void timer(){
    gamecount++;
    if(score.speed_change_flag == 1){
        enemy_timer = 197;
        block_enemy_timer = 293;
    }else if(score.speed_change_flag == 2){
        enemy_timer = 181;
        block_enemy_timer = 269;
    }else if(score.speed_change_flag == 3){
        enemy_timer = 151;
        block_enemy_timer = 239;
    }else if(score.speed_change_flag == 4){
        enemy_timer = 109;
        block_enemy_timer = 211;
    }else if(score.speed_change_flag == 5){
        enemy_timer = 97;
        block_enemy_timer = 193;
    }else if(score.speed_change_flag == 6){
        enemy_timer = 79;
        block_enemy_timer = 181;
    }else if(score.speed_change_flag == 7){
        enemy_timer = 61;
        block_enemy_timer = 163;
    }else if(score.speed_change_flag == 8){
        enemy_timer = 53;
        block_enemy_timer = 149;
    }else if(score.speed_change_flag == 9){
        enemy_timer = 41;
        block_enemy_timer = 127;
    }
    
}

void draw_timer(int layers){
    HgWText(layers, (WINDOWSIZE)/2+20, 600, "%d", gamecount);
}

int min_player_block_length(double player_block_length[]){
    double min = 0;
    int block_num = 0;
    int tmp = 0;

    for(int i=0; i<BLOCKS; i++){
        if(block[i].delete_flag == 1){
            min = player_block_length[i];
            tmp = i;
            break;
        }
    }

    for(int j=0; j<BLOCKS; j++){
        if(block[j].delete_flag == 1){
            if(min >= player_block_length[j]){
                min = player_block_length[j];
                block_num = j;
            }
        }
    }

    if(block_num == 0){
        return tmp;
    }else{
        return block_num;
    }
}

int count_delete_block(){
    int count = 30;
    for(int i=0; i<BLOCKS; i++){
        if(block[i].delete_flag == 1){
            count++;
        }else if(block[i].delete_flag == 0) {
            count--;
        }
    }

    return count;
}

void change_speed(){
    player.speed += 1;
    player_shot.right_shot_speed_x += 2;
    player_shot.left_shot_speed_x -= 2;
    player_shot.shot_speed_y += 2;
    
    for(int i=0; i<DISPLAY_ENEMY_MAX; i++){
        enemy[i].speed -= 1/2;
        block_enemy[i].speed -= 1/2;
        block_10_enemy[i].speed -= 1/2;
    }
}

void game_speed(){
    if(score.speed_change_flag == 0){
        if(score.count_score >= 1000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 1){
        if(score.count_score >= 3000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 2){
        if(score.count_score >= 5000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 3){
        if(score.count_score >= 7000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 4){
        if(score.count_score >= 10000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 5){
        if(score.count_score >= 15000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 6){
        if(score.count_score >= 20000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 7){
        if(score.count_score >= 30000){
            change_speed();
            score.speed_change_flag ++;
        }
    }else if(score.speed_change_flag == 8){
        if(score.count_score >= 50000){
            change_speed();
            score.speed_change_flag ++;
        }
    }
}

void draw_gameover(int layers){
    HgWSetFont(layers, HG_TB, 40);
    HgWSetColor(layers, HG_WHITE);
    HgWText(layers, 200, 350, "GAME  OVER");
    HgWSetFont(layers, HG_TB, 18);
    HgWText(layers, 270, 320, "press  any  key");
    if(gamecount%5 == 0){
        HgWSetFillColor(layers, HgRGBA(0, 0, 0, 0.1));
        HgWBoxFill(layers, 0, 0, 640, 640, 0);
    }
}

void high_score_write(){
    int high_score = 0;
    FILE *fp;
    fp = fopen("./high_score.txt", "r");
    fscanf(fp, "%d\n", &high_score);
    fclose(fp);
    if(high_score < score.count_score){
        fp = fopen("./high_score.txt", "w");
        fprintf(fp, "%d\n", score.count_score);
        fclose(fp);
    }
}

int move_end(int game_state, hgevent *event, int wid){
    if(cursor.title_flag == 1){
        cursor.title_flag = 0;
    }
    if(cursor.end_flag == 0){
        cursor.end_flag = 1;
    }
    
    if(event != NULL){
        if(event->type == HG_KEY_DOWN){
            if(event->ch == 13){
                if(cursor.ey == 200){
                    HgWClear(wid);
                    game_state = 1;
                }else if(cursor.ey == 180){
                    HgWClear(wid);
                    game_state = 7;
                }else if(cursor.ey == 160){
                    HgWClear(wid);
                    game_state = 100;
                }
            }
        }
    }
    return game_state;
}

void draw_end(int layers){
    HgWSetFillColor(layers, HG_BLACK);
    HgWBoxFill(layers, 0, 0, 640, 640, 0);
    HgWSetColor(layers, HG_WHITE);
    HgWSetFont(layers, HG_TB, 20);
    if(gamecount >= 60) HgWText(layers, 100, 500, "Your Score :");
    if(gamecount >= 120) HgWText(layers, 220, 500, "%d", score.count_score);
    if(gamecount >= 180) HgWText(layers, 100, 450, "High Score :");
    if(gamecount >= 240) HgWText(layers, 220, 450, "%d", score.high_score);
    if(score.count_score > score.high_score){
        HgWSetFont(layers, HG_TB, 30);
        if(gamecount >= 300) HgWText(layers, 150, 300, "You win the High Score !!");
    }
    if(gamecount >= 320){
        HgWSetFont(layers, HG_TB, 15);
        HgWText(layers, 280, 200, "RETRY");
        HgWText(layers, 280, 180, "TITLE");
        HgWText(layers, 280, 160, "EXIT");
    }
}
