#pragma once

#include <Engine/ApplicationLayer.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneLayer.hpp>
#include <Scene/EntityComponents/CharacterControllerComponent.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Scene/EntityComponents/AudioSourceComponent.hpp>
#include <Scene/SceneEnvironment.hpp>
#include <Graphics/Fonts.hpp>
#include <UI/UiLayer.hpp>
#include <UI/Widgets/TextView.hpp>
#include <Resources/DefaultResources.hpp>
#include <Resources/ResourceCache.hpp>
#include <Resources/ResourceTypes/AudioClipResource.hpp>
#include <Resources/ResourceTypes/MaterialResource.hpp>
#include <Resources/ResourceTypes/TextureResource.hpp>
#include <Resources/ResourceReference.hpp>
#include <Math/Vec3.hpp>
#include <Math/Color.hpp>
#include <Input/Input.hpp>
#include <Core/Time.hpp>
#include <unordered_map>
#include <vector>
#include <string>

namespace VSGE {
    class DoorComponent;
}

class DemoLayer : public VSGE::IApplicationLayer {
public:
    enum class GameState {
        Menu,
        Play,
        Pause,
        Win,
        Lose
    };

    struct Monster {
        VSGE::Entity* root = nullptr;
        VSGE::CharacterControllerComponent* controller = nullptr;
        VSGE::Entity* visual = nullptr;
        VSGE::Entity* body = nullptr;
        VSGE::Entity* head = nullptr;
        VSGE::Entity* jaw = nullptr;
        VSGE::Entity* growth = nullptr;
        VSGE::AudioSourceComponent* vocalSource = nullptr;
        std::vector<VSGE::Entity*> hitRegions;
        std::vector<VSGE::Entity*> writhingParts;
        std::vector<Vec3> writhingBaseRotations;
        Vec3 bodyScale = Vec3(0.56f, 0.72f, 0.52f);
        Vec3 headPosition = Vec3(0.f);
        Vec3 bodyPosition = Vec3(0.f);
        Vec3 spawnPosition = Vec3(0.f);
        Vec3 movementVelocity = Vec3(0.f);
        float bodyYaw = 0.f;
        float headYaw = 0.f;
        Vec3 growthPosition = Vec3(0.f);
        float idlePhase = 0.f;
        int archetype = 0;
        bool bodyYawInitialized = false;
        bool scareArmed = false;
        bool scarePlayed = false;
        bool vocalAlerted = false;
        float health = 100.f;
        float attackCooldown = 0.f;
        float attackAnimation = 0.f;
        float lungeCooldown = 0.f;
        float lungeTime = 0.f;
        float alertTime = 0.f;
        float lightNoticeCooldown = 0.f;
        Vec3 lastKnownPlayerPosition = Vec3(0.f);
        float wanderTimer = 0.f;
        Vec3 wanderTarget = Vec3(0.f);
        bool alive = true;
    };

    struct Assets {
        VSGE::MaterialResource* wall = nullptr;
        VSGE::MaterialResource* floor = nullptr;
        VSGE::MaterialResource* ceiling = nullptr;
        VSGE::MaterialResource* wood = nullptr;
        VSGE::MaterialResource* darkMetal = nullptr;
        VSGE::MaterialResource* mutantBase = nullptr;
        VSGE::MaterialResource* mutantCarapace = nullptr;
        VSGE::MaterialResource* mutantHide[3] = {};
        VSGE::MaterialResource* mutantCarapaceByArchetype[3] = {};
        VSGE::MaterialResource* mutantBrain[3] = {};
        VSGE::MaterialResource* mutantEyes[3] = {};
        VSGE::MaterialResource* mutantMouth[3] = {};
        VSGE::MaterialResource* mutantBone = nullptr;
        VSGE::MaterialResource* healthPickup = nullptr;
        VSGE::MaterialResource* staminaPickup = nullptr;
        VSGE::MaterialResource* horrorBlood = nullptr;
        VSGE::MaterialResource* brain = nullptr;
        VSGE::MaterialResource* eyes = nullptr;
        VSGE::MaterialResource* artifact = nullptr;
        VSGE::MaterialResource* carpet = nullptr;
        VSGE::MaterialResource* painting = nullptr;
        VSGE::MaterialResource* cloth = nullptr;
        VSGE::MaterialResource* door = nullptr;
        VSGE::MaterialResource* exitGlow = nullptr;
        VSGE::MaterialResource* eerie = nullptr;
        VSGE::MaterialResource* flame = nullptr;
    };

