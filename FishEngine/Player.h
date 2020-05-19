#pragma once

#include "yse.hpp"
#include "Character.h"
#include "assert.h"
#include "Tool.h"
#include "Light.h"
#include <Keyboard.h>


class Player : public Character
{
private:
	bool pull;
	bool updateHook = true;
	bool returnRope = false;
	int WIDTH = 1920;
	int HEIGHT = 1280;
	InputHandler* inputhandler = nullptr;
public:
	int maxHealth = 5;
	int health = maxHealth;
	unsigned int points = 0;

	DirectX::XMFLOAT3 fishingRodPos = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 hookPos = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 ropePos = DirectX::XMFLOAT3(0, 0, 0);
	Player(InputHandler* handler);
	Light* playerLight = nullptr;

	float rodOffsetX = 3.0f;
	float rodOffsetY = 10.0f;

	float ropeOffsetX = 10.0f;
	float ropeOffsetY = 20.0f;

	float hookOffsetX = 10.0f;
	float hookOffsetY = 20.0f;

	float hookPositionCheck = 10.0f;

	YSE::sound jumpSound;
	void updatePlayer(Tool* tool, Tool* hook, Tool* rope);
	void updatePlayerTools(Tool* rod, Tool* hook, Tool* rope, double deltaTime);
	float boostReserve = 10.f;

	void stepAnim(double deltaT);
};