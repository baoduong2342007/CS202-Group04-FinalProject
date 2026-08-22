/**
 * @file Entity.h
 * @author TV1 (Duong) & TV3 (Bao)
 * @brief Base Entity class — all game objects with physics, rendering, and lifecycle management
 * @note Sprint 4: TextureManager + AnimationSystem wiring; Box2D safe destruction
 */

#pragma once

#include <memory>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

// Forward declarations — avoid heavy includes in header
class TextureManager;
class AnimationSystem;

// ── Collision Filter Groups ───────────────────────────────────────────
// Box2D rule: two fixtures that share the same NEGATIVE groupIndex never
// generate contacts / never resolve physically, regardless of category/mask.
// Mario's fixture and every FireBall fixture use this group so fireballs
// truly pass THROUGH the plumber (no push, no jitter) while every other
// body keeps default category/mask collisions.
inline constexpr int16_t COLLISION_GROUP_PLAYER_PROJECTILE = -1;

class Entity : public sf::Drawable {
public:
    // 1. Constructor / Destructor
    Entity();
    Entity(const sf::Vector2f& position, const sf::Vector2f& size);
    ~Entity() override; // Safely destroys Box2D body

    // 2. Override methods
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // 3. Public methods
    virtual void update(float dt) = 0;

    /// Set the TextureManager reference for this entity to load textures from
    void setTextureManager(TextureManager& textureManager);

    /// Load and assign a texture to this entity's sprite using TextureManager
    void setSprite(const std::string& textureId);

    /// Play a named animation clip via the entity's AnimationSystem
    void playAnimation(const std::string& clipName);

    /// Update animation frame (call each frame in update())
    void updateAnimation(float dt);

    // Box2D Physics Methods
    virtual void initPhysics(b2World* world, b2BodyType type, const sf::Vector2f& size, bool isSensor = false);
    virtual void syncPhysics();
    void destroyPhysicsBody();

    // 4. Getters / Setters
    sf::FloatRect getBoundingBox() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const { return m_size; }
    sf::Vector2f getVelocity() const;
    const std::optional<sf::Sprite>& getSprite() const { return m_sprite; }
    bool shouldRemove() const;

    /// Enum for fast constant-time type identification (eliminates dynamic_cast RTTI overhead)
    enum class EntityType {
        MARIO,
        ENEMY,
        ITEM,
        PROJECTILE,
        SPRINGBOARD,
        ELEVATOR,
        TERRAIN,
        UNKNOWN
    };

    /// Stable, behaviorally meaningful identity within the broad EntityType.
    /// Collision policy uses this value (plus capabilities), never RTTI-style
    /// virtual `isX()` predicates.
    enum class EntitySubtype : std::uint8_t {
        UNKNOWN,
        MARIO, TOAD,
        GOOMBA, KOOPA, RED_KOOPA, PARATROOPA, BUZZY_BEETLE,
        PIRANHA_PLANT, CHEEP_CHEEP, CHEEP_CHEEP_JUMPING, BLOOPER, PODOBOO, BULLET_BILL,
        LAKITU, SPINY_EGG, SPINY, HAMMER_BRO, BOWSER, FIREBAR,
        FIRE_BALL, ENEMY_PROJECTILE, HAMMER, BOWSER_FIRE,
        COIN, MUSHROOM, FIRE_FLOWER, STAR,
        QUESTION_BLOCK, SPRINGBOARD, ELEVATOR, BULLET_BILL_LAUNCHER,
        BOWSER_AXE, BLOCK_DEBRIS, FIREBALL_EXPLOSION, SCORE_POPUP
    };

    /// Orthogonal collision facts. Capabilities describe what an entity can
    /// do in a contact; they are not mutable state (dying, grounded, etc.).
    enum class Capability : std::uint32_t {
        NONE             = 0,
        STOMPABLE        = 1u << 0,
        SHELL_LIKE       = 1u << 1,
        PLAYER_PROJECTILE= 1u << 2,
        ENEMY_PROJECTILE = 1u << 3,
        COLLECTIBLE      = 1u << 4,
        FIREPROOF        = 1u << 5,
        INDESTRUCTIBLE   = 1u << 6,
        SOLID            = 1u << 7,
        TRIGGER          = 1u << 8
    };
    using Capabilities = std::uint32_t;

    static constexpr Capabilities capability(Capability value) noexcept {
        return static_cast<Capabilities>(value);
    }

    /// Pure virtual type check — constant time O(1) type identification
    virtual EntityType getType() const = 0;
    virtual EntitySubtype getSubtype() const noexcept { return EntitySubtype::UNKNOWN; }
    virtual Capabilities getCapabilities() const noexcept {
        return capability(Capability::SOLID);
    }
    bool hasCapability(Capability value) const noexcept {
        return (getCapabilities() & capability(value)) != 0;
    }

