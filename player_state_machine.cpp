#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "entity.hpp"
#include "player.hpp"
#include "enemy.hpp"

void Player::Update(float delta_time){
    if (hp > 0) { // player is still alive, continue updating player entity
        current_state -> Update(*this, delta_time);
    }

    Rectangle player_hitbox = {position.x - radius, position.y - radius, radius * 2, radius * 2}; // define player hitbox

    for(int i = 0; i < enemies.size(); i++) {
        Rectangle enemy_hitbox = {enemies[i]->position.x, enemies[i]->position.y, enemies[i]->rect_boundaries.x, enemies[i]->rect_boundaries.y};
        bool has_collided_with_enemy = CheckCollisionRecs(player_hitbox, enemy_hitbox);
        
        if (enemies[i]->damage_cooldown > 0) {
            if (!damage_taken && has_collided_with_enemy) {
                if (current_state == &idle || current_state == &moving || current_state == &attack) {
                    hp -= 10; // take normal damage
                    damage_taken = true;
                    std::cout << "Player loses 10 hp!" << std::endl;
                }
                if (current_state == &block) {
                    hp -= 5; // take half a damage
                    damage_taken = true;
                    std::cout << "Player loses 5 hp!" << std::endl;
                }
                if (current_state == &dodge) {
                    hp -= 0; // take 0 damage
                    damage_taken = true;
                    std::cout << "Player dodges and does not lose any hp!" << std::endl;
                }
            }
        }
        
        if (enemies[i]->damage_cooldown <= 0) {
            damage_taken = false;
            enemies[i]->damage_cooldown = 2.0f;
        }
    }
    
    if (hp <= 0) {
        has_lost = true;
    }
}



void Player::Draw(){
    Rectangle frame = {0, 0, 32, 32};
    Rectangle tex_pos = {position.x, position.y, radius, radius};
    DrawTextureRec(tex, frame, {tex_pos.x, tex_pos.y}, WHITE);
    //DrawCircleV(position, radius, color);
    // for better hp formatting
    int hp_int = static_cast<int>(GetHP());
    std::string hp_str = std::to_string(hp_int);
    DrawText(("HP: " + hp_str).c_str(), position.x, position.y - 10, 10, RED);
}

void Player::SetState(PlayerState* new_state){
    current_state = new_state;
    current_state -> Enter(*this);
}

Player::Player(Vector2 pos, float rad, float spd, float intl_hp){
    position = pos;
    radius = rad;
    speed = spd;
    hp = intl_hp;
    
    texture = "player.png";
    
    SetState(&idle);
};

void PlayerIdle::Enter(Player& player){
    std::cout << "Player is IDLE." << std::endl;
    player.is_moving = false;
    player.is_attacking = false;
    player.is_blocking = false;
    player.color = PURPLE;
}

void PlayerMoving::Enter(Player& player){
    std::cout << "Player is MOVING." << std::endl;
    player.is_moving = true;
    player.color = BLUE;
}

void PlayerAttack::Enter(Player& player){
    std::cout << "Player is ATTACKING." << std::endl;
    player.attack_timer = 0.5f;
    player.is_attacking = true;
    player.color = RED;
    player.attack_sound = LoadSound("player_attack.mp3");
    PlaySound(player.attack_sound);
}

void PlayerDodge::Enter(Player& player){
    std::cout << "Player is DODGING." << std::endl;
    player.dodge_timer = 0.5f;
    player.is_dodging = true;
    player.color = GREEN;
}

void PlayerBlock::Enter(Player& player){
    std::cout << "Player is BLOCKING." << std::endl;
    player.is_blocking = true;
    player.color = SKYBLUE;
}

// While IDLE, player can move, attack, or block.
void PlayerIdle::Update(Player& player, float delta_time){
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)) { // MOVE
        player.SetState(&player.moving);
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){ // ATTACK
        player.SetState(&player.attack);
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){ // BLOCK
        player.SetState(&player.block);
    }
    

}

