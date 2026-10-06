#pragma once

#include "Framework/Actor.h"

struct PowerupDesc : public nu::ActorDesc {

};

class Powerup : public nu::Actor {
public:
	CLASS_PROTOTYPE(Powerup)

	Powerup() = default;
	Powerup(const PowerupDesc& powerupDesc) : Actor{ powerupDesc } {}

	void Update(float dt) override;

	void OnCollision(Actor* other) override;

private:
};