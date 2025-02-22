#pragma once

#include <raylib.h>

// Return a value up to amount units in the direction of to from from.
Vector3 v3_move_towards(const Vector3& from, const Vector3& to, float amount);

// Returns a vector that is `amount` units closer to `to`, 
Vector3 v3_move_closer(const Vector3& from, const Vector3& to, float amount, float min_distance);