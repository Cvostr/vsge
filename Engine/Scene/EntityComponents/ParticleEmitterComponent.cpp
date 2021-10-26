#include "ParticleEmitterComponent.hpp"
#include <Math/MathBase.hpp>
#include <Core/Random.hpp>
#include <Scene/Entity.hpp>
#include <Core/Time.hpp>
#include <Math/MatrixTransform.hpp>
#include <Core/YamlHelper.hpp>

using namespace VSGE;
using namespace YAML;

ParticleEmitterComponent::ParticleEmitterComponent() :
	_simulating(false),
	_shape(PE_SHAPE_SPHERE),
	_duration(5),
	_looping(true),
	_prewarm(true),
	_lifetime(2),
	_maxParticles(100),
	_direction(Vec3(-1.f, -1.f, -1.f), Vec3(1.f, 1.f, 1.f)),
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
	e << Key << "shape" << Value << _shape;
	e << Key << "duration" << Value << _duration;
	e << Key << "loop" << Value << _looping;
	e << Key << "prewarm" << Value << _prewarm;
	e << Key << "lifetime" << Value << _lifetime;
	e << Key << "max_particles" << Value << _maxParticles;
	e << Key << "emrate_min" << Value << _emissionRate.Min;
	e << Key << "emrate_max" << Value << _emissionRate.Max;

	e << Key << "direction_min" << Value << _direction.Min;
	e << Key << "direction_max" << Value << _direction.Max;

	e << Key << "size_min" << Value << _size.OriginalValue.Min;
	e << Key << "size_max" << Value << _size.OriginalValue.Max;
	e << Key << "size_add" << Value << _size.Add;
	e << Key << "size_mul" << Value << _size.Mul;

	e << Key << "velocity_min" << Value << _velocity.Min;
	e << Key << "velocity_max" << Value << _velocity.Max;

	e << Key << "constant_force" << Value << _constantForce;
	e << Key << "damping_force" << Value << _dampingForce;

	e << Key << "rotation_min" << Value << _rotation.Min;
	e << Key << "rotation_max" << Value << _rotation.Max;

	e << Key << "rotation_speed_min" << Value << _rotationSpeed.Min;
	e << Key << "rotation_speed_max" << Value << _rotationSpeed.Max;
}
void ParticleEmitterComponent::Deserialize(YAML::Node& entity) {
	_shape = (ParticleEmitterShape)entity["shape"].as<int>();
	_duration = entity["duration"].as<float>();
	_looping = entity["loop"].as<bool>();
	_prewarm = entity["prewarm"].as<bool>();
	_lifetime = entity["lifetime"].as<float>();
	_maxParticles = entity["max_particles"].as<uint32>();

	_emissionRate.Min = entity["emrate_min"].as<int>();
	_emissionRate.Max = entity["emrate_max"].as<int>();

	_direction.Min = entity["direction_min"].as<Vec3>();
	_direction.Max = entity["direction_max"].as<Vec3>();

	_size.OriginalValue.Min = entity["size_min"].as<Vec2>();
	_size.OriginalValue.Max = entity["size_max"].as<Vec2>();
	_size.Add = entity["size_add"].as<float>();
	_size.Mul = entity["size_mul"].as<float>();

	_velocity.Min = entity["velocity_min"].as<float>();
	_velocity.Max = entity["velocity_max"].as<float>();

	_constantForce = entity["constant_force"].as<Vec3>();
	_dampingForce = entity["damping_force"].as<float>();

	_rotation.Min = entity["rotation_min"].as<float>();
	_rotation.Max = entity["rotation_max"].as<float>();

	_rotationSpeed.Min = entity["rotation_speed_min"].as<float>();
	_rotationSpeed.Max = entity["rotation_speed_max"].as<float>();
}

void ParticleEmitterComponent::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_shape);
	serializer.Serialize(_duration);
	serializer.Serialize(_looping);
	serializer.Serialize(_prewarm);
	serializer.Serialize(_lifetime);
	serializer.Serialize(_maxParticles);

	serializer.Serialize(_emissionRate);
	serializer.Serialize(_direction);
	serializer.Serialize(_size);
	serializer.Serialize(_velocity);
	serializer.Serialize(_constantForce);
	serializer.Serialize(_dampingForce);
	serializer.Serialize(_rotation);
	serializer.Serialize(_rotationSpeed);
}
void ParticleEmitterComponent::Deserialize(ByteSolver& solver) {
	_shape = solver.GetValue<ParticleEmitterShape>();
	_duration = solver.GetValue<float>();
	_looping = solver.GetValue<bool>();
	_prewarm = solver.GetValue<bool>();
	_lifetime = solver.GetValue<float>();
	_maxParticles = solver.GetValue<uint32>();

	_emissionRate = solver.GetValue<MinMaxValue<int>>();
	_direction = solver.GetValue<MinMaxValue<Vec3>>();
	_size = solver.GetValue<DeltaValue<MinMaxValue<Vec2>>>();
	_velocity = solver.GetValue<MinMaxValue<float>>();
	_constantForce = solver.GetValue<Vec3>();
	_dampingForce = solver.GetValue<float>();
	_rotation = solver.GetValue<MinMaxValue<float>>();
	_rotationSpeed = solver.GetValue<MinMaxValue<float>>();
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

	uint32_t freeIndex = GetFreeParticleIndex();

	if (freeIndex == 0xFFFFFFFF || freeIndex > (_maxParticles - 1))
		return false;

	Particle* particlePtr = _particles[freeIndex];

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

void ParticleEmitterComponent::SetSize(DeltaValue<MinMaxValue<Vec2>>& size) {
	_size = size;
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
const MinMaxValue<int>& ParticleEmitterComponent::GetEmissionRate() {
	return _emissionRate;
}
const MinMaxValue<Vec3>& ParticleEmitterComponent::GetDirection() {
	return _direction;
}
const MinMaxValue<float>& ParticleEmitterComponent::GetVelocity() {
	return _velocity;
}
const DeltaValue<MinMaxValue<Vec2>>& ParticleEmitterComponent::GetSize() {
	return _size;
}
const Vec3& ParticleEmitterComponent::GetConstantForce() {
	return _constantForce;
}
float ParticleEmitterComponent::GetDampingForce() {
	return _dampingForce;
}
const MinMaxValue<float>& ParticleEmitterComponent::GetRotation() {
	return _rotation;
}
const MinMaxValue<float>& ParticleEmitterComponent::GetRotationSpeed() {
	return _rotationSpeed;
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
