#include <raylib.h>
#include <raymath.h>

// Return a value up to amount units in the direction of to from from.
Vector3 v3_move_towards(Vector3 from, Vector3 to, float amount) {
	float distance = Vector3Distance(from, to);
	if (distance < amount) {
		return to;
	}
	Vector3 path = Vector3Subtract(to, from);
	Vector3 unit_path = Vector3Normalize(path);
	return Vector3Add(from, Vector3Scale(unit_path, amount));	
}

Vector3 v3_move_closer(Vector3 from, Vector3 to, float amount, float min_distance) {
	Vector3 target = v3_move_towards(to, from, min_distance);
	return v3_move_towards(from, target, amount);
}
