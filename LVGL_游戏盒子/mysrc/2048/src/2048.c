#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "2048.h"
#include "ui.h"
#include "lvgl/lvgl.h"
#include "lv_font_2048.h"

// 游戏板大小
#define BOARD_SIZE 4

// 游戏难度定义
typedef enum
{
    DIFFICULTY_EASY,   // 简单：更多4生成
    DIFFICULTY_NORMAL, // 普通：标准2048
    DIFFICULTY_HARD    // 困难：更少4生成
} game_difficulty_t;

// 函数声明
static void init_board(void);
static void add_new_tile(void);
static int check_game_over(void);
static int check_win(void);
static int move_tiles(int direction);
static void update_ui(void);
static void touch_event_cb(lv_event_t *e);
static void restart_btn_event_cb(lv_event_t *e);
static void exit_btn_event_cb(lv_event_t *e);
static void difficulty_btn_event_cb(lv_event_t *e); // 难度选择按钮回调
static void create_game_ui(void);
static void load_high_score(void); // 加载最高分
static void save_high_score(void); // 保存最高分
static void update_high_score(void); // 更新最高分

// 游戏状态
typedef enum
{
    GAME_STATE_PLAYING,
    GAME_STATE_WIN,
    GAME_STATE_OVER
} game_state_t;

// 游戏数据结构
typedef struct
{
    int board[BOARD_SIZE][BOARD_SIZE];
    int score;
    int high_score;        // 历史最高分
    game_difficulty_t difficulty; // 游戏难度
    game_state_t state;
} game_2048_t;

// 游戏UI对象
static lv_obj_t *main_screen = NULL;
static lv_obj_t *score_label = NULL;
static lv_obj_t *high_score_label = NULL; // 最高分标签
static lv_obj_t *board_container = NULL;
static lv_obj_t *tiles[BOARD_SIZE][BOARD_SIZE] = {0};
static lv_obj_t *tile_images[BOARD_SIZE][BOARD_SIZE] = {0}; // 瓦片图片对象
static lv_obj_t *game_over_msg = NULL;
static lv_obj_t *restart_btn = NULL;
static lv_obj_t *exit_btn = NULL; // 退出按钮
static lv_obj_t *difficulty_btn = NULL; // 难度选择按钮
static lv_obj_t *difficulty_label = NULL; // 难度显示标签

// 游戏数据
static game_2048_t game = {0};

// 初始化游戏板
static void init_board(void)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            game.board[i][j] = 0;
        }
    }
    game.score = 0;
    game.high_score = 0;
    game.state = GAME_STATE_PLAYING;

    // 默认难度为普通
    game.difficulty = DIFFICULTY_NORMAL;

    // 加载最高分
    load_high_score();

    // 添加两个初始瓦片
    add_new_tile();
    add_new_tile();
}

// 添加新瓦片
static void add_new_tile(void)
{
    int empty_cells[BOARD_SIZE * BOARD_SIZE][2];
    int empty_count = 0;

    // 找出所有空单元格
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (game.board[i][j] == 0)
            {
                empty_cells[empty_count][0] = i;
                empty_cells[empty_count][1] = j;
                empty_count++;
            }
        }
    }

    // 如果有空单元格，随机选择一个并添加瓦片
    if (empty_count > 0)
    {
        int index = rand() % empty_count;
        int row = empty_cells[index][0];
        int col = empty_cells[index][1];

        // 根据难度调整生成4的概率
        int probability = 10; // 默认10%概率生成4
        if (game.difficulty == DIFFICULTY_EASY)
        {
            probability = 20; // 简单模式20%概率生成4
        }
        else if (game.difficulty == DIFFICULTY_HARD)
        {
            probability = 5;  // 困难模式5%概率生成4
        }

        game.board[row][col] = (rand() % 100 < probability) ? 4 : 2;
    }
}

// 检查游戏是否结束
static int check_game_over(void)
{
    // 检查是否有空单元格
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (game.board[i][j] == 0)
            {
                return 0; // 游戏未结束
            }
        }
    }

    // 检查是否有相邻的相同数字
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            int current = game.board[i][j];

            // 检查右侧
            if (j < BOARD_SIZE - 1 && game.board[i][j + 1] == current)
            {
                return 0; // 游戏未结束
            }

            // 检查下方
            if (i < BOARD_SIZE - 1 && game.board[i + 1][j] == current)
            {
                return 0; // 游戏未结束
            }
        }
    }

    return 1; // 游戏结束
}

// 检查是否获胜
static int check_win(void)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (game.board[i][j] == 2048)
            {
                return 1; // 获胜
            }
        }
    }
    return 0; // 未获胜
}

