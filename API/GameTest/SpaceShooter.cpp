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
bool gameOver = false;

// Animation states
enum PlayerAnimations {
	IDLE,
	MOVE_LEFT,
	MOVE_RIGHT,
	PITCH_BACKWARD,
	PITCH_FORWARD,
};
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	//------------------------------------------------------------------------
	// Create player ship sprite
	playerShip = App::CreateSprite(".\\Assets\\SpaceShip.bmp", 5, 1);

	// Position ship at bottom center of screen
	playerShip->SetPosition(WINDOW_WIDTH / 2, 100.0f);

	// Create animations (if your sprite sheet supports it)
	const float animSpeed = 1.0f / 25.0f;
	playerShip->CreateAnimation(IDLE, animSpeed, { 0 });
	playerShip->CreateAnimation(MOVE_LEFT, animSpeed, { 1 });
	playerShip->CreateAnimation(MOVE_RIGHT, animSpeed, { 2 });
    playerShip->CreateAnimation(PITCH_BACKWARD, animSpeed, { 3 });
    playerShip->CreateAnimation(PITCH_FORWARD, animSpeed, { 4 });

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
    if (gameOver)
    {
        if (App::GetController().CheckButton(XINPUT_GAMEPAD_A, true))
        {
            playerShip->SetPosition(WINDOW_WIDTH / 2, 100.0f);
            gameOver = false;
        }
        else
        {
            return;
        }
    }

	//------------------------------------------------------------------------
    // Player movement
    float x, y;
    playerShip->GetPosition(x, y);

    // Horizontal movement
    if (App::GetController().GetLeftThumbStickX() > 0.5f)
    {
        playerShip->SetAnimation(MOVE_LEFT);
        x += 5.0f;  // Movement speed
    }
    else if (App::GetController().GetLeftThumbStickX() < -0.5f)
    {
        playerShip->SetAnimation(MOVE_RIGHT);
        x -= 5.0f;  // Movement speed
    }
    // Vertical movement
    else if (App::GetController().GetLeftThumbStickY() > 0.5f)
    {
        playerShip->SetAnimation(PITCH_FORWARD);
        y += 5.0f;  // Movement speed
    }
    else if (App::GetController().GetLeftThumbStickY() < -0.5f)
    {
        playerShip->SetAnimation(PITCH_BACKWARD);
        y -= 5.0f;  // Movement speed
    }
    // No movements
    else
    {
        playerShip->SetAnimation(IDLE);
    }

    // Boundary checking
    x = (x < 0.0f) ? 0.0f : x;
    x = (x > static_cast<float>(APP_INIT_WINDOW_WIDTH)) ? static_cast<float>(APP_INIT_WINDOW_WIDTH) : x;
    y = (y < 0.0f) ? 0.0f : y;
    y = (y > static_cast<float>(APP_INIT_WINDOW_HEIGHT)) ? static_cast<float>(APP_INIT_WINDOW_HEIGHT) : y;

    playerShip->SetPosition(x, y);

    // Shooting mechanism
    static float shootCooldown = 0.0f;
    shootCooldown += deltaTime;

    if (App::GetController().CheckButton(XINPUT_GAMEPAD_A, true) && shootCooldown > 0.2f)
    {
        // Create a bullet
        float bulletX, bulletY;
        playerShip->GetPosition(bulletX, bulletY);

        CSimpleSprite* bullet = App::CreateSprite(".\\Assets\\Bullet.bmp", 1, 1);
        CSimpleSprite* bullet_l = App::CreateSprite(".\\Assets\\Bullet.bmp", 1, 1);
        CSimpleSprite* bullet_r = App::CreateSprite(".\\Assets\\Bullet.bmp", 1, 1);

        // Spawn from ship's top
        bullet->SetPosition(bulletX, bulletY + 30);  
        bullet_l->SetPosition(bulletX + 20, bulletY + 20);
        bullet_r->SetPosition(bulletX - 20, bulletY + 20);

        bullets.push_back(bullet);
        bullets.push_back(bullet_l);
        bullets.push_back(bullet_r);

        // Play shooting sound
        App::PlaySound(".\\Assets\\ShootSound.wav", false);
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

    // Collision detection for player ship
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); )
    {
        float enemyX, enemyY, playerX, playerY;
        (*enemyIt)->GetPosition(enemyX, enemyY);
        playerShip->GetPosition(playerX, playerY);

        // Simple collision check
        if (abs(enemyX - playerX) < 50 && abs(enemyY - playerY) < 50)
        {
            // Game over
            gameOver = true;

            // Play game over sound
            //App::PlaySound(".\\Assets\\GameOver.wav", false);

            break;
        }
        else
        {
            ++enemyIt;
        }
    }

    // Collision detection for bullets
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); )
    {
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); )
        {
            float enemyX, enemyY, bulletX, bulletY, playerX, playerY;
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
                App::PlaySound(".\\Assets\\Explosion.wav", false);
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
// Display calls here (DrawLine,Print, DrawSprite.)
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

    App::PrintRightAligned(WINDOW_WIDTH, WINDOW_HEIGHT - 30, "Move: WASD | Shoot/Restart: Spacebar", 1.0f, 1.0f, 1.0f);

    if (gameOver)
    {
        App::PrintCentered(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "GAME OVER!", 1.0f, 0.0f, 0.0f);
    }
    else
    {
        // Display score
        char scoreText[64];
        sprintf(scoreText, "Score: %d", score);
        App::Print(10, WINDOW_HEIGHT - 30, scoreText, 1.0f, 1.0f, 1.0f);
    }
}
//------------------------------------------------------------------------
// Called when the APP_QUIT_KEY is pressed.
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