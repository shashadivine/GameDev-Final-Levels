#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "entity.hpp"
#include "player.hpp"
#include "enemy.hpp"

void Enemy::Update(float delta_time) {
    if (hp > 0) { //if an enemy is still alive, continue updating enemy entity
        current_state -> Update(*this, delta_time);
        //radii collision with player
    }
    
    if (hp <= 0) {
        is_dead = true;
    }
}

void Enemy::Draw() {
    if (!is_dead) {
        Rectangle frame = {0, 0, 32, 32};
        Rectangle tex_pos = {position.x, position.y, radius, radius};
        DrawTextureRec(tex, frame, {tex_pos.x, tex_pos.y}, WHITE);
        //DrawCircleV(position, radius, color);
        //DrawRectanglePro({position.x, position.y, rect_boundaries.x, rect_boundaries.y}, {(rect_boundaries.x / 2),(rect_boundaries.y / 2)}, rotation, color);
        DrawCircleLines(position.x, position.y, detect_radius, BLANK); // detect range
        DrawCircleLines(position.x, position.y, aggro_radius, BLANK); //aggro range
        DrawCircleLines(position.x, position.y, attack_radius, BLANK); // attack range
        
        // for better hp formatting
        int hp_int = static_cast<int>(GetHP());
        std::string hp_str = std::to_string(hp_int);

        DrawText(("HP: " + hp_str).c_str(), position.x, position.y - 10, 10, RED);
    }
}

void Enemy::SetState(EnemyState* new_state){
    current_state = new_state;
    current_state -> Enter(*this);
}

Enemy::Enemy(Vector2 pos, Vector2 rect_bounds, float det_rad, float agg_rad, float atk_rad, float spd, float rtn, float intl_hp){
    position = pos;
    rect_boundaries = rect_bounds;
    detect_radius = det_rad;
    aggro_radius = agg_rad;
    attack_radius = atk_rad;
    speed = spd;
    hp = intl_hp;
    
    previous_position = position;
    //move_timer = 0;
    //rand_x = 0;
    //rand_y = 0;
    
    texture = "overworld-enemy.png";

    SetState(&wander);
}

void EnemyWander::Enter(Enemy& enemy){
    enemy.color = RED;
    enemy.is_chasing = false;
    enemy.is_readying = false;
    enemy.is_attacking = false;
    
    // movement stuff
    enemy.rand_x = GetRandomValue(1,2);
    enemy.rand_y = GetRandomValue(1,2);
    enemy.move_timer = 10.0f;
    
    // initialize velocity randomly
    //float random_angle = (rand() % 360) * DEG2RAD;
    //enemy.velocity = {cos(random_angle) * enemy.speed, sin(random_angle) * enemy.speed};
}

void EnemyChase::Enter(Enemy& enemy){
    enemy.is_chasing = true;
}

void EnemyPrep::Enter(Enemy& enemy){
    enemy.is_readying = true;
    enemy.attack_timer = 3.0f;
}

void EnemyAttack::Enter(Enemy& enemy){
    enemy.is_attacking = true;
    enemy.damage_cooldown = 2.0f;
    enemy.move_timer = 1.6f;
    enemy.damage_hit = false;
}

void EnemyWander::Update(Enemy& enemy, float delta_time){
    enemy.move_timer -= delta_time;
    //std::cout << enemy.move_timer << std::endl;

    if(!enemy.is_chasing && !enemy.is_attacking && enemy.move_timer > 0){ // if any is not chasing and attacking, timer is running, adjust enemy position accordingly
        if(enemy.rand_x == 1){ //left
            enemy.position = Vector2Add(enemy.position, Vector2Scale({-1,0}, (enemy.speed * delta_time)));
            //std::cout << "Position: " << enemy.position.x << ", " << enemy.position.y << std::endl;
        }
        if(enemy.rand_x == 2){ //right
            enemy.position = Vector2Add(enemy.position, Vector2Scale({1,0}, (enemy.speed * delta_time)));
            //std::cout << "Position: " << enemy.position.x << ", " << enemy.position.y << std::endl;
        }
        if(enemy.rand_y == 1){ //up
            enemy.position = Vector2Add(enemy.position, Vector2Scale({0,-1}, (enemy.speed * delta_time)));
            //std::cout << "Position: " << enemy.position.x << ", " << enemy.position.y << std::endl;
        }
        if(enemy.rand_y == 2){ //down
            enemy.position = Vector2Add(enemy.position, Vector2Scale({0,1}, (enemy.speed * delta_time)));
            //std::cout << "Position: " << enemy.position.x << ", " << enemy.position.y << std::endl;
        }
    }
    
    if(enemy.move_timer <= 0){ // if timer runs out, set random values and move timer
        enemy.rand_x = GetRandomValue(1,2);
        //std::cout << "Rand X: " << enemy.rand_x << std::endl;
        enemy.rand_y = GetRandomValue(1,2);
        //std::cout << "Rand Y: " << enemy.rand_y << std::endl;

        enemy.move_timer = 10.0f;
    }
    
    // add clamp to define bounds
    Vector2 min = {32, 32};
    Vector2 max = {1200, 700};
    enemy.position = Vector2Clamp(enemy.position, min, max);
    
    enemy.is_player_detected = CheckCollisionCircles(enemy.position, enemy.detect_radius, enemy.target_player->position, enemy.target_player->radius);
    
    if (enemy.is_player_detected) {
        enemy.is_chasing = true;
        enemy.SetState(&enemy.chase);
    }
}


