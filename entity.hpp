#ifndef ENTITY
#define ENTITY

#include <raylib.h>
#include <raymath.h>

class Entity;

class Entity{
    public:
        Vector2 position;
        Vector2 previous_position;
        Vector2 rect_boundaries;
        float radius;
        float rotation;
        Color color;
    
        Vector2 velocity;
        float speed;

        int hp;
        float damage_cooldown;
    
        // get functions
        virtual Vector2 GetPosition() {
            return position;
        }
    
        virtual Vector2 GetPreviousPosition() {
            return previous_position;
        }
    
        virtual float GetRadius() {
            return radius;
        }
        
        virtual Vector2 GetRectBoundaries() {
            return rect_boundaries;
        }
    
        virtual Color GetColor() {
            return color;
        }
        
        virtual Vector2 GetVelocity() {
            return velocity;
        }
        
        virtual float GetSpeed() {
            return speed;
        }
        
        virtual float GetHP() {
            return hp;
        }
        
        virtual float GetDMGCooldown() {
            return damage_cooldown;
        }
    
        // set functions
        virtual void SetPosition(Vector2 new_position) {
            position = new_position;
        }
    
        // other entity functions
        virtual void Update(float delta_time) {
        };
    
        virtual void Draw(){
        };
};

#endif
