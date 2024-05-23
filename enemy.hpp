#ifndef ENEMY
#define ENEMY

#include <raylib.h>
#include <raymath.h>
#include <string>

#include "entity.hpp"

class Entity;
class Enemy;

class EnemyState{
    //Vector2 start_pos;

    public:
        virtual ~EnemyState(){}
        virtual void Enter(Enemy& enemy) = 0;
        virtual void Update(Enemy& enemy, float delta_time) = 0;
};

class EnemyWander : public EnemyState {
        //void Enter(Enemy& enemy) override;
        //void Update(Enemy& enemy, float delta_time) override;

    public:
        void Enter(Enemy& enemy);
        void Update(Enemy& enemy, float delta_time);
};

class EnemyChase : public EnemyState{
    public:
        void Enter(Enemy& enemy);
        void Update(Enemy& enemy, float delta_time);
};

class EnemyPrep : public EnemyState{ // readying attack
    public:
        void Enter(Enemy& enemy);
        void Update(Enemy& enemy, float delta_time);
};

class EnemyAttack : public EnemyState{
    public:
        void Enter(Enemy& enemy);
        void Update(Enemy& enemy, float delta_time);
};



class Enemy : public Entity {
    public:
        Vector2 position;
        Vector2 previous_position;
        Vector2 rect_boundaries;
        float rotation;
        Color color;
    
        std::string texture;
        Texture2D tex;

        float detect_radius;
        float aggro_radius;
        float attack_radius;
    
        float hp;
        float damage_cooldown;
        float attack_timer;
        Vector2 player_pos;
    
        float move_timer;
        float rand_x, rand_y;
    
        bool is_wandering;
        bool is_chasing;
        bool is_readying;
        bool is_attacking;
        bool damage_taken;
        bool damage_hit;
        bool is_dead;
    
        bool is_player_detected;
        bool is_player_on_aggro;
        bool is_player_on_atk;
    
        Player* target_player;
    
        EnemyWander wander;
        EnemyChase chase;
        EnemyPrep ready;
        EnemyAttack attack;
    
        Enemy(Vector2 pos, Vector2 rect_bounds, float det_rad, float agg_rad, float atk_rad, float spd, float rtn, float initial_hp);

        Vector2 GetPosition() {
            return position;
        }
    
        Vector2 GetRectBoundaries() {
            return rect_boundaries;
        }
    
        float GetHP() {
            return hp;
        }
    
        virtual void SetPosition(Vector2 pos) {
            previous_position = position; // store current pos before the update
            position = pos;
        }
    
        void LoadEnemyTexture() {
            tex = LoadTexture(texture.c_str());
        }

        void Update(float delta_time);

        void Draw();

        void SetState(EnemyState* new_state);

    private:
        EnemyState* current_state;
};

#endif
