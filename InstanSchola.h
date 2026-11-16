#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include "sqlite3.h"
#include "buttons.h"
#pragma comment(lib, "sqlite3.lib")


/*==============================================================================
                        CONTROL VARS
==============================================================================*/

enum screenstatus {
    loading,
	WELCOMESCREEN,
    menuing,
    apping,

};
enum screenstatus currentstate;

enum songs {
    Spectre,
    Diamond_hearts,
    Dreamer,
};


/*=================================================================================================================================================================
                             SAFETY
==================================================================================================================================================================*/                                    
                              
// Safely loads an image and exits gracefully if it fails
static inline Image SafeLoadImage(const char* filePath) {
    Image img = LoadImage(filePath);
    if (img.data == NULL) {
        TraceLog(LOG_ERROR, "CRITICAL ERROR: Could not load image at '%s'", filePath);
        CloseWindow();
        TraceLog(LOG_WARNING, "OPTIONAL ASSET: Could not load icon at '%s'. Using default window frame.", filePath);
        //exit(1); // Force closes the program cleanly
    }
    return img;
}

// Safely loads a texture and exits gracefully if it fails
static inline Texture2D SafeLoadTexture(const char* filePath) {
    Texture2D tex = LoadTexture(filePath);
    if (tex.id == 0) {
        TraceLog(LOG_ERROR, "CRITICAL ERROR: Could not load texture at '%s'", filePath);
        CloseWindow();
        TraceLog(LOG_WARNING, "OPTIONAL ASSET: Could not load icon at '%s'. Using default window frame.", filePath);
        //exit(1); // Force closes the program cleanly
    }
    return tex;
}

// Non-fatal loader for optional window icons
static inline void SafeSetWindowIcon(const char* filePath) {
    Image icon = SafeLoadImage(filePath);

    if (icon.data != NULL) {
        SetWindowIcon(icon);
        UnloadImage(icon); // Always unload CPU memory after sending to window
        TraceLog(LOG_INFO, "Window icon loaded successfully from '%s'", filePath);
    }
    else {
        // Log a warning instead of calling exit(1)
        TraceLog(LOG_WARNING, "OPTIONAL ASSET: Could not load icon at '%s'. Using default window frame.", filePath);
    }
}


/*============================================================================================================
==============================================================================================================
                                APP'S METADATA
==============================================================================================================
=============================================================================================================*/

//screenspace control




/*===============================================================================================================
                                         functions
=================================================================================================================*/
Music bgm;
static inline void SetMusic(enum songs currentsong) {
	if (bgm.ctxData != NULL) {
		UnloadMusicStream(bgm);
	}
    switch (currentsong) {
        case Spectre: {
            bgm = LoadMusicStream("assets/audio/Spectre.mp3");
            break;
        }
        case Diamond_hearts: {			
            bgm = LoadMusicStream("assets/audio/Diamond hearts.mp3");
            break;
        }
        case Dreamer: {			
            bgm = LoadMusicStream("assets/audio/Dreamer.mp3");
            break;
        }
    }
  
	UpdateMusicStream(bgm);

}