#include "raylib.h"
#include "raylib_text_font_binder.hpp"

using namespace raylib_extend;

int main(void) 
{
    FontFileData ffd{ "simkai.ttf" };

    InitWindow(800, 450, "测试中文显示");
    SetTargetFPS(60);

    TextData data{ "纯是一段这文测试本的 ,", ffd };  // render our needed characters, be cautions here, we also include a space and a comma.

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTextEx(*(data.font()), "这是一段文本, 是的, 测试文本", (Vector2){ 50, 50 }, 32, 0, RED);  /* draw it. */
        EndDrawing();
    }

    return 0;
}
