#include <raylib.h>
#include "scenes.h"

int main() {
    InitWindow(screenWidth, screenHeight, "Little Red Riding Hood");
    InitAudioDevice();

    SceneManager scene_manager;

    MainMenu main_menu_scene;
    main_menu_scene.SetSceneManager(&scene_manager);

    Level1 level1_scene;
    level1_scene.SetSceneManager(&scene_manager);

    Level2 level2_scene; // Create an instance of Level2
    level2_scene.SetSceneManager(&scene_manager);

    PlayerWin player_win_scene;
    player_win_scene.SetSceneManager(&scene_manager);

    PlayerLose player_lose_scene;
    player_lose_scene.SetSceneManager(&scene_manager);

    scene_manager.RegisterScene(&main_menu_scene, 0);
    scene_manager.RegisterScene(&level1_scene, 1);
    scene_manager.RegisterScene(&level2_scene, 2); // Register Level2 with ID 2
    scene_manager.RegisterScene(&player_win_scene, 3);
    scene_manager.RegisterScene(&player_lose_scene, 4);

    scene_manager.SwitchScene(0);



    while (!WindowShouldClose()) {
        Scene* active_scene = scene_manager.GetActiveScene();

        BeginDrawing();
        ClearBackground(BLACK);
        if (active_scene != nullptr) {
            active_scene->Update();
            active_scene->Draw();
        }
        EndDrawing();
    }

    Scene* active_scene = scene_manager.GetActiveScene();
    if (active_scene != nullptr) {
        active_scene->End();
    }

    ResourceManager::GetInstance()->UnloadAllTextures();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
