#include <raylib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "scene_manager.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "enemy.hpp"

// ------------------ CONSTANTS ------------------
const int screenWidth = 1280;
const int screenHeight = 720;
const float FPS = 60;



std::string imgName;
int tileCount;
int gridRow, gridCol; // grid dimensions
Vector2 player_start_pos;
int enemy_count;

// Struct for Tile
struct Tile {
    Rectangle location; // location of the tile in tilemap.png
};

// Struct for Player Tile
struct PlayerTile {
    Rectangle player_location; // location of the player tile in the playersprite.png
};

// Struct for Enemy
struct Enemies {
    Rectangle enemy_location; // location of the player tile in the enemy-l1.png, enemy-l2.png, and boss.png
    Vector2 enemy_start_pos;
};

// Dimensions to fill the screen
float tileScreenW;
float tileScreenH;

bool player_on_stairs = false;

//save file is premade
std::ofstream save("save_file.txt");

// ------------------ FUNCTIONS ------------------

void ReadLevelTXT(const std::string& filename, std::vector<std::vector<int>>& grid, Tile* tile, Enemies* enemy) {
    // Read from file and assign values for variables accordingly
    std::ifstream file(filename);
    std::string line;
    
    
    while (std::getline(file, line)) { // traverses through Level1.txt line by line and stores them in string line
        std::stringstream lineStream(line); // parses line into tokens based on whitespace
        std::string label, value;
        
        lineStream >> label; // extracts next token (IMAGE_NAME, TILE_COUNT, GRID) into string label
        
        // IMAGE NAME
        if (label == "IMAGE_NAME") {
            lineStream >> imgName;
            //std::cout << "IMAGE_NAME: " << imgName << std::endl;
        }
        
        // TILE COUNT
        if (label == "TILE_COUNT") {
            lineStream >> tileCount;
            //std::cout << "TILE_COUNT: " << tileCount << std::endl;
        }
        
        // TILES
        if (label.substr(0, 5) == "TILE0") {
            //std::cout << label.substr(5) << std::endl;
            std::string strIndex = label.substr(5);
            int i = std::stoi(strIndex) - 1;
            //std::cout << i << std::endl;
            
            while (lineStream >> value) {
                tile[i].location.x = std::stoi(value);
                lineStream >> tile[i].location.y >> tile[i].location.width >> tile[i].location.height;
                break;
            }
            //std::cout << label << ": {" << tile[i].location.x << ", " << tile[i].location.y << ", " << tile[i].location.width << ", " << tile[i].location.height << "}" << std::endl;
        }
        
        // GRID
        if (label == "GRID") {
            lineStream >> gridRow >> gridCol;
            //std::cout << "GRID_DIMENSIONS: {" << gridRow << ", " << gridCol << "}" << std::endl;
            
            // Initialize 2d array for grid
            grid = std::vector<std::vector<int>>(gridRow, std::vector<int>(gridCol));
            
            // Read grid line by line
            for (int i = 0; i < gridRow; i++) {
                std::getline(file, line);
                std::stringstream gridLineStream(line);
                
                for (int j = 0; j < gridCol; j++) {
                    gridLineStream >> grid[i][j];
                    //std::cout << grid[i][j] << " ";
                }
                //std::cout << std::endl;
            }
        }
        
        // PLAYER_START_POS
        if (label == "PLAYER_START_POS") {
            lineStream >> player_start_pos.x >> player_start_pos.y;
            //std::cout << "PLAYER_START_POS: {" << player_start_pos.x << ", " << player_start_pos.y << "}" << std::endl;
        }
        
        // ENEMY COUNT
        if (label == "ENEMY_COUNT") {
            lineStream >> enemy_count;
            //std::cout << "ENEMY_COUNT: " << enemy_count << std::endl;
        }
        
        // ENEMY_START_POS
        if (label.substr(0, 6) == "ENEMY0") {
            std::string strIndex = label.substr(5);
            int i = std::stoi(strIndex) - 1;
            
            while (lineStream >> value) {
                enemy[i].enemy_start_pos.x = std::stoi(value);
                lineStream >> enemy[i].enemy_start_pos.y;
                break;
            }
            //std::cout << label << ": {" << enemy[i].enemy_start_pos.x << ", " << enemy[i].enemy_start_pos.y << "}" << std::endl;
        }
    }
    
    
    file.close();
    
    tileScreenW = screenWidth / gridCol;
    tileScreenH = screenHeight / gridRow;
}


