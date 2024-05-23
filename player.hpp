#ifndef PLAYER
#define PLAYER

#include <raylib.h>
#include <raymath.h>
#include <vector>

#include "entity.hpp"

class Entity;
class Player;
class Enemy;

class PlayerState{
    public:
        virtual ~PlayerState(){}
        virtual void Enter(Player& player) = 0;
        virtual void Update(Player& player, float delta_time) = 0;
};

class PlayerIdle : public PlayerState {
    public:
        void Enter(Player& player);
        void Update(Player& player, float delta_time);
};

class PlayerMoving : public PlayerState{
    public:
        void Enter(Player& player);
        void Update(Player& player, float delta_time);
};

class PlayerAttack : public PlayerState{
    public:
        void Enter(Player& player);
        void Update(Player& player, float delta_time);


};

class PlayerBlock : public PlayerState{
    public:
        void Enter(Player& player);
        void Update(Player& player, float delta_time);
};

class PlayerDodge : public PlayerState{
    public:
        void Enter(Player& player);
        void Update(Player& player, float delta_time);
};

class Player : public Entity{
    public:
        Sound attack_sound;

        // Attributes
        Vector2 position;
        Vector2 previous_position;
        float radius;
        Color color;
    
        std::string texture;
        Texture2D tex;
    
        Vector2 velocity;
        float speed;
    
        float hp;
        float damage_cooldown;
        float attack_timer;
        float dodge_timer;
    
        // Bools
        bool is_moving;
        bool is_attacking;
        bool is_blocking;
        bool is_dodging;
        bool damage_taken; // dmg from enemy
        bool damage_hit; // dmg towards enemy
        bool has_won;
        bool has_lost;
        bool is_on_mud;
    
    
        std::vector<Enemy*> enemies;


        PlayerIdle idle;
        PlayerMoving moving;
        PlayerAttack attack;
        PlayerBlock block;
        PlayerDodge dodge;

        Player(Vector2 pos, float rad, float spd, float initial_hp);
    
        Vector2 GetPosition() {
            return position;
        }
        
        float GetRadius() {
            return radius;
        }
    
        float GetHP() {
            return hp;
        }
        
        virtual void SetPosition(Vector2 pos) {
            previous_position = position; // store current pos before the update
            position = pos;
        }

        
        void LoadPlayerTexture() {
            tex = LoadTexture(texture.c_str());
        }

        void Update(float delta_time);
    
        void Draw();
    
        void SetState(PlayerState* new_state);
        


    private:
        PlayerState* current_state;
};
#endif

