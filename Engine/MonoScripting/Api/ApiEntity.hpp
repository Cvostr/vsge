#pragma once

#include <Scene/Entity.hpp>

template<typename T>
void _WComponent(VSGE::Entity* ent, int action) {
	if (action == 1) {
		ent->AddComponent<T>();
	}
	else if (action == 2) {
		ent->RemoveComponent<T>();
	}
}