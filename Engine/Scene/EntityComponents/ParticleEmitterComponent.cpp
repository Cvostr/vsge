#include "ParticleEmitterComponent.hpp"
#include <Math/MathBase.hpp>
#include <Core/Random.hpp>
#include <Scene/Entity.hpp>
#include <Core/Time.hpp>
#include <Math/MatrixTransform.hpp>

using namespace VSGE;

ParticleEmitterComponent::ParticleEmitterComponent() :
	_simulating(false),
	_shape(PE_SHAPE_SPHERE),
	_duration(5),
	_looping(true),
	_prewarm(true),
	_lifetime(2),
	_maxParticles(100),
	_direction(Vec3(0.f, 1.f, 0.f)),
	_velocity(8.f, 10.f),
	_constantForce(0, -9.8f, 0),
	_dampingForce(0),
	_rotation(0, 0),
	_rotationSpeed(0, 0),
	_emissionRate(200, 200)
{
	_meshResource.SetResourceType(RESOURCE_TYPE_MESH);
	_materialResource.SetResourceType(RESOURCE_TYPE_MATERIAL);
}

void ParticleEmitterComponent::Serialize(YAML::Emitter& e) {

}
void ParticleEmitterComponent::Deserialize(YAML::Node& entity) {

}

void ParticleEmitterComponent::OnPreRender() {

}

void ParticleEmitterComponent::StartSimulation() {
	_particles.resize(_maxParticles);

	for (uint32 particle_i = 0; particle_i < _maxParticles; particle_i++) {
		_particles[particle_i] = new Particle;
	}

	_simulating = true;
	_simulationTime = 0;
	_emitterTime = 0;
}

void ParticleEmitterComponent::StopSimulation() {
	//Free all particles
	for (auto particle : _particles) {
		delete particle;
	}
	//Clear particles vector
	_particles.clear();

	_simulating = false;
}

bool ParticleEmitterComponent::IsSimulating() {
	return _simulating;
}

bool ParticleEmitterComponent::EmitNewParticle() {
	Entity* ent = GetEntity();

	uint32_t FreeIndex = GetFreeParticleIndex();

	if (FreeIndex == 0xFFFFFFFF || FreeIndex > (_maxParticles - 1))
		return false;

	Particle* particlePtr = _particles[FreeIndex];

	Vec3 BaseDir;// = _getDirection(transform->abs_rotation);

	particlePtr->_size = GetRandomSize();
	particlePtr->_color = _color.OriginalValue;

	Vec3 Dir = (BaseDir + GetRandomDirection()).GetNormalized();

	switch (_shape) {
	case PE_SHAPE_SPHERE: {

		Vec3 vel = Vec3(
			GetRandomFloat(2.f) - 1.0f,
			GetRandomFloat(2.f) - 1.0f,
			GetRandomFloat(2.f) - 1.0f
		).GetNormalized();

		particlePtr->_position = ent->GetAbsolutePosition() + ent->GetAbsoluteScale() * vel * 0.5f;

		break;
	}
	case PE_SHAPE_BOX: {
		Vec3 Size = ent->GetAbsoluteScale();
		particlePtr->_position = Vec3(
			GetRandomFloat(Size.x) - Size.x * 0.5f,
			GetRandomFloat(Size.y) - Size.y * 0.5f,
			GetRandomFloat(Size.z) - Size.z * 0.5f
		);

		break;
	}
	}

	particlePtr->_rotation = GetRandomRotation();
	particlePtr->_rotationSpeed = GetRandomRotationSpeed();
	particlePtr->_velocity = Dir * GetRandomVelocity();
	particlePtr->_alive = true;
	particlePtr->_timePassed = 0;
	//particlePtr->Position += Dir * particlePtr->Size.Y;
	
	return true;
}