// 移动瓦片
static int move_tiles(int direction)
{
    int moved = 0;

    // 0: 上, 1: 右, 2: 下, 3: 左
    if (direction == 0)
    { // 上
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            for (int i = 1; i < BOARD_SIZE; i++)
            {
                if (game.board[i][j] != 0)
                {
                    int row = i;
                    while (row > 0 && game.board[row - 1][j] == 0)
                    {
                        game.board[row - 1][j] = game.board[row][j];
                        game.board[row][j] = 0;
                        row--;
                        moved = 1;
                    }

                    if (row > 0 && game.board[row - 1][j] == game.board[row][j])
                    {
                        game.board[row - 1][j] *= 2;
                        game.score += game.board[row - 1][j];


                        game.board[row][j] = 0;
                        moved = 1;

                        // 检查是否获胜
                        if (game.board[row - 1][j] == 2048)
                        {
                            game.state = GAME_STATE_WIN;
                        }
                    }
                }
            }
        }
    }
    else if (direction == 1)
    { // 右
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = BOARD_SIZE - 2; j >= 0; j--)
            {
                if (game.board[i][j] != 0)
                {
                    int col = j;
                    while (col < BOARD_SIZE - 1 && game.board[i][col + 1] == 0)
                    {
                        game.board[i][col + 1] = game.board[i][col];
                        game.board[i][col] = 0;
                        col++;
                        moved = 1;
                    }

                    if (col < BOARD_SIZE - 1 && game.board[i][col + 1] == game.board[i][col])
                    {
                        game.board[i][col + 1] *= 2;
                        game.score += game.board[i][col + 1];


                        game.board[i][col] = 0;
                        moved = 1;

                        // 检查是否获胜
                        if (game.board[i][col + 1] == 2048)
                        {
                            game.state = GAME_STATE_WIN;
                        }
                    }
                }
            }
        }
    }
    else if (direction == 2)
    { // 下
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            for (int i = BOARD_SIZE - 2; i >= 0; i--)
            {
                if (game.board[i][j] != 0)
                {
                    int row = i;
                    while (row < BOARD_SIZE - 1 && game.board[row + 1][j] == 0)
                    {
                        game.board[row + 1][j] = game.board[row][j];
                        game.board[row][j] = 0;
                        row++;
                        moved = 1;
                    }

                    if (row < BOARD_SIZE - 1 && game.board[row + 1][j] == game.board[row][j])
                    {
                        game.board[row + 1][j] *= 2;
                        game.score += game.board[row + 1][j];


                        game.board[row][j] = 0;
                        moved = 1;

                        // 检查是否获胜
                        if (game.board[row + 1][j] == 2048)
                        {
                            game.state = GAME_STATE_WIN;
                        }
                    }
                }
            }
        }
    }
    else if (direction == 3)
    { // 左
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 1; j < BOARD_SIZE; j++)
            {
                if (game.board[i][j] != 0)
                {
                    int col = j;
                    while (col > 0 && game.board[i][col - 1] == 0)
                    {
                        game.board[i][col - 1] = game.board[i][col];
                        game.board[i][col] = 0;
                        col--;
                        moved = 1;
                    }

                    if (col > 0 && game.board[i][col - 1] == game.board[i][col])
                    {
                        game.board[i][col - 1] *= 2;
                        game.score += game.board[i][col - 1];


                        game.board[i][col] = 0;
                        moved = 1;

                        // 检查是否获胜
                        if (game.board[i][col - 1] == 2048)
                        {
                            game.state = GAME_STATE_WIN;
                        }
                    }
                }
            }
        }
    }

    // 移动后更新最高分
    if (moved)
    {
        update_high_score();
    }

    return moved;
}