    /// Polymorphic Double Dispatch collision callbacks
    virtual void onCollisionBegin(Entity* other, b2Contact* contact, const b2Vec2& normal);
    virtual void onCollisionEnd(Entity* other, b2Contact* contact);

    /// Children this entity produced since the last drain (Lakitu's Spiny
    /// eggs, a cannon's Bullet Bills). Level is the single owner of the
    /// entity list and adopts whatever an entity hands over each frame.
    virtual std::vector<std::unique_ptr<Entity>> takePendingSpawns() {
        return {};
    }

    /// Compatibility helpers are non-virtual adapters over the stable
    /// identity contract. New collision policy must use type/subtype/capability.
    bool isItem() const { return getType() == EntityType::ITEM; }
    bool isMario() const { return getType() == EntityType::MARIO; }
    bool isEnemy() const { return getType() == EntityType::ENEMY; }
    bool isFireBall() const { return getSubtype() == EntitySubtype::FIRE_BALL; }
    bool isToad() const { return getSubtype() == EntitySubtype::TOAD; }
    bool isSpringboard() const { return getSubtype() == EntitySubtype::SPRINGBOARD; }
    bool isElevator() const { return getSubtype() == EntitySubtype::ELEVATOR; }
    bool isKoopa() const {
        const auto s = getSubtype();
        return s == EntitySubtype::KOOPA || s == EntitySubtype::RED_KOOPA ||
               s == EntitySubtype::PARATROOPA || s == EntitySubtype::BUZZY_BEETLE;
    }
    bool isPiranhaPlant() const { return getSubtype() == EntitySubtype::PIRANHA_PLANT; }
    bool isCheepCheep() const {
        return getSubtype() == EntitySubtype::CHEEP_CHEEP ||
               getSubtype() == EntitySubtype::CHEEP_CHEEP_JUMPING;
    }
    bool isBuzzyBeetle() const { return getSubtype() == EntitySubtype::BUZZY_BEETLE; }
    bool isRedKoopa() const { return getSubtype() == EntitySubtype::RED_KOOPA; }
    bool isParatroopa() const { return getSubtype() == EntitySubtype::PARATROOPA; }
    bool isBlooper() const { return getSubtype() == EntitySubtype::BLOOPER; }
    bool isPodoboo() const { return getSubtype() == EntitySubtype::PODOBOO; }
    bool isBulletBill() const { return getSubtype() == EntitySubtype::BULLET_BILL; }
    bool isBulletBillLauncher() const { return getSubtype() == EntitySubtype::BULLET_BILL_LAUNCHER; }
    bool isLakitu() const { return getSubtype() == EntitySubtype::LAKITU; }
    bool isSpinyEgg() const { return getSubtype() == EntitySubtype::SPINY_EGG; }
    bool isSpiny() const { return getSubtype() == EntitySubtype::SPINY; }
    bool isHammerBro() const { return getSubtype() == EntitySubtype::HAMMER_BRO; }
    bool isHammer() const { return getSubtype() == EntitySubtype::HAMMER; }
    bool isBowserFire() const { return getSubtype() == EntitySubtype::BOWSER_FIRE; }
    bool isEnemyProjectile() const { return hasCapability(Capability::ENEMY_PROJECTILE); }
    bool isBowser() const { return getSubtype() == EntitySubtype::BOWSER; }
    bool isBowserAxe() const { return getSubtype() == EntitySubtype::BOWSER_AXE; }
    bool isFirebar() const { return getSubtype() == EntitySubtype::FIREBAR; }
    bool isCoin() const { return getSubtype() == EntitySubtype::COIN; }
    bool isMushroom() const { return getSubtype() == EntitySubtype::MUSHROOM; }
    bool isStar() const { return getSubtype() == EntitySubtype::STAR; }
    bool isQuestionBlock() const { return getSubtype() == EntitySubtype::QUESTION_BLOCK; }


    void setPosition(const sf::Vector2f& position);
    void setVelocity(const sf::Vector2f& velocity);
    void markForRemoval();

    b2Body* getBody() const { return m_body; }

    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    bool isPendingDestroy() const { return m_pendingDestroy; }
    void markForDestroy() { m_pendingDestroy = true; m_active = false; }

protected:
    // 6. Protected members
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    std::optional<sf::Sprite> m_sprite;
    sf::Vector2f m_velocity;

    // Box2D Body
    b2Body* m_body = nullptr;

    // Rendering (non-owning reference — Level owns the TextureManager)
    TextureManager* m_textureManager = nullptr;
    std::string m_textureId;

    // Animation (Entity owns its animation state — unique_ptr ensures cleanup)
    std::unique_ptr<AnimationSystem> m_animationSystem;

    // Entity lifecycle
    bool m_markedForRemoval = false;
    bool m_active = true;
    bool m_pendingDestroy = false;
};