// ------------------ UI LIBRARY ------------------
struct UiLibrary {
    // hot and active widget ids
    int hot = -1;
    int active = -1;

    // button widget
    bool Button(int id, const std::string& text, const Rectangle& bounds, Color originalButtonBG, Color hoverButtonBG, Color clickButtonBG, Color originalButtonTXT, Color clickButtonTXT) {
        bool result = false;

        // text size
        Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), 14, 0);

        // text position - set as center of the button
        Vector2 textPosition = {bounds.x + (bounds.width - textSize.x) / 2, bounds.y + (bounds.height - textSize.y) / 2};

        // Active
        if (id == active) {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if (id == hot) {
                    result = true;
                    active = -1;
                }

                DrawRectangleRec(bounds, originalButtonBG);
                DrawText(text.c_str(), static_cast<int>(textPosition.x), static_cast<int>(textPosition.y), 14, originalButtonTXT);
            }
        }

        // Hot
        if (id == hot) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                active = id;

                DrawRectangleRec(bounds, clickButtonBG);
                DrawText(text.c_str(), static_cast<int>(textPosition.x), static_cast<int>(textPosition.y), 14, clickButtonTXT);
            }
        }

        if (CheckCollisionPointRec(GetMousePosition(), bounds)) {
            hot = id;

            if (id != active) {
                DrawRectangleRec(bounds, hoverButtonBG);
                DrawText(text.c_str(), static_cast<int>(textPosition.x), static_cast<int>(textPosition.y), 14, originalButtonTXT);
            }
        } else {
            if (hot == id) {
                hot = -1;
            }

            if (id != active) {
                DrawRectangleRec(bounds, originalButtonBG);
                DrawText(text.c_str(), static_cast<int>(textPosition.x), static_cast<int>(textPosition.y), 14, originalButtonTXT);
            }
        }

        return result;
    }
};


// ------------------ SCENES ------------------

// ------- MAIN MENU -------

class MainMenu : public Scene {
    UiLibrary ui_library;
    
public:
    void Begin() override {}
    
    void End() override {}
    
    void Update() override {
        // New Game Button: switches to level 1 scene
        if (ui_library.Button(6, "Start New Game", {(screenWidth / 2) - 100, (screenHeight / 2) - 50, 200, 50}, WHITE, RED, YELLOW, BLACK, BLACK)) {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(1);
            }
        }
        // Load Game Button: loads saved game
        if (ui_library.Button(7, "Load Game", {(screenWidth / 2) - 100, (screenHeight / 2) + 20, 200, 50}, WHITE, RED, YELLOW, BLACK, BLACK)) {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(2); // read from save file; if level 1, switch to level 1; if level 2, switch to level 2
            }
        }
        
        // Quit Button: closes the game window
        if (ui_library.Button(8, "Quit", {(screenWidth / 2) - 100, (screenHeight / 2) + 90, 200, 50}, WHITE, RED, YELLOW, BLACK, BLACK)) {
            CloseWindow();
        }
    }
    
    void Draw() override {
        DrawText("Little Red Riding Hood", (screenWidth / 2) - 275, (screenHeight / 2) - 200, 50, RED);
        ui_library.Button(6, "Start New Game", {(screenWidth / 2) - 100, (screenHeight / 2) - 50, 200, 50}, WHITE, RED, YELLOW, BLACK, BLACK);
        ui_library.Button(7, "Load Game", {(screenWidth / 2) - 100, (screenHeight / 2) + 20, 200, 50}, WHITE, RED, YELLOW, BLACK, BLACK);
        ui_library.Button(8, "Quit", {(screenWidth / 2) - 100, (screenHeight / 2) + 90, 200, 50}, WHITE, RED, YELLOW, BLACK, BLACK);
    }
};


// ------- LEVEL 1 -------
class Level1 : public Scene {
        
    Sound level1_bg;

