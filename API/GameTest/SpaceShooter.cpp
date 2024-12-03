///////////////////////////////////////////////////////////////////////////////
// Filename: SpaceShooter.cpp
// A game of shooting asteriods using the API
///////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
//------------------------------------------------------------------------
#include "app\app.h"
#include <App/main.h>
#include <ctime>
#include <algorithm>
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Space shooter :....
//------------------------------------------------------------------------

// Global variables
CSimpleSprite* playerShip = nullptr;
std::vector<CSimpleSprite*> bullets;
std::vector<CSimpleSprite*> enemies;
float enemySpawnTimer = 0.0f;
const float ENEMY_SPAWN_INTERVAL = 2.0f;
int score = 0;

// Animation states
enum PlayerAnimations {
	MOVE_LEFT,
	IDLE,
	MOVE_RIGHT
};
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	//------------------------------------------------------------------------
	// Create player ship sprite
	playerShip = App::CreateSprite(".\\Assets\\SpaceShip.bmp", 3, 1);

	// Position ship at bottom center of screen
	playerShip->SetPosition(WINDOW_WIDTH / 2, 100.0f);

	// Create animations (if your sprite sheet supports it)
	const float animSpeed = 1.0f / 15.0f;
	playerShip->CreateAnimation(IDLE, animSpeed, { 0 });
	playerShip->CreateAnimation(MOVE_LEFT, animSpeed, { 1 });
	playerShip->CreateAnimation(MOVE_RIGHT, animSpeed, { 2 });

	// Seed random number generator for enemy spawning
	srand(time(nullptr));
	//------------------------------------------------------------------------
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(const float deltaTime)
{
	//------------------------------------------------------------------------
    // Player movement
    float x, y;
    playerShip->GetPosition(x, y);

    // Horizontal movement
    if (App::GetController().GetLeftThumbStickX() > 0.5f)
    {
        playerShip->SetAnimation(IDLE);
        x += 5.0f;  // Movement speed
    }
    else if (App::GetController().GetLeftThumbStickX() < -0.5f)
    {
        playerShip->SetAnimation(IDLE);
        x -= 5.0f;  // Movement speed
    }
    else
    {
        playerShip->SetAnimation(IDLE);
    }

    // Boundary checking
    x = (x < 0.0f) ? 0.0f : x;
    x = (x > static_cast<float>(APP_INIT_WINDOW_WIDTH)) ? static_cast<float>(APP_INIT_WINDOW_WIDTH) : x;
    playerShip->SetPosition(x, y);

    // Shooting mechanism
    static float shootCooldown = 0.0f;
    shootCooldown += deltaTime;

    if (App::GetController().CheckButton(XINPUT_GAMEPAD_A, true) && shootCooldown > 0.2f)
    {
        // Create a bullet
        float bulletX, bulletY;
        playerShip->GetPosition(bulletX, bulletY);

        CSimpleSprite* bullet = App::CreateSprite(".\\Assets\\Bullet.bmp", 3, 1);
        bullet->SetPosition(bulletX, bulletY + 50);  // Spawn from ship's top
        bullets.push_back(bullet);

        // Play shooting sound
        //App::PlaySound(".\\Assets\\ShootSound.wav", false);
        shootCooldown = 0.0f;
    }

    // Update bullets
    for (auto it = bullets.begin(); it != bullets.end(); )
    {
        float bx, by;
        (*it)->GetPosition(bx, by);

        // Move bullet upwards
        by += 10.0f;
        (*it)->SetPosition(bx, by);

        // Remove bullets that go off screen
        if (by > WINDOW_HEIGHT)
        {
            delete* it;
            it = bullets.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Enemy spawning
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= ENEMY_SPAWN_INTERVAL * 100)
    {
        // Create enemy
        CSimpleSprite* enemy = App::CreateSprite(".\\Assets\\Enemy.bmp", 1, 1);

        // Spawn at random horizontal position
        float enemyX = rand() % WINDOW_WIDTH;
        enemy->SetPosition(enemyX, WINDOW_HEIGHT);
        enemies.push_back(enemy);

        enemySpawnTimer = 0.0f;
    }

    // Update and remove enemies
    for (auto it = enemies.begin(); it != enemies.end(); )
    {
        float ex, ey;
        (*it)->GetPosition(ex, ey);

        // Move enemy downwards
        ey -= 3.0f;  // Enemy speed
        (*it)->SetPosition(ex, ey);

        // Remove enemies that go off screen
        if (ey < 0)
        {
            delete* it;
            it = enemies.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Collision detection
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); )
    {
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); )
        {
            float enemyX, enemyY, bulletX, bulletY;
            (*enemyIt)->GetPosition(enemyX, enemyY);
            (*bulletIt)->GetPosition(bulletX, bulletY);

            // Simple collision check
            if (abs(enemyX - bulletX) < 50 && abs(enemyY - bulletY) < 50)
            {
                // Destroy enemy and bullet
                delete* enemyIt;
                enemyIt = enemies.erase(enemyIt);

                delete* bulletIt;
                bulletIt = bullets.erase(bulletIt);

                // Increase score
                score += 1;

                // Play explosion sound
                //App::PlaySound(".\\Assets\\Explosion.wav", false);
                break;
            }
            else
            {
                ++bulletIt;
            }
        }

        if (enemyIt != enemies.end())
        {
            ++enemyIt;
        }
    }
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	//------------------------------------------------------------------------
    // Draw player ship
    playerShip->Draw();

    // Draw bullets
    for (auto* bullet : bullets)
    {
        bullet->Draw();
    }

    // Draw enemies
    for (auto* enemy : enemies)
    {
        enemy->Draw();
    }

    // Display score
    char scoreText[64];
    sprintf(scoreText, "Score: %d", score);
    App::Print(10, WINDOW_HEIGHT - 30, scoreText, 1.0f, 1.0f, 1.0f);
}
//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{
	//------------------------------------------------------------------------
    // Clean up player ship
    delete playerShip;

    // Clean up bullets
    for (auto* bullet : bullets)
    {
        delete bullet;
    }
    bullets.clear();

    // Clean up enemies
    for (auto* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
	//------------------------------------------------------------------------
}