void ParticleEmitterComponent::StepSimulation() {
	if (!_simulating)
		return;

	if (_simulationTime >= _duration) {
		if (_looping) {
			if (!_prewarm)
				RestartSimulation();
		}
		else {
			StopSimulation();
		}
	}

	float DeltaTime = TimePerf::Get()->GetDeltaTime();

	_simulationTime += DeltaTime;
	_emitterTime += DeltaTime;

	int32_t leftToEmit = _maxParticles - GetAliveParticlesCount();

	while (leftToEmit > 0 && _emitterTime > 0) {
		if (EmitNewParticle()) {
			_emitterTime -= (1.f / GetRandomEmissionRate());
			leftToEmit--;
		}
		else
			break;
	}

	for (uint32 particle_i = 0; particle_i < GetAliveParticlesCount(); particle_i++) {
		Particle* particlePtr = _particles[particle_i];

		if (!particlePtr->_alive)
			continue;

		if (particlePtr->_timePassed >= _lifetime) {
			DestroyParticle(particlePtr);
			continue;
		}

		//update velocity
		particlePtr->_velocity += _constantForce * DeltaTime;

		if (_dampingForce != 0.f) {
			Vec3 force = particlePtr->_velocity * _dampingForce;
			particlePtr->_velocity += force * DeltaTime;
		}

		//Update size
		particlePtr->_size = particlePtr->_size + Vec2(_size.Add * DeltaTime);
		particlePtr->_size *= (_size.Mul - 1.f) * DeltaTime + 1.f;
		//Update position
		particlePtr->_position += particlePtr->_velocity * DeltaTime;
		//Update rotation
		particlePtr->_rotation += particlePtr->_rotationSpeed * DeltaTime;
		//Update particle age
		particlePtr->_timePassed += DeltaTime;
	}
}

void ParticleEmitterComponent::RestartSimulation() {
	StopSimulation();
	StartSimulation();
}

void ParticleEmitterComponent::DestroyParticle(Particle* particle) {
	particle->_alive = false;
}

uint32 ParticleEmitterComponent::GetAliveParticlesCount() {
	uint32 Result = 0;
	for (auto particle : _particles) {
		if (particle->_alive)
			Result += 1;
	}
	return Result;
}

uint32 ParticleEmitterComponent::GetFreeParticleIndex() {
	for (uint32 i = 0; i < static_cast<uint32_t>(_particles.size()); i++) {
		if (!_particles[i]->_alive)
			return i;
	}
	return UINT32_MAX;
}

Vec3 ParticleEmitterComponent::GetRandomDirection() {
	return Vec3(
		lerp(_direction.Min.x, _direction.Max.x, GetRandomFloat(1.f)),
		lerp(_direction.Min.y, _direction.Max.y, GetRandomFloat(1.f)),
		lerp(_direction.Min.z, _direction.Max.z, GetRandomFloat(1.f)));
}

Vec2 ParticleEmitterComponent::GetRandomSize() {

	Vec2 SizeMin = _size.OriginalValue.Min;
	Vec2 SizeMax = _size.OriginalValue.Max;

	return Vec2(
		lerp(SizeMin.x, SizeMax.x, GetRandomFloat(1.f)),
		lerp(SizeMin.y, SizeMax.y, GetRandomFloat(1.f)));
}

float ParticleEmitterComponent::GetRandomVelocity() {
	return lerp(_velocity.Min, _velocity.Max, GetRandomFloat(1.f));
}

float ParticleEmitterComponent::GetRandomRotation() {
	return lerp(_rotation.Min, _rotation.Max, GetRandomFloat(1.f));
}

float ParticleEmitterComponent::GetRandomRotationSpeed() {
	return lerp(_rotationSpeed.Min, _rotationSpeed.Max, GetRandomFloat(1.f));
}

int ParticleEmitterComponent::GetRandomEmissionRate() {
	return lerp(_emissionRate.Min, _emissionRate.Max, GetRandomFloat(1.f));
}

float ParticleEmitterComponent::GetRandomFloat(float max) {
	return (Rand() * max) / 32767.f;
}

void ParticleEmitterComponent::SetDuration(float duration) {
	_duration = duration;
}

void ParticleEmitterComponent::SetLooping(bool looping) {
	_looping = looping;
}

void ParticleEmitterComponent::SetPrewarm(bool prewarm) {
	_prewarm = prewarm;
}

void ParticleEmitterComponent::SetParticleLifetime(float lifetime) {
	_lifetime = lifetime;
}