void EnemyChase::Update(Enemy& enemy, float delta_time){
    // if not chasing, set to wander state
    if(!enemy.is_chasing) {
        enemy.SetState(&enemy.wander);
    }
    
    
    // if enemy is chasing player, start chasing player
    if(enemy.is_chasing) {
        std::cout << "Player is within detection radius. Enemy starts chasing!" << std::endl;
        
        // adjust enemy position based on distance from player position
        Vector2 chase_player = Vector2Subtract(enemy.target_player->position, enemy.position); // get distance of enemy from player
        chase_player = Vector2Normalize(chase_player);
        Vector2 chase_movement = Vector2Scale(chase_player, (enemy.speed * delta_time)); // scale enemy movement based on speed
        
        enemy.position = Vector2Add(enemy.position, chase_movement); // update enemy position accordingly
        
        // set enemy rotation
        enemy.rotation = atan2f(chase_player.y, chase_player.x) * RAD2DEG;
        
        // stop chasing if player is not detected anymore / outside of the radii of the enemy
        if(!CheckCollisionCircles(enemy.position, enemy.detect_radius, enemy.target_player->position, enemy.target_player->radius)) {
            std::cout << "Player escaped. Enemy stops chasing!" << std::endl;
            enemy.is_chasing = false;
        }
    }
    
    // checks if player is colliding with aggro radius of the enemy
    enemy.is_player_on_aggro = CheckCollisionCircles(enemy.position, enemy.aggro_radius, enemy.target_player->position, enemy.target_player->radius);
    
    if (enemy.is_player_on_aggro) {
        std::cout << "Player is within aggro radius. Enemy is readying to attack!" << std::endl;
        enemy.is_readying = true;
        enemy.SetState(&enemy.ready);
    }
}


void EnemyPrep::Update(Enemy& enemy, float delta_time){
    enemy.attack_timer -= delta_time;
    //std::cout << "Attack Timer: " << enemy.attack_timer << std::endl;
    
    if (enemy.is_readying && enemy.attack_timer > 0) {
        std::cout << "Starting timer before attack!" << std::endl;
        //rotate the enemy's body towards the player
    }
    
    if (enemy.attack_timer <= 0) { // if readying timer runs out, set state to attack
        std::cout << "Readying attack timer runs out." << std::endl;
        enemy.player_pos = enemy.target_player->position; // player position on enemy ready state to be passed onto attack state
        enemy.is_readying = false;
        enemy.is_attacking = true;
        enemy.SetState(&enemy.attack);
    }
}

void EnemyAttack::Update(Enemy& enemy, float delta_time) {
    enemy.damage_cooldown -= delta_time;
    std::cout << "Damage Cooldown: " << enemy.damage_cooldown << std::endl;
    
    if (enemy.damage_cooldown <= 0) {
        enemy.damage_hit = false;
        enemy.SetState(&enemy.wander);
    }
    
    if (enemy.is_attacking) {
        std::cout << "Enemy is attacking the player!" << std::endl;
        
        // calculate direction of enemy towards player position (from readying atk)
        Vector2 player_direction = Vector2Subtract(enemy.player_pos, enemy.position);
        player_direction = Vector2Normalize(player_direction);
        
        // calculate attack speed and adjust dash distance of enemy
        float attack_speed = enemy.speed * 5.0f;
        float dash_distance = 50.0f; // shorten the dash of the enemy
        
        // calculate movement based on player direction, speed, and dash distance
        Vector2 attack_movement = Vector2Scale(player_direction, attack_speed * delta_time * (dash_distance / 100.0f));
    
        // update enemy pos as it attacks the player
        enemy.position = Vector2Add(enemy.position, attack_movement);
        
        // damage cooldown runs out then damage is complete
        if (enemy.damage_cooldown <= 0) {
            enemy.damage_hit = true;
            enemy.is_attacking = false;
        }
        
    }
}