// While MOVING, player can attack, dodge, or be idle again.
void PlayerMoving::Update(Player& player, float delta_time) {
    
    // if player is on mud
    if (player.is_on_mud) {
        player.speed -= 200.0f * delta_time; // decrease the player speed by a ton
        
        if (player.speed < 50.0f) { // set minimum speed so the slowness doesn't go below that
            player.speed = 50.0f;
        }
    }
    else { // if player is not on the mud
        player.speed += 100.0f * delta_time; //back to normal speed
        
        if (player.speed > 100.0f) {
            player.speed = 200.0f;
        }
    }
    
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { // ATTACK
        player.SetState(&player.attack);

    }

    if(IsKeyPressed(KEY_SPACE)) { // DODGE
        player.SetState(&player.dodge);
    }
    
    player.previous_position = player.position;
    player.velocity = Vector2Zero();
    
    if(IsKeyDown(KEY_W)) {
            player.velocity.y -= (player.speed * delta_time);
    }
    if(IsKeyDown(KEY_A)) {
            player.velocity.x -= (player.speed * delta_time);
    }
    if(IsKeyDown(KEY_S)) {
            player.velocity.y += (player.speed * delta_time);
    }
    if(IsKeyDown(KEY_D)) {
            player.velocity.x += (player.speed * delta_time);
    }
    player.position = Vector2Add(player.position, player.velocity);
    
    if(Vector2Length(player.velocity) == 0){
        player.SetState(&player.idle);
    }
}

// After ATTACKING, player can only be idle again.
void PlayerAttack::Update(Player& player, float delta_time){
    player.attack_timer -= delta_time;
    
    Rectangle player_hitbox = {player.position.x - player.radius, player.position.y - player.radius, player.radius * 2, player.radius * 2}; // define player hitbox

    if (player.attack_timer <= 0) { // if attack timer is over, player is idle again.
        player.damage_hit = false; // doesnt allow consective dmg in one atk
        player.SetState(&player.idle); // IDLE
    }
    
    if (player.attack_timer > 0) {
        std::cout << "Player is attacking!" << std::endl;
        for(int i = 0; i < player.enemies.size(); i++) {
            Rectangle enemy_hitbox = {player.enemies[i]->position.x, player.enemies[i]->position.y, player.enemies[i]->rect_boundaries.x, player.enemies[i]->rect_boundaries.y};
            bool has_collided_with_enemy = CheckCollisionRecs(player_hitbox, enemy_hitbox);
            
            if (!player.damage_hit && has_collided_with_enemy) {
                player.enemies[i]->hp -= 10; // ENEMY LOSES 10HP
                player.damage_hit = true;
                std::cout << "Player attacks the enemy. Enemy loses 10 HP!" << std::endl;
            }
        }
    }

}

// After BLOCKING, player can only be idle again.
void PlayerBlock::Update(Player& player, float delta_time){
    if(player.is_blocking && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        player.is_blocking = false;
        player.SetState(&player.idle); // IDLE
    }
}

// After DODGING, player can only be idle again.
void PlayerDodge::Update(Player& player, float delta_time){
    player.dodge_timer -= delta_time;
    
    if (player.dodge_timer <= 0) { // if dodge timer is over, player is idle again.
        player.is_dodging = false;
        player.SetState(&player.idle);
    }
    
    if (player.dodge_timer > 0) { // player is dodging
        Vector2 dodge = {0, 0};
        
        if(IsKeyDown(KEY_W)) {
            dodge.y -= (player.speed + 100) * delta_time;
            //player.SetState(&player.idle);
        }
        if(IsKeyDown(KEY_A)) {
            dodge.x -= (player.speed + 100) * delta_time;
            //player.SetState(&player.idle);
        }
        if(IsKeyDown(KEY_S)) {
            dodge.y += (player.speed + 100) * delta_time;
            //player.SetState(&player.idle);
        }
        if(IsKeyDown(KEY_D)) {
            dodge.x += (player.speed + 100) * delta_time;
            //player.SetState(&player.idle);
        }
        
        player.position = Vector2Add(player.position, dodge);
        
        if(Vector2Length(dodge) == 0){
            player.SetState(&player.idle);
        }
    }
}