    // Initialize tiles and enemies
    Tile tile[6];
    Enemies enemy[3];
    
    // Define grid based on grid dimensions
    std::vector<std::vector<int>> grid;
    
    // Initialize texture
    Texture tilemap;
    
    // Entities
    std::vector<Entity*> entities;
    
    // Delta Time
    float delta_time;
    
    // Camera
    Camera2D camera;
    
    std::vector<Enemy*> enemies;
    
    Player player{{0, 0}, 20.0f, 200.0f, 100.0f};
    
    Enemy enemy0{{0, 0}, {35, 35}, 200.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy1{{0, 0}, {35, 35}, 200.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy2{{0, 0}, {35, 35}, 200.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    
    
public:
    void Begin() override {
        level1_bg = LoadSound("yoshis_island.mp3");
        PlaySound(level1_bg);
        
        
        // Call ReadFromTXT
        ReadLevelTXT("Level1.txt", grid, tile, enemy);
        
        tilemap = LoadTexture(imgName.c_str());
        
        SetTargetFPS(60);
        
        player.position = {player_start_pos.x, player_start_pos.y};
        enemy0.position = {enemy[0].enemy_start_pos.x, enemy[0].enemy_start_pos.y};
        enemy1.position = {enemy[1].enemy_start_pos.x, enemy[1].enemy_start_pos.y};
        enemy2.position = {enemy[2].enemy_start_pos.x, enemy[2].enemy_start_pos.y};
        
        
        // assign texture
        player.texture = "player.png";
        enemy0.texture = "overworld-enemy.png";
        enemy1.texture = "overworld-enemy.png";
        enemy2.texture = "overworld-enemy.png";
        
        // load textures only once
        player.LoadPlayerTexture();
        enemy0.LoadEnemyTexture();
        enemy1.LoadEnemyTexture();
        enemy2.LoadEnemyTexture();
       
        enemy0.target_player = &player;
        enemy1.target_player = &player;
        enemy2.target_player = &player;
        
        player.enemies.push_back(&enemy0);
        player.enemies.push_back(&enemy1);
        player.enemies.push_back(&enemy2);
        
        entities.push_back(&player);
        entities.push_back(&enemy0);
        entities.push_back(&enemy1);
        entities.push_back(&enemy2);
        

        // DEFINE CAMERA
        camera = { 0 };
        camera.target = player.position;

        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 2.0f;
    }
    
    void End() override {
        UnloadSound(level1_bg);
        UnloadTexture(tilemap);
        UnloadTexture(player.tex);
        UnloadTexture(enemy0.tex);
        UnloadTexture(enemy1.tex);
        UnloadTexture(enemy2.tex);
        //for (auto& entity : entities) {
            //delete entity;
        //}
        //entities.clear();
    }
    
    void Update() override {
        delta_time = GetFrameTime();
        
        for (auto& entity : entities) {
            entity->Update(delta_time);
        }

        if (player.has_lost) {
            std::cout << "Player has died!" << std::endl;
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(4);
            }
        }


        // Update Camera to follow player
        camera.target = player.position;
        
        // Player Collision
        Rectangle player_hitbox = {player.position.x - player.radius, player.position.y - player.radius, player.radius * 2, player.radius * 2}; // define player hitbox
        bool player_colliding_with_wall = false;
        bool player_on_mud = false;
        player_on_stairs = false;

        bool all_enemies_dead = true;
        for (auto& enemy : player.enemies) {
            if (!enemy->is_dead) {
                all_enemies_dead = false;
                break;
            }
        }

        // traverse through grid to check if player and walls are colliding
        for (int i = 0; i < gridRow; i++) {
            for (int j = 0; j < gridCol; j++) {
                if (grid[i][j] == 0 || grid[i][j] == 4) { // walls player should collide with
                    Rectangle wall = {j * tileScreenW, i * tileScreenH, tileScreenW, tileScreenH};
                    if (CheckCollisionRecs(player_hitbox, wall)) {
                        std::cout << "Player collided with the wall!" << std::endl;
                        player_colliding_with_wall = true;
                        break;
                    }
                }
                if (grid[i][j] == 3) { // player steps on mud
                    Rectangle mud = {j * tileScreenW, i * tileScreenH, tileScreenW, tileScreenH};
                    if (CheckCollisionRecs(player_hitbox, mud)) {
                        std::cout << "Player stepped on mud. Player is forcibly slowed down." << std::endl;
                        player.is_on_mud = true;
                        player_on_mud = true;
                        break;
                    }
                }
                if (grid[i][j] == 5) { // player collides with stairs
                    Rectangle stairs = {j * tileScreenW, i * tileScreenH, tileScreenW, tileScreenH};
                    if (CheckCollisionRecs(player_hitbox, stairs) && all_enemies_dead) {
                        std::cout << "Player stepped on stairs. Proceed to Level 2." << std::endl;
                        player_on_stairs = true;
                        break;
                    }
                }
            }
            
            if (player_colliding_with_wall || player_on_mud || player_on_stairs) {
                break;
            }
        }
        
        if (!player_on_mud) {
            player.is_on_mud = false;
            player_on_mud = false;
        }
        
        if (player_colliding_with_wall) {
            if (player.is_dodging) {
                std::cout << "Player collided with the wall while dodging!" << std::endl;
                player.dodge_timer = 0; //would stop player from dodging
                
                Vector2 dodge_direction = Vector2Subtract(player.position, player.previous_position);
                float distance_to_wall = player.radius - 5.0f;
                player.position = Vector2Subtract(player.position, Vector2Scale(Vector2Normalize(dodge_direction), distance_to_wall));
            }
            else {
                player.position = player.previous_position;
            }
        }
        
        if (player_on_stairs) {
            if (GetSceneManager() != nullptr) {
                std::cout << "Player proceeds to level 2." << std::endl;
                GetSceneManager()->SwitchScene(2);
            }
        }
        
        for (auto& entity: entities) {
            Enemy* enemy= dynamic_cast<Enemy*>(entity);
            if (enemy) {
                Rectangle enemy_hitbox = {enemy->position.x, enemy->position.y, enemy->rect_boundaries.x, enemy->rect_boundaries.y};
                bool enemy_colliding_with_wall = false;
                
                // traverse through grid to check if enemies and walls are colliding
                if (!player_on_stairs) {
                    for (int i = 0; i < gridRow; i++) {
                        for (int j = 0; j < gridCol; j++) {
                            if (grid[i][j] == 0 || grid[i][j] == 4) {
                                Rectangle wall = {j * tileScreenW, i * tileScreenH, tileScreenW, tileScreenH};
                                if (CheckCollisionRecs(enemy_hitbox, wall)) {
                                    std::cout << "Enemy collided with the wall!" << std::endl;
                                    enemy_colliding_with_wall = true;
                                    break;
                                }
                            }
                        }
                        
                        if (enemy_colliding_with_wall) {
                            break;
                        }
                    }
                }
                
                if (enemy_colliding_with_wall) {
                    // reset move_timer
                    enemy->move_timer = 0;
                    
                    // update position
                    if (enemy->rand_x == 1) { // left
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({1, 0}, (enemy->speed * delta_time)));
                    }
                    else if (enemy->rand_x == 1) { // right
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({-1, 0}, (enemy->speed * delta_time)));
                    }
                    if (enemy->rand_y == 1) { // up
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({0, 1}, (enemy->speed * delta_time)));
                    }
                    else if (enemy->rand_y == 2) {
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({0, -1}, (enemy->speed * delta_time)));
                    }
                    
                    enemy->velocity = {0, 0};
                }
            }
        }

        //save file?
        if(IsKeyPressed(KEY_R)){
            std::cout << "Player just saved!" << std::endl;
            save.open("save_file.txt");
            save << 1 << "\n";
            save << player.position.x << " " << player.position.y << "\n";
            save << player.hp << "\n";
            save << enemy0.position.x << " " << enemy0.position.y << "\n";
            save << enemy0.hp << "\n";
            save << enemy1.position.x << " " << enemy1.position.y << "\n";
            save << enemy1.hp << "\n";
            save << enemy2.position.x << " " << enemy2.position.y << "\n";
            save << enemy2.hp << "\n";
            save.close();

        }
        
        // ------------ PLAYER, ENEMIES, WALL COLLISIONS ------------
    }
        void Draw() override {
        ClearBackground(BLACK);
        BeginMode2D(camera);
        
        // Draw tiles based on the grid
        if (!player_on_stairs) {
            for (int i = 0; i < gridRow; i++) {
                for (int j = 0; j < gridCol; j++) {
                    Rectangle locTile = {j * tileScreenW, i * tileScreenH, tileScreenW, tileScreenH};
                    int tileIndex = grid[i][j];
                    if (tileIndex < 6) {
                        DrawTexturePro(tilemap, tile[tileIndex].location, locTile, {0, 0}, 0, WHITE);
                    }
                }
            }
        }
        
        // Draw entities
        for (auto& entity : entities) {
            entity->Draw();
        }
        
        EndMode2D();
    }
};

