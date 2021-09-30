#pragma once

#include <Scene/Entity.hpp>

template<typename T>
T* _WComponent(VSGE::Entity* ent, int action) {
	if (action == 1) {
		return ent->AddComponent<T>();
	}
	else if (action == 2) {
		ent->RemoveComponent<T>();
	}
	else if (action == 3)
		return ent->GetComponent<T>();
	return nullptr;
}