    struct Artifact {
        VSGE::Entity* root = nullptr;
        Vec3 basePosition = Vec3(0.f);
        bool collected = false;
        std::string name;
    };

    enum class PickupType {
        Health,
        Stamina
    };

    struct Pickup {
        VSGE::Entity* root = nullptr;
        Vec3 basePosition = Vec3(0.f);
        PickupType type = PickupType::Health;
        float phase = 0.f;
        bool collected = false;
    };

    struct Torch {
        VSGE::Entity* root = nullptr;
        VSGE::LightsourceComponent* light = nullptr;
        float phase = 0.f;
        float baseIntensity = 1.f;
    };

    struct RoomMotion {
        VSGE::Entity* entity = nullptr;
        Vec3 baseRotation = Vec3(0.f);
        Vec3 baseScale = Vec3(1.f);
        float phase = 0.f;
        float speed = 1.f;
        float sway = 0.f;
        float pulse = 0.f;
    };

private:
    Assets _assets;
    VSGE::Scene* _scene = nullptr;
    VSGE::Entity* _player = nullptr;
    VSGE::Entity* _playerVisual = nullptr;
    VSGE::Entity* _cameraEntity = nullptr;
    VSGE::Camera* _camera = nullptr;
    VSGE::Entity* _firstPersonArm = nullptr;
    VSGE::Entity* _firstPersonFist = nullptr;
    VSGE::Entity* _flashlight = nullptr;
    VSGE::Entity* _flashlightLens = nullptr;
    VSGE::Entity* _exitScreamer = nullptr;
    VSGE::LightsourceComponent* _flashlightLight = nullptr;
    VSGE::Entity* _exitDoor = nullptr;

    VSGE::Entity* _sfxEntity = nullptr;
    VSGE::AudioSourceComponent* _sfxSource = nullptr;
    VSGE::AudioSourceComponent* _ambientSource = nullptr;
    VSGE::AudioClipResource* _ambientMusicClip = nullptr;
    VSGE::AudioClipResource* _endingMusicClip = nullptr;
    std::unordered_map<std::string, std::vector<VSGE::AudioClipResource*>> _customAudioClips;
    VSGE::Entity* _footstepEntity = nullptr;
    VSGE::AudioSourceComponent* _footstepSource = nullptr;
    VSGE::Entity* _ambientEntity = nullptr;

    std::vector<Artifact> _artifacts;
    std::vector<Pickup> _pickups;
    std::vector<Monster> _monsters;
    std::vector<Torch> _torches;
    std::vector<RoomMotion> _roomMotion;

    VSGE::TextView* _hudObjective = nullptr;
    VSGE::TextView* _hudHealth = nullptr;
    VSGE::TextView* _hudStamina = nullptr;
    VSGE::TextView* _hudArtifacts = nullptr;
    VSGE::TextView* _hudInventory[5] = {};
    VSGE::TextView* _hudFlashlight = nullptr;
    VSGE::TextView* _hudMessage = nullptr;
    VSGE::TextView* _hudControls = nullptr;
    VSGE::TextView* _hudTitle = nullptr;
    VSGE::TextView* _hudSubtitle = nullptr;
    VSGE::TextView* _hudPrompt = nullptr;
    VSGE::TextView* _hudWarning = nullptr;
    VSGE::TextView* _hudCrosshair = nullptr;
    VSGE::TextView* _hudTarget = nullptr;
    VSGE::TextView* _hudScareText = nullptr;
    VSGE::TextView* _hudScareEcho = nullptr;
    VSGE::View* _hudBloodOverlay = nullptr;

