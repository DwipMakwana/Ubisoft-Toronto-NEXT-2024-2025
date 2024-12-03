// Game.cpp
#include "Game.h"
#include "stdafx.h"
#include "../App/SimpleSprite.h""

CSimpleSprite* sprite;

Game::Game() {}
Game::~Game() {}

void Game::Initialize() {
    sprite = new CSimpleSprite(".\\TestData\\Test.bmp", 4, 4);
    sprite->SetAnimation(0);
}

void Game::Update(const float dt) {
    sprite->Update(dt);
}

void Game::Render() {
    sprite->Draw();
}