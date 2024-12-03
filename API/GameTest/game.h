// Game.h
#ifndef GAME_H
#define GAME_H

#include "../App/SimpleSprite.h"

class Game {
public:
    Game();
    ~Game();

    void Initialize();
    void Update(const float dt);
    void Render();

private:
    CSimpleSprite* sprite;
};

#endif // GAME_H