// ------- LEVEL 2 -------
class Level2 : public Scene{

    Sound level2_bg;

    // Initialize tiles and enemies
    Tile tile[9];
    Enemies enemy[6];
    
    // Define grid based on grid dimensions
    std::vector<std::vector<int>> grid;
    
    // Initialize texture
    Texture tilemap;
    
    // Entities
    std::vector<Entity*> entities;
    
    // Delta Time
    float delta_time;
    
    // Camera
    Camera2D camera;
    
    
    std::vector<Enemy*> enemies;
    
    Player player{{0, 0}, 20.0f, 200.0f, 500.0f};
    
    Enemy enemy0{{0, 0}, {35, 35}, 200.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy1{{0, 0}, {35, 35}, 250.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy2{{0, 0}, {35, 35}, 200.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy3{{0, 0}, {35, 35}, 250.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy4{{0, 0}, {35, 35}, 200.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy5{{0, 0}, {35, 35}, 200.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    Enemy enemy6{{0, 0}, {35, 35}, 250.0f, 85.0f, 45.0f, 50.0f, 90.0f, 50.0f};
    


public:
    void Begin() override {
        level2_bg = LoadSound("luigi.mp3");
        PlaySound(level2_bg);

        ReadLevelTXT("Level2.txt", grid, tile, enemy);
        tilemap = LoadTexture(imgName.c_str());
        
        
        SetTargetFPS(60);
        
        std::cout << "level 2" << std::endl;
        
        player.position = {player_start_pos.x, player_start_pos.y};
        enemy0.position = {enemy[0].enemy_start_pos.x, enemy[0].enemy_start_pos.y};
        enemy1.position = {enemy[1].enemy_start_pos.x, enemy[1].enemy_start_pos.y};
        enemy2.position = {enemy[2].enemy_start_pos.x, enemy[2].enemy_start_pos.y};
        enemy3.position = {enemy[3].enemy_start_pos.x, enemy[3].enemy_start_pos.y};
        enemy4.position = {enemy[3].enemy_start_pos.x, enemy[4].enemy_start_pos.y};
        enemy5.position = {enemy[5].enemy_start_pos.x, enemy[5].enemy_start_pos.y};
        enemy6.position = {enemy[6].enemy_start_pos.x, enemy[6].enemy_start_pos.y};
        
        // assign textures
        player.texture = "player.png";
        enemy0.texture = "mansion-enemy.png";
        enemy1.texture = "mansion-enemy-2.png";
        enemy2.texture = "mansion-enemy.png";
        enemy3.texture = "mansion-enemy-2.png";
        enemy4.texture = "mansion-enemy.png";
        enemy5.texture = "mansion-enemy.png";
        enemy6.texture = "mansion-enemy-2.png";
        
        // load textures only once
        player.LoadPlayerTexture();
        enemy0.LoadEnemyTexture();
        enemy1.LoadEnemyTexture();
        enemy2.LoadEnemyTexture();
        enemy3.LoadEnemyTexture();
        enemy4.LoadEnemyTexture();
        enemy5.LoadEnemyTexture();
        enemy6.LoadEnemyTexture();
        

        enemy0.target_player = &player;
        enemy1.target_player = &player;
        enemy2.target_player = &player;
        enemy3.target_player = &player;
        enemy4.target_player = &player;
        enemy5.target_player = &player;
        enemy6.target_player = &player;
        
        player.enemies.push_back(&enemy0);
        player.enemies.push_back(&enemy1);
        player.enemies.push_back(&enemy2);
        player.enemies.push_back(&enemy3);
        player.enemies.push_back(&enemy4);
        player.enemies.push_back(&enemy5);
        player.enemies.push_back(&enemy6);
        
        entities.push_back(&player);
        entities.push_back(&enemy0);
        entities.push_back(&enemy1);
        entities.push_back(&enemy2);
        entities.push_back(&enemy3);
        entities.push_back(&enemy4);
        entities.push_back(&enemy5);
        entities.push_back(&enemy6);
        

        // DEFINE CAMERA
        camera = { 0 };
        camera.target = player.position;

        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 2.0f;
    }

    void End() override {
        UnloadSound(level2_bg);
        UnloadTexture(tilemap);
        UnloadTexture(player.tex);
        UnloadTexture(enemy0.tex);
        UnloadTexture(enemy1.tex);
        UnloadTexture(enemy2.tex);
        UnloadTexture(enemy3.tex);
        UnloadTexture(enemy4.tex);
        UnloadTexture(enemy5.tex);
        UnloadTexture(enemy6.tex);
        //for (auto& entity : entities) {
            //delete entity;
        //}
        //entities.clear();
    }
        
    void Update() override {
        delta_time = GetFrameTime();
        
            
        for (auto& entity : entities) {
            entity->Update(delta_time);
        }

        if (player.has_lost) {
            std::cout << "Player has died!" << std::endl;
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(4);
            }
        }

        bool all_enemies_dead = true;
        for (auto& enemy : player.enemies) {
            if (!enemy->is_dead) {
                all_enemies_dead = false;
                break;
            }
        }
        if (all_enemies_dead && !player.has_lost) {
            std::cout << "Player has won!" << std::endl;
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(3);
            }
        }

        camera.target = player.position;

        Rectangle player_hitbox = {player.position.x - player.radius, player.position.y - player.radius, player.radius * 2, player.radius * 2}; // define player hitbox
        bool player_colliding_with_wall = false;

        for (int i = 0; i < gridRow; i++) {
            for (int j = 0; j < gridCol; j++) {
                if (grid[i][j] != 4) { // walls player should collide with that is not floor tile
                    Rectangle wall = {j * tileScreenW, i * tileScreenH, tileScreenW, tileScreenH};
                    if (CheckCollisionRecs(player_hitbox, wall)) {
                        std::cout << "Player collided with the wall!" << std::endl;
                        player_colliding_with_wall = true;
                        break;
                    }
                }
            }

            if(player_colliding_with_wall){
                break;
            }
                
        }

        if (player_colliding_with_wall) {
            if (player.is_dodging) {
                std::cout << "Player collided with the wall while dodging!" << std::endl;
                player.dodge_timer = 0; //would stop player from dodging
                
                Vector2 dodge_direction = Vector2Subtract(player.position, player.previous_position);
                float distance_to_wall = player.radius - 5.0f;
                player.position = Vector2Subtract(player.position, Vector2Scale(Vector2Normalize(dodge_direction), distance_to_wall));
            }
            else {
                player.position = player.previous_position;
            }
        }

        for (auto& entity: entities) {
            Enemy* enemy= dynamic_cast<Enemy*>(entity);
            if (enemy) {
                Rectangle enemy_hitbox = {enemy->position.x, enemy->position.y, enemy->rect_boundaries.x, enemy->rect_boundaries.y};
                bool enemy_colliding_with_wall = false;
                
                // traverse through grid to check if enemies and walls are colliding
                for (int i = 0; i < gridRow; i++) {
                    for (int j = 0; j < gridCol; j++) {
                        if (grid[i][j] != 4) { // enemy should collide with walls except floor tile
                            Rectangle wall = {j * tileScreenW + 30, i * tileScreenH + 30, tileScreenW, tileScreenH};
                            if (CheckCollisionRecs(enemy_hitbox, wall)) {
                                std::cout << "Enemy collided with the wall!" << std::endl;
                                enemy_colliding_with_wall = true;
                                break;
                            }
                        }
                    }
                    
                    if (enemy_colliding_with_wall) {
                        break;
                    }
                }
                
                if (enemy_colliding_with_wall) {
                    // reset move_timer
                    enemy->move_timer = 0;
                    
                    // update position
                    if (enemy->rand_x == 1) { // left
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({1, 0}, (enemy->speed * delta_time)));
                    }
                    else if (enemy->rand_x == 1) { // right
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({-1, 0}, (enemy->speed * delta_time)));
                    }
                    if (enemy->rand_y == 1) { // up
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({0, 1}, (enemy->speed * delta_time)));
                    }
                    else if (enemy->rand_y == 2) {
                        enemy->position = Vector2Add(enemy->position, Vector2Scale({0, -1}, (enemy->speed * delta_time)));
                    }
                    
                    enemy->velocity = {0, 0};
                }
            }
        }

        if(IsKeyPressed(KEY_R)){
            std::cout << "Player just saved!" << std::endl; 
            save.open("save_file.txt");
            save << 2 << "\n";
            save << player.position.x << " " << player.position.y << "\n";
            save << player.hp << "\n";
            save << enemy0.position.x << " " << enemy0.position.y << "\n";
            save << enemy0.hp << "\n";
            save << enemy1.position.x << " " << enemy1.position.y << "\n";
            save << enemy1.hp << "\n";
            save << enemy2.position.x << " " << enemy2.position.y << "\n";
            save << enemy2.hp << "\n";
            save << enemy3.position.x << " " << enemy3.position.y << "\n";
            save << enemy3.hp << "\n";
            save << enemy4.position.x << " " << enemy4.position.y << "\n";
            save << enemy4.hp << "\n";
            save << enemy5.position.x << " " << enemy5.position.y << "\n";
            save << enemy5.hp << "\n";
            save << enemy6.position.x << " " << enemy6.position.y << "\n";
            save << enemy6.hp << "\n";
            save.close();
        }
        
        
    }
    
    void Draw() override {
        ClearBackground(BLACK);
        BeginMode2D(camera);
        
        // Draw tiles based on the grid
        for (int i = 0; i < gridRow; i++) {
            for (int j = 0; j < gridCol; j++) {
                Rectangle locTile = {j * tileScreenW, i * tileScreenH, tileScreenW, tileScreenH};
                int tileIndex = grid[i][j];
                if (tileIndex < 9) {
                    DrawTexturePro(tilemap, tile[tileIndex].location, locTile, {0, 0}, 0, WHITE);
                }
            }
        }
        
        // Draw entities
        for (auto& entity : entities) {
            entity->Draw();
        }
        
        EndMode2D();
    }
};




// ------- WIN SCREEN -------
class PlayerWin : public Scene {
    Sound win;