    GameState _state = GameState::Menu;
    bool _mouseCaptured = false;
    bool _ignoreMouseDelta = false;
    float _yawDegrees = 180.f;
    float _pitch = 0.f;
    float _health = 100.f;
    float _stamina = 100.f;
    float _attackCooldown = 0.f;
    float _hurtCooldown = 0.f;
    float _damageFlash = 0.f;
    float _elapsed = 0.f;
    float _messageTimer = 0.f;
    float _warningTimer = 0.f;
    float _lockedDoorMessageCooldown = 0.f;
    float _flickerTime = 0.f;
    float _attackPunch = 0.f;
    float _exitScareTimer = 0.f;
    float _bobTime = 0.f;
    float _footstepTimer = 0.f;
    bool _alternateFootstep = false;
    bool _isSprinting = false;
    bool _flashlightEnabled = true;
    float _scareCooldown = 6.f;
    float _playerNoiseRadius = 0.f;
    float _playerNoiseTimer = 0.f;
    Vec3 _previousPlayerPosition = Vec3(0.f);
    bool _hasPreviousPlayerPosition = false;
    Vec3 _smoothVelocity = Vec3(0.f);
    Vec3 _aimForward = Vec3(0.f, 0.f, -1.f);
    Vec3 _lastSafePlayerPos = Vec3(0.f, 0.645f, 7.0f);
    Vec3 _lastSafeMonsterPos[3] = { Vec3(-13.2f, 0.95f, 9.0f), Vec3(12.2f, 0.95f, -8.2f), Vec3(14.0f, 0.95f, 8.7f) };
    int _artifactsCollected = 0;
    int _deaths = 0;
    bool _doorOpen = false;

public:
    DemoLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(const VSGE::IEvent& e) override {}

private:
    VSGE::TextureResource* MakeTexture(uint32 id,
                                       const std::string& name,
                                       uint32 width,
                                       uint32 height,
                                       const std::string& style);

    VSGE::MaterialResource* MakeMaterial(const std::string& name,
                                         uint32 id,
                                         VSGE::TextureResource* albedo,
                                         const VSGE::Color& color,
                                         float roughness,
                                         float metallic,
                                         float emission = 0.f,
                                         VSGE::TextureResource* emissionMap = nullptr);

    void CreateResources();
    void CreateAudio();
    VSGE::Entity* AddStaticBox(const std::string& name,
                               const Vec3& position,
                               const Vec3& scale,
                               VSGE::MaterialResource* material,
                               bool castShadows = true);
    void BuildManor();
    void CreatePlayer();
    void CreateArtifacts();
    void CreatePickups();
    void CreateMonsters();
    void CreateHUD();
    void SetSkybox();
    void UpdatePlayerCamera(float dt);
    void UpdateMonsters(float dt);
    void UpdateArtifacts(float dt);
    void CheckExit();
    void DamagePlayer(float amount);
    Monster* FindAttackTarget(const Vec3& forward);
    void Attack();
    bool HasLineOfSight(const Vec3& from, const Vec3& to, VSGE::Entity* source) const;
    void SetMessage(const std::string& text, float duration);
    void SetWarning(const std::string& text, float duration);
    void UpdateHUD();
    void ResetGame();
    void StartGame();
    void OpenDoor();
    void TogglePause();
    void SetState(GameState state);
    void SetHudText(VSGE::TextView* view, const std::string& text, const VSGE::Color& color);
    void UpdateTorchFlicker(float dt);
    void PlaySfx(const std::string& clipName);
    void PlayMonsterVocal(Monster& monster, bool attack);
    VSGE::AudioClipResource* GetCustomAudioClip(const std::string& category) const;
    std::vector<byte> MakeWavBytes(const std::string& clipName, float seconds, float (*sampleFn)(float, float));
    static float ThumpSample(float t, float phase);
    static float FootstepSample(float t, float phase);
    static float FlashlightSwitchSample(float t, float phase);
    static float SwingSample(float t, float phase);
    static float ImpactSample(float t, float phase);
    static float ScareSample(float t, float phase);
    static float ExitScareSample(float t, float phase);
    static float PickupSample(float t, float phase);
    static float HurtSample(float t, float phase);
    static float DoorSample(float t, float phase);
    static float StalkerVocalSample(float t, float phase);
    static float BruteVocalSample(float t, float phase);
    static float CrawlerVocalSample(float t, float phase);
    static float HealthPickupSample(float t, float phase);
    static float StaminaPickupSample(float t, float phase);
};
