/**
 * @file Entity.h
 * @author TV1 (Dương) & TV3 (Bảo)
 * @brief Base Entity class — all game objects with physics, rendering, and lifecycle management
 * @note Sprint 4: TextureManager + AnimationSystem wiring; Box2D safe destruction
 */

#pragma once

#include <memory>
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

    /// Pure virtual type check — constant time O(1) type identification
    virtual EntityType getType() const = 0;

    /// Polymorphic Double Dispatch collision callbacks
    virtual void onCollisionBegin(Entity* other, b2Contact* contact, const b2Vec2& normal);
    virtual void onCollisionEnd(Entity* other, b2Contact* contact);

    /// Children this entity produced since the last drain (Lakitu's Spiny
    /// eggs, a cannon's Bullet Bills). Level is the single owner of the
    /// entity list and adopts whatever an entity hands over each frame.
    virtual std::vector<std::unique_ptr<Entity>> takePendingSpawns() {
        return {};
    }

    /// Legacy helper type checks
    virtual bool isItem() const { return getType() == EntityType::ITEM; }
    virtual bool isMario() const { return getType() == EntityType::MARIO; }
    virtual bool isEnemy() const { return getType() == EntityType::ENEMY; }
    virtual bool isFireBall() const { return getType() == EntityType::PROJECTILE; }
    virtual bool isSpringboard() const { return getType() == EntityType::SPRINGBOARD; }
    virtual bool isElevator() const { return getType() == EntityType::ELEVATOR; }
    virtual bool isKoopa() const { return false; }
    virtual bool isPiranhaPlant() const { return false; }
    virtual bool isCheepCheep() const { return false; }
    virtual bool isBuzzyBeetle() const { return false; }
    virtual bool isRedKoopa() const { return false; }
    virtual bool isParatroopa() const { return false; }
    virtual bool isBlooper() const { return false; }
    virtual bool isPodoboo() const { return false; }
    virtual bool isBulletBill() const { return false; }
    virtual bool isBulletBillLauncher() const { return false; }
    virtual bool isLakitu() const { return false; }
    virtual bool isSpinyEgg() const { return false; }
    virtual bool isSpiny() const { return false; }
    virtual bool isHammerBro() const { return false; }
    virtual bool isHammer() const { return false; }
    virtual bool isBowserFire() const { return false; }
    virtual bool isEnemyProjectile() const { return false; }
    virtual bool isBowser() const { return false; }
    virtual bool isBowserAxe() const { return false; }
    virtual bool isFirebar() const { return false; }
    virtual bool isMushroom() const { return false; }
    virtual bool isStar() const { return false; }
    virtual bool isQuestionBlock() const { return false; }


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