    void Begin() override {
        win = LoadSound("win.mp3");
        PlaySound(win);
    }


    
    void End() override {
        UnloadSound(win);
    }
    
    void Update() override {
        if (IsKeyPressed(KEY_ESCAPE)) { // if player presses escape button
            CloseWindow(); // close the window
        }
    }
    
    void Draw() override {
        DrawText("YOU WIN", (screenWidth / 2) - 130, (screenHeight / 2) - 30, 50, GREEN);
        DrawText("Press [ESC] to quit the game", (screenWidth / 2) - 173, (screenHeight / 2) + 20, 20, WHITE);
    }
};

// ------- LOSE SCREEN -------
class PlayerLose : public Scene {
    Sound lose;

    void Begin() override {
        lose = LoadSound("mario_death.mp3");
        PlaySound(lose);
    }
    
    void End() override {
        UnloadSound(lose);
    }
    
    void Update() override {
        if (IsKeyPressed(KEY_ESCAPE)) { // if player presses escape button
            CloseWindow(); // close the window
        }
    }
    
    void Draw() override {
        DrawText("YOU LOSE", (screenWidth / 2) - 150, (screenHeight / 2) - 30, 50, RED);
        DrawText("Press [ESC] to quit the game", (screenWidth / 2) - 173, (screenHeight / 2) + 20, 20, WHITE);
    }
};