// 更新UI显示
static void update_ui(void)
{
    char score_str[32];
    lv_snprintf(score_str, sizeof(score_str), "得分: %d", game.score);
    lv_label_set_text(score_label, score_str);

    // 更新最高分显示
    char high_score_str[32];
    lv_snprintf(high_score_str, sizeof(high_score_str), "最高分: %d", game.high_score);
    lv_label_set_text(high_score_label, high_score_str);


    // 更新难度显示
    const char *difficulty_text;
    switch (game.difficulty)
    {
        case DIFFICULTY_EASY:
            difficulty_text = "简单";
            break;
        case DIFFICULTY_NORMAL:
            difficulty_text = "普通";
            break;
        case DIFFICULTY_HARD:
            difficulty_text = "困难";
            break;
        default:
            difficulty_text = "普通";
            break;
    }
    lv_label_set_text(difficulty_label, difficulty_text);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            int value = game.board[i][j];
            char img_path[32];

            if (value == 0)
            {
                lv_img_set_src(tile_images[i][j], "A:/wxj/2048/0.bmp"); // 空瓦片
            }
            else
            {
                // 根据值选择对应的图片
                lv_snprintf(img_path, sizeof(img_path), "A:/wxj/2048/%d.bmp", value);
                lv_img_set_src(tile_images[i][j], img_path);
            }
        }
    }

    // 检查游戏状态
    if (game.state == GAME_STATE_WIN)
    {
        lv_obj_clear_flag(game_over_msg, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(game_over_msg, "恭喜你赢了!");
    }
    else if (game.state == GAME_STATE_OVER)
    {
        lv_obj_clear_flag(game_over_msg, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(game_over_msg, "游戏结束!");
    }
    else
    {
        lv_obj_add_flag(game_over_msg, LV_OBJ_FLAG_HIDDEN);
    }
}

// 触摸事件回调
static void touch_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING || code == LV_EVENT_RELEASED)
    {
        static lv_point_t start_point = {0};
        static bool is_pressed = false;

        if (code == LV_EVENT_PRESSED)
        {
            lv_indev_t *indev = lv_indev_get_act();
            if (indev)
            {
                lv_indev_get_point(indev, &start_point);
                is_pressed = true;
            }
        }
        else if (code == LV_EVENT_RELEASED && is_pressed)
        {
            lv_indev_t *indev = lv_indev_get_act();
            lv_point_t end_point = {0};
            if (indev)
            {
                lv_indev_get_point(indev, &end_point);
            }
            lv_point_t vector = {end_point.x - start_point.x, end_point.y - start_point.y};

            // 计算滑动方向
            int moved = 0;
            if (abs(vector.x) > abs(vector.y))
            {
                // 水平滑动
                if (vector.x > 30)
                {
                    moved = move_tiles(1); // 右滑
                }
                else if (vector.x < -30)
                {
                    moved = move_tiles(3); // 左滑
                }
            }
            else
            {
                // 垂直滑动
                if (vector.y > 30)
                {
                    moved = move_tiles(2); // 下滑
                }
                else if (vector.y < -30)
                {
                    moved = move_tiles(0); // 上滑
                }
            }

            if (moved)
            {
                add_new_tile();

                // 检查游戏是否结束
                if (check_game_over())
                {
                    game.state = GAME_STATE_OVER;
                }

                update_ui();
            }

            is_pressed = false;
        }
    }
}

// 重新开始按钮回调
static void restart_btn_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    // 保存最高分
    save_high_score();

    init_board();
    update_ui();
}

// 退出按钮回调
static void exit_btn_event_cb(lv_event_t *e)
{
    // 使用LV_UNUSED宏标记未使用的参数e，避免编译器警告
    LV_UNUSED(e);

    // 保存最高分
    save_high_score();

    // 返回主界面
    extern lv_obj_t *main_screen_obj;
    lv_screen_load(main_screen_obj);
}

