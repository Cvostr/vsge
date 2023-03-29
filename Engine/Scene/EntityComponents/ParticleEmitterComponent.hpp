#pragma once

#include "../IEntityComponent.hpp"
#include <Resources/ResourceTypes/MeshResource.hpp>
#include <Resources/ResourceReference.hpp>
#include <Math/Color.hpp>
#include <string>
#include "../Camera.hpp"

namespace VSGE {

	enum ParticleEmitterShape {
		PE_SHAPE_NONE,
		PE_SHAPE_SPHERE,
		PE_SHAPE_BOX,
		PE_SHAPE_CONE
	};

	enum ParticleRotateMode {
		PRM_BILLBOARD,
		PRM_DIRECTION
	};

	template<typename T>
	struct MinMaxValue {
		T Min;
		T Max;

		MinMaxValue() :
			Min(0),
			Max(10)
		{}

		MinMaxValue(T Value) :
			Min(Value),
			Max(Value)
		{
		}

		MinMaxValue(T BeginValue, T EndValue) :
			Min(BeginValue),
			Max(EndValue)
		{}
	};

	template <typename T>
	struct DeltaValue {
		T OriginalValue;

		float Add;
		float Mul;

		DeltaValue() :
			Add(0),
			Mul(1)
		{}

		DeltaValue(T Original) :
			OriginalValue(Original),
			Add(0),
			Mul(1)
		{}
	};

	class Particle {
	public:
		bool _alive;
		Vec3 _position; //Current position of particle
		Vec3 _velocity; //Current velocity of particle
		Vec2 _size; // Current size of particle
		float _rotation; //Rotation over Z of particle
		float _rotationSpeed;
		Color _color; //Current color of particle
		float _timePassed; //Time, elapsed from creation

		Particle() :
			_alive(false),
			_timePassed(0),
			_rotation(0),
			_rotationSpeed(0)
		{}
	};

	class ParticleEmitterComponent : public IEntityComponent {
	private:
		ResourceReference _meshResource;
		ResourceReference _materialResource;

		//Array of particles
		std::vector<Particle*> _particles;

		//Shape of emitter
		ParticleEmitterShape _shape;
		float _duration; //single cycle duration
		bool _looping; //Does particle system loop
		bool _prewarm; //Restart system on loop
		float _lifetime; //Lifetime of single particle
		uint32_t _maxParticles; //Limit particles amount
		MinMaxValue<int> _emissionRate; //Particles per second emitted

		MinMaxValue<Vec3> _direction;
		DeltaValue<MinMaxValue<Vec2>> _size;
		MinMaxValue<float> _velocity;
		Vec3 _constantForce;
		float _dampingForce;
		MinMaxValue<float> _rotation;
		MinMaxValue<float> _rotationSpeed;
		DeltaValue<Color> _color;

		bool _simulating; //are particles simulating now? true if simulating
		float _emitterTime; 
		float _simulationTime; //time of simulation passed
	public:
		ParticleEmitterComponent();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_PARTICLE_EMITTER, "Particle Emitter")

		ParticleEmitterShape GetEmitterShape();
		float GetDuration();
		bool GetLooping();
		bool GetPrewarm();
		float GetParticleLifetime();
		uint32 GetMaxParticlesCount();
		const MinMaxValue<int>& GetEmissionRate();
		const MinMaxValue<Vec3>& GetDirection();
		const MinMaxValue<float>& GetVelocity();
		const DeltaValue<MinMaxValue<Vec2>>& GetSize();
		const Vec3& GetConstantForce();
		float GetDampingForce();
		const MinMaxValue<float>& GetRotation();
		const MinMaxValue<float>& GetRotationSpeed();

		void SetEmitterShape(ParticleEmitterShape shape);
		void SetDuration(float duration);
		void SetLooping(bool looping);
		void SetPrewarm(bool prewarm);
		void SetParticleLifetime(float lifetime);
		void SetMaxParticlesCount(uint32 maxParticles);
		void SetEmissionRate(int min, int max);
		void SetDirection(const Vec3& min, const Vec3& max);
		void SetSize(DeltaValue<MinMaxValue<Vec2>>& size);
		void SetVelocity(float min, float max);
		void SetConstantForce(const Vec3& force);
		void SetDampingForce(float damping);
		void SetRotation(float min, float max);
		void SetRotationSpeed(float min, float max);
		
		bool IsSimulating();

		bool EmitNewParticle();
		void StartSimulation();
		void StopSimulation();
		void StepSimulation();
		void RestartSimulation();
		void DestroyParticle(Particle* particle);
		uint32 GetAliveParticlesCount();
		uint32 GetFreeParticleIndex();
		void GetParticlesTransforms(Mat4** Transforms, Camera& cam);

		Vec3 GetRandomDirection();
		Vec2 GetRandomSize();
		float GetRandomVelocity();
		float GetRandomRotation();
		float GetRandomRotationSpeed();
		int GetRandomEmissionRate();
		float GetRandomFloat(float max);

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);

		void OnPreRender();

		void SetMeshName(const std::string& mesh);
		MeshResource* GetMeshResource() { return _meshResource.GetResource<MeshResource>(); }
		ResourceReference& GetResourceReference() {
			return _meshResource;
		}
	};
}