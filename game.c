/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include "gframework.c"
#include "raylib.h"
#include <stdlib.h>


const Color BACKGROUND_COLOR = {0, 255, 179, 255};
const int RIGHT_BOUND = 302;
const int LEFT_BOUND = 0;
const int TOP_BOUND = 0;
const int BOTTOM_BOUND = 164;

//------------------------------------------------------------------------------------
// Human
//------------------------------------------------------------------------------------
struct Human{
    int x;
    int y;
    unsigned int timer;
    char weight;    
    
};
typedef struct Human Human;

unsigned int getHumanAge(Human* this){
    return this->timer >> 6;
}

bool isHumanAdult(Human* this){
    return getHumanAge(this) >= 18;
}

void updateDeadHuman(Human* this){
    bool isAdult = isHumanAdult(this);
    int spriteIndex = 1 + (!isAdult * 2);
    draw(spriteIndex, this->x, this->y);
}

void updateHuman(Human* this, bool* hasDied, bool* multiply){
    // calculate values
    unsigned int age = getHumanAge(this);
    bool isAdult = isHumanAdult(this);
    int lethalWeight;
    
    if (age < 15){
       lethalWeight = age * 2;
    }else {
        lethalWeight = 30;
    }
    
    // move
    this->x += GetRandomValue(-1, 1);
    this->y += GetRandomValue(-1, 1);
    
    // clamp position
    if (this->x > RIGHT_BOUND) this->x = RIGHT_BOUND;
    if (this->x < LEFT_BOUND) this->x = LEFT_BOUND;
    if (this->y < TOP_BOUND) this->y = TOP_BOUND;
    if (this->y > BOTTOM_BOUND) this->y = BOTTOM_BOUND;
    
    // update
    this->timer++;
    if (this->timer % 60 == 0){
        this->weight -= 1 + isAdult;
        
        // die
        if (this->weight < lethalWeight){
            *hasDied = true;
        }else if (GetRandomValue(0, 1000) < age){
            *hasDied = true;
        }
    }
    
    // multiply
    if (isAdult && this->weight > lethalWeight * 2){
        *multiply = true;
        this->weight -= 20;
    }
    
    // draw
    int spriteIndex = (!isAdult * 2);
    draw(spriteIndex, this->x, this->y);
}

void humanEatFruit(Human* this){
    
    if (this->weight < 100){
        this->weight += 2;
    }
    
}

Human* initHuman(int x, int y){
    Human* this = malloc(sizeof(Human));
    
    this->x = x;
    this->y = y;
    this->timer = 0;
    this->weight = 5;
    
    return this;
}
//------------------------------------------------------------------------------------
// Fruits
//------------------------------------------------------------------------------------
struct Fruit{
    int x;
    int y;
    unsigned int timer;
};
typedef struct Fruit Fruit;

Fruit* initFruit(int x, int y){
    Fruit* this = malloc(sizeof(Fruit));
    this->x = x;
    this->y = y;
    this->timer = 0;
    
    return this;
}

void updateFruit(Fruit* this, bool* hasRotten){
    this->timer++;
    if (this->timer > 250){
        *hasRotten = true; 
    }
    draw(4, this->x, this->y);
}


//------------------------------------------------------------------------------------
// Vector
//------------------------------------------------------------------------------------
#define MAX_VECTOR_SIZE 10000
struct Vector{
    void* elements[MAX_VECTOR_SIZE];
    int lastUsedIndex;
};
typedef struct Vector Vector;

Vector* initVector(){
    Vector* this = malloc(sizeof(Vector));
    this->lastUsedIndex = -1;
    return this;
}

void vectorAdd(Vector* this, void* element){
    if (this->lastUsedIndex < MAX_VECTOR_SIZE){
        this->lastUsedIndex++;
        this->elements[this->lastUsedIndex] = element;
    }else {
        free(this->elements[this->lastUsedIndex]);
        this->elements[this->lastUsedIndex] = element;
    }
}

void* vectorGet(Vector* this, int index){
    return this->elements[index];
}

void vectorRemove(Vector* this, int index){
    if (index < this->lastUsedIndex){
        for (int i = index; i < this->lastUsedIndex; i++){
            this->elements[i] = this->elements[i+1];
        }
        this->lastUsedIndex--;
    }else if (index == this->lastUsedIndex){
        this->elements[this->lastUsedIndex] = 0;
        this->lastUsedIndex--;
    }
}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

#define STARTING_HUMAN_POPULATION 20
#define FRUIT_PER_UPDATE 2

int main(void)
{
    initFramework();
    
    
    Vector* aliveHumans = initVector();
    Vector* deadHumans = initVector();
    Vector* fruits = initVector();
    // setup simulation
    for (int i = 0; i < STARTING_HUMAN_POPULATION; i++){
        vectorAdd(aliveHumans, initHuman(GetRandomValue(LEFT_BOUND, RIGHT_BOUND), GetRandomValue(TOP_BOUND, BOTTOM_BOUND)));
    }
    
    while (!WindowShouldClose())
    {
        
        fDrawBegin();

            ClearBackground(BACKGROUND_COLOR);
            
            // spawn fruit
            for (int i = 0; i < FRUIT_PER_UPDATE; i++){
                vectorAdd(fruits, initFruit(GetRandomValue(LEFT_BOUND, RIGHT_BOUND), GetRandomValue(TOP_BOUND, BOTTOM_BOUND)));
            }
            
            // update dead humans
            for (int i = 0; i < deadHumans->lastUsedIndex + 1; i++){
                Human* h = vectorGet(deadHumans, i);
                updateDeadHuman(h);
            }
            
 
            // update fruits
            for (int i = 0; i < fruits->lastUsedIndex + 1; i++){
                bool hasRotten = false;
                Fruit* f = vectorGet(fruits, i);
                updateFruit(f, &hasRotten);
                if (hasRotten){
                    free(f);
                    vectorRemove(fruits, i);
                    i--;
                }
            }
            
            
            // update alive humans
            for (int i = 0; i < aliveHumans->lastUsedIndex + 1; i++){
                bool hasDied = false;
                bool multiply = false;
                Human* h = vectorGet(aliveHumans, i);
                updateHuman(h, &hasDied, &multiply);
                
                if (multiply){
                    vectorAdd(aliveHumans, initHuman(h->x, h->y));
                }
                
                if (hasDied){
                    vectorAdd(deadHumans, h);
                    vectorRemove(aliveHumans, i);
                    i--;
                }else {
                    // eat fruits
                    for (int j = 0; j < fruits->lastUsedIndex + 1; j++){
                        Fruit* f = vectorGet(fruits, j);
                        if (checkBoxCollisions(h->x, h->y, 16, 16, f->x, f->y, 16, 16)){
                            humanEatFruit(h);
                            free(f);
                            vectorRemove(fruits, j);
                            j--;
                        }
                    }
            
                }
                
                
            }
            
        fDrawEnd();
        
    }
    printf("fruits : %d \n", fruits->lastUsedIndex);

	disposeFramework();
    

    return 0;
}
