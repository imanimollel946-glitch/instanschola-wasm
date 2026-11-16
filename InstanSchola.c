#define _CRT_SECURE_NO_WARNINGS
#include "InstanSchola.h"

int main(void) {   

    printf("SQLite Version: %s\n", sqlite3_libversion());
    printf("Thread Safety Level: %d\n", sqlite3_threadsafe());
  

	
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_WINDOW_HIGHDPI |FLAG_VSYNC_HINT);
    InitWindow(1920, 1051, "Study Learn");

    
    InitAudioDevice();
    HideCursor();
  
   
    Font italic = LoadFont("assets/fonts/italic.ttf");
    GuiSetFont(italic);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    SafeSetWindowIcon("assets/images/titlebar.png");
    Texture2D logo = SafeLoadTexture("assets/images/logo.png");      
    
    // Load background track and begin playback
    static enum songs currentsong = Diamond_hearts;
    SetMusic(currentsong);
    PlayMusicStream(bgm);    
	float sound = 1.0f;
    SetMusicVolume(bgm, sound);

   
   
    while (!WindowShouldClose()) {        
        UpdateMusicStream(bgm);

        Vector2 cursorpos = GetMousePosition();
        Vector2 logopos = { (GetScreenWidth() - logo.width) / 2.0f, (GetScreenHeight() - logo.height) / 2.0f };

        BeginDrawing();
        DrawFPS(10, 10);
        ClearBackground(MAROON);

        switch (currentstate) {

            case loading: {                  
                DrawTextureV(logo, logopos, WHITE);
                DrawTextEx(italic, "Loading Assets", cursorpos, 20.0f, 10.0f, BLUE);
                    if (GuiButton((Rectangle) { 20, 20, 120, 30 }, "Start")) { 
                        currentstate = WELCOMESCREEN;
                    }
                GuiLabel((Rectangle) { 20, 60, 200, 20 }, "Loading Assets");
                break;
            }
            
			case WELCOMESCREEN: {
				DrawTextEx(italic, "Welcome to Study Learn, this is a placeholder for the welcome screen.", cursorpos, 30.0f, 1.0f, VIOLET);
				if (GuiButton((Rectangle) { 20, 20, 120, 30 }, "Continue")) { currentstate=menuing; }
				break;
			}

            case menuing: {
                Vector2 pos = GetMousePosition();
                if (GuiButton((Rectangle) { 20, 20, 120, 30 }, "Play Dreamer"))  { SetMusic(Dreamer); PlayMusicStream(bgm);}

                DrawTextEx(italic, "this is a placeholder for menu", pos, 30.0f, 1.0f, VIOLET);//placeholder for menu;
               
                if (IsKeyPressed(KEY_RIGHT)) { currentstate=apping; }
                if (IsKeyPressed(KEY_LEFT)) { currentstate=loading; }
                break;
            }

            case apping: {              
                if (IsKeyPressed(KEY_LEFT)) { currentstate=menuing; }
                break;//going to start with apping

            }
        }

        DrawCircleV(cursorpos, 5, BLUE);
        EndDrawing();
    }

    // Unload audio assets and close audio hardware
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    UnloadTexture(logo);    
    UnloadFont(italic);
    CloseWindow();


    return 0;
}