void ParticleEmitterComponent::SetMaxParticlesCount(uint32 maxParticles) {
	_maxParticles = maxParticles;
}

void ParticleEmitterComponent::SetEmissionRate(int min, int max) {
	_emissionRate.Min = min;
	_emissionRate.Max = max;
}

void ParticleEmitterComponent::SetDirection(const Vec3& min, const Vec3& max) {
	_direction.Min = min;
	_direction.Max = max;
}

void ParticleEmitterComponent::SetVelocity(float min, float max) {
	_velocity.Min = min;
	_velocity.Max = max;
}

void ParticleEmitterComponent::SetConstantForce(const Vec3& force) {
	_constantForce = force;
}

void ParticleEmitterComponent::SetDampingForce(float damping) {
	_dampingForce = damping;
}

void ParticleEmitterComponent::SetRotation(float min, float max) {
	_rotation.Min = min;
	_rotation.Max = max;
}
void ParticleEmitterComponent::SetRotationSpeed(float min, float max) {
	_rotationSpeed.Max = max;
	_rotationSpeed.Min = min;
}
void ParticleEmitterComponent::SetEmitterShape(ParticleEmitterShape shape) {
	_shape = shape;
}

ParticleEmitterShape ParticleEmitterComponent::GetEmitterShape() {
	return _shape;
}
float ParticleEmitterComponent::GetDuration() {
	return _duration;
}
bool ParticleEmitterComponent::GetLooping() {
	return _looping;
}
bool ParticleEmitterComponent::GetPrewarm() {
	return _prewarm;
}
float ParticleEmitterComponent::GetParticleLifetime(){
	return _lifetime;
}
uint32 ParticleEmitterComponent::GetMaxParticlesCount() {
	return _maxParticles;
}
MinMaxValue<int>& ParticleEmitterComponent::GetEmissionRate() {
	return _emissionRate;
}
MinMaxValue<Vec3>& ParticleEmitterComponent::GetDirection() {
	return _direction;
}
void ParticleEmitterComponent::GetParticlesTransforms(Mat4** Transforms, Camera& cam) {
	uint32_t aliveParticlesCount = GetAliveParticlesCount();
	if (aliveParticlesCount == 0)
		return;
	*Transforms = new Mat4[aliveParticlesCount];
	uint32_t TransformI = 0;

	for (int i = 0; i < _particles.size(); i++) {
		Particle* Particle = _particles[i];
		if (Particle->_alive) {
			Mat4 transformMat = GetTranslationMatrix(Particle->_position);
			transformMat = RemoveRotationFromTransform(transformMat, cam.GetViewMatrix());
			transformMat = GetScaleMatrix(Vec3(Particle->_size.x, Particle->_size.y, 1)) * GetRotationZMatrixEuler(Particle->_rotation) * transformMat;

			Vec3 Rt = Particle->_velocity.GetNormalized();
			float q1 = Rt.x;
			float q2 = Rt.y;
			float q3 = Rt.z;

			/*Mat4 transformMat = getScaleMat(Particle->Size.X, Particle->Size.Y, 1)
				* getRotationMat(Quaternion(q1, q2, q3, 0)) * getRotationZMat(Particle->Rotation)
				* getTranslationMat(Particle->Position);*/

			(*Transforms)[TransformI++] = transformMat;
		}
	}

	Vec3 CamPos = cam.GetPosition();
	//Sort Array
	for (unsigned int i = 1; i < aliveParticlesCount - 1; i++) {
		for (unsigned int j = 0; j < aliveParticlesCount - i - 1; j++) {
			Vec3 Pos1 = (*Transforms)[j].GetPosition();
			Vec3 Pos2 = (*(*Transforms + (j + 1))).GetPosition();

			float Dist1 = CamPos.DistanceTo(Pos1);
			float Dist2 = CamPos.DistanceTo(Pos2);

			if (Dist1 < Dist2) {
				Mat4 temp = (*Transforms)[j];
				(*Transforms)[j] = (*Transforms)[j + 1];
				*(*Transforms + (j + 1)) = temp;
			}
		}
	}
}