// 创建游戏UI
static void create_game_ui(void)
{
    // 创建主屏幕
    main_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(main_screen, lv_color_hex(0xFAF8EF), LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建标题
    lv_obj_t *title = lv_label_create(main_screen);
    lv_obj_set_style_text_font(title, &lv_font_2048, 0);
    lv_label_set_text(title, "2048游戏");
    lv_obj_align(title, LV_ALIGN_TOP_RIGHT, 0, 20);

    // 创建分数标签
    score_label = lv_label_create(main_screen);
    lv_obj_set_style_text_font(score_label, &lv_font_2048, 0);
    lv_obj_align(score_label, LV_ALIGN_TOP_RIGHT, 0, 60);

    // 创建最高分标签
    high_score_label = lv_label_create(main_screen);
    lv_obj_set_style_text_font(high_score_label, &lv_font_2048, 0);
    lv_obj_align(high_score_label, LV_ALIGN_TOP_RIGHT, 0, 100);


    // 创建难度选择按钮
    difficulty_btn = lv_btn_create(main_screen);
    lv_obj_set_size(difficulty_btn, 150, 40);
    lv_obj_align(difficulty_btn, LV_ALIGN_TOP_RIGHT, 0, 140);
    lv_obj_add_event_cb(difficulty_btn, difficulty_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // 创建难度显示标签
    difficulty_label = lv_label_create(difficulty_btn);
    lv_obj_set_style_text_font(difficulty_label, &lv_font_2048, 0);
    lv_label_set_text(difficulty_label, "普通");
    lv_obj_center(difficulty_label);

    // 创建游戏板容器
    board_container = lv_obj_create(main_screen);
    lv_obj_set_size(board_container, 500, 500);
    lv_obj_align(board_container, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(board_container, lv_color_hex(0xBBADA0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(board_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(board_container, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建瓦片
    int tile_size = 100;
    int tile_margin = 15;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            tiles[i][j] = lv_obj_create(board_container);
            lv_obj_set_size(tiles[i][j], tile_size, tile_size);
            lv_obj_align(tiles[i][j], LV_ALIGN_TOP_LEFT,
                         j * (tile_size + tile_margin) + tile_margin,
                         i * (tile_size + tile_margin) + tile_margin);
            lv_obj_set_style_radius(tiles[i][j], 3, LV_PART_MAIN | LV_STATE_DEFAULT);

            // 创建瓦片上的图片
            tile_images[i][j] = lv_img_create(tiles[i][j]);
            lv_obj_center(tile_images[i][j]);
            lv_img_set_src(tile_images[i][j], "A:/wxj/2048/0.bmp"); // 初始为空
            // 禁用图片滚动
            lv_obj_clear_flag(tiles[i][j], LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_center(tile_images[i][j]);
        }
    }

    // 创建游戏结束消息
    game_over_msg = lv_label_create(main_screen);
    lv_obj_set_style_text_font(game_over_msg, &lv_font_2048, 0);
    lv_obj_align(game_over_msg, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_add_flag(game_over_msg, LV_OBJ_FLAG_HIDDEN);

    // 创建重新开始按钮
    restart_btn = lv_btn_create(main_screen);
    lv_obj_set_size(restart_btn, 150, 40);
    lv_obj_align(restart_btn, LV_ALIGN_BOTTOM_MID, -80, -20);
    lv_obj_add_event_cb(restart_btn, restart_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(restart_btn);
    lv_obj_set_style_text_font(btn_label, &lv_font_2048, 0);
    lv_label_set_text(btn_label, "重新开始");
    lv_obj_center(btn_label);

    // 创建退出按钮
    exit_btn = lv_btn_create(main_screen);
    lv_obj_set_size(exit_btn, 150, 40);
    lv_obj_align(exit_btn, LV_ALIGN_BOTTOM_MID, 80, -20);
    lv_obj_add_event_cb(exit_btn, exit_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *exit_btn_label = lv_label_create(exit_btn);
    lv_obj_set_style_text_font(exit_btn_label, &lv_font_2048, 0);
    lv_label_set_text(exit_btn_label, "退出");
    lv_obj_center(exit_btn_label);

    // 添加触摸事件回调
    lv_obj_add_event_cb(main_screen, touch_event_cb, LV_EVENT_ALL, NULL);

    // 加载背景图片
    LV_IMG_DECLARE(bg_img);
    lv_obj_t *bg = lv_img_create(main_screen);
    lv_img_set_src(bg, "A:/wxj/2048/bg.bmp");
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));
    lv_obj_center(bg);
    lv_obj_move_background(bg);
}

// 难度选择按钮回调
static void difficulty_btn_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    // 切换难度
    if (game.difficulty == DIFFICULTY_EASY)
    {
        game.difficulty = DIFFICULTY_NORMAL;
    }
    else if (game.difficulty == DIFFICULTY_NORMAL)
    {
        game.difficulty = DIFFICULTY_HARD;
    }
    else
    {
        game.difficulty = DIFFICULTY_EASY;
    }

    // 更新UI显示
    update_ui();
}


// 加载最高分
static void load_high_score(void)
{
    // 尝试从文件中读取最高分
    FILE *fp = fopen("/wxj/2048/highscore.txt", "r");
    if (fp != NULL)
    {
        fscanf(fp, "%d", &game.high_score);
        fclose(fp);
    }
    else
    {
        // 文件不存在，创建并初始化最高分为0
        fp = fopen("/wxj/2048/highscore.txt", "w");
        if (fp != NULL)
        {
            fprintf(fp, "0");
            fclose(fp);
        }
        game.high_score = 0;
    }
}

// 保存最高分
static void save_high_score(void)
{
    // 保存最高分到文件
    FILE *fp = fopen("/wxj/2048/highscore.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d", game.high_score);
        fclose(fp);
    }
}

// 更新最高分数
static void update_high_score(void)
{
    if (game.score > game.high_score)
    {
        game.high_score = game.score;
        // 保存最高分到文件
        save_high_score();
    }
}

// 游戏入口函数
int game_2048()
{
    // 初始化随机数种子
    srand((unsigned int)time(NULL));

    // 创建游戏UI
    create_game_ui();

    // 初始化游戏板
    init_board();

    // 更新UI
    update_ui();

    // 加载屏幕
    lv_screen_load(main_screen);

    return 0;
}
