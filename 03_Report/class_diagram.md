# Current class diagrams of the game

This document describes the classes and runtime boundaries inferred from the
current source code in `include/` and `src/`. The diagrams are split by
responsibility so each slice can be read without confusing ownership
relations with usage relations.

## Scope and how to read

- `*--` is composition: the left object owns the component's lifetime
  (usually a by-value member or a `std::unique_ptr`).
- `o--` is aggregation/non-owning recording; `-->` and `..>` are references,
  calls, or return types — they transfer no ownership.
- Cardinality labels (`1`, `0..1`, `0..*`, `2`) reflect the runtime
  members/collections, not the number of statically created C++ classes.
  `<<RAII token>>` is a role declared or shown directly in the code.
  `SaveManager` deliberately carries no Singleton label.
- Class, enum, and method names stay in English as in C++; the explanations
  are prose. Long templates are shortened inside class boxes to keep Mermaid
  stable; the precise owning type is recorded in the notes and evidence
  table.

The color scheme is consistent across blocks: orange = core/owner, blue =
state or physics, green = gameplay/entity, purple = pattern/event, and light
red = enemy. Colors are only a visual aid; semantics are decided by the
arrows and notes.

## Navigation

1. [Application loop and state stack](#runtime)
2. [Ownership boundary of the gameplay states](#state-boundary)
3. [Level, physics, and the entity collection](#world)
4. [Collision callback boundary](#collision)
5. [Entity inheritance tree](#entity-hierarchy)
6. [Mario power-up state](#mario-state)
7. [Factory Method](#factory)
8. [EventBus and Observer](#observer)
9. [Command and input](#command)
10. [Source evidence](#evidence)

<a id="runtime"></a>
## 1. Application loop and state stack

This is the path of one frame: `Game` receives events and keeps the
`InputState`, then delegates input, update, and render to `GameManager`.
`GameManager` is a Singleton coordinating a stack of `unique_ptr<IGameState>`;
`SaveManager` is merely a by-value member of the manager, not an independent
Singleton.

```mermaid
classDiagram
    direction TB

    class Game {
        -m_window: RenderWindow
        -m_renderTexture: RenderTexture
        -m_inputState: InputState
        +run()
        -processEvents()
        -update(dt)
        -render()
    }

    class GameManager {
        <<Singleton>>
        -m_stateStack: vector unique_ptr IGameState
        -m_pendingOps: deferred StateOp collection
        -m_saveManager: SaveManager
        +getInstance(): GameManager
        +changeState(newState)
        +pushState(newState)
        +popState()
        +processEvents(event)
        +processInput(inputState)
        +update(dt)
        +render(target)
    }

    class SaveManager {
        -m_savePath: string
        -m_data: SaveData
        +load(): bool
        +save(): bool
        +resetToDefaults()
        +updateHighScore(score): bool
        +updateHighestUnlockedLevel(level): bool
        +updateAudioSettings(soundVolume, musicVolume): bool
    }

    class IGameState {
        <<interface>>
        +onEnter()
        +onExit()
        +onPause()
        +onResume()
        +processEvents(event)
        +processInput(inputState)
        +update(dt)
        +render(target)
        +isOverlay(): bool
    }

    class MenuState
    class LevelSelectState
    class CharacterSelectState
    class CoopCharacterSelectState
    class PvpCharacterSelectState
    class PlayState
    class PvpPlayState
    class PauseState {
        <<overlay>>
    }
    class GameOverState
    class WinState

    Game --> GameManager : delegates frame work
    GameManager "1" *-- "0..*" IGameState : owns stack entries
    GameManager "1" *-- "1" SaveManager : value member

    IGameState <|-- MenuState
    IGameState <|-- LevelSelectState
    IGameState <|-- CharacterSelectState
    IGameState <|-- CoopCharacterSelectState
    IGameState <|-- PvpCharacterSelectState
    IGameState <|-- PlayState
    IGameState <|-- PvpPlayState
    IGameState <|-- PauseState
    IGameState <|-- GameOverState
    IGameState <|-- WinState

```

`GameManager::changeState`, `pushState`, and `popState` only enqueue a
`PendingOp`; only at the end of `GameManager::update()` does
`processPendingOps()` run. `CHANGE` calls `onExit()` on the old stack then
`onEnter()` on the new state; `PUSH` calls `onPause()` on the state below and
`POP` calls `onResume()` on the new top state. When the top state is an
overlay, `render()` draws the state below first. These behaviors are real
lifecycle semantics, not just inheritance relations on the picture.

<a id="state-boundary"></a>
## 2. Ownership boundary of the gameplay states

`PlayState` is the only campaign state owning the current level, HUD, two
by-value `InputHandler`s (the second handler is for co-op), session progress,
and event subscription tokens. `PvpPlayState` has an arena `Level` and two
input handlers. Neither state owns `Mario` directly; `Level` is the owner.

```mermaid
classDiagram
    direction LR

    class IGameState {
        <<interface>>
        +onEnter()
        +onExit()
        +onPause()
        +onResume()
        +processEvents(event)
        +processInput(inputState)
        +update(dt)
        +render(target)
    }

    class IObserver {
        <<interface>>
        +onNotify(event)
    }

    class PlayState {
        -m_level: unique_ptr Level
        -m_inputHandler: InputHandler
        -m_inputHandler2: InputHandler
        -m_hud: unique_ptr HUD
        -m_progress: GameProgress
        -m_eventSubscriptions: vector Subscription
        +onEnter()
        +onExit()
        +onPause()
        +onResume()
        +processInput(inputState)
        +update(dt)
        +render(target)
        +onNotify(event)
        -loadLevel(levelNumber): bool
    }

    class PvpPlayState {
        -m_level: unique_ptr Level
        -m_p1Input: InputHandler
        -m_p2Input: InputHandler
        -m_phase: Phase
        +onEnter()
        +onExit()
        +onPause()
        +onResume()
        +processInput(inputState)
        +update(dt)
        +render(target)
    }

    class Level {
        +loadFromFile(path): bool
        +update(dt)
        +render(target)
    }

    class HUD {
        -m_mario: Mario reference
        -m_mario2: Mario pointer
        -m_eventSubscriptions: vector Subscription
        +onNotify(event)
        +update(dt, gameplayActive)
        +draw(target)
    }

    class InputHandler {
        -m_keyBindings: key to Binding collection
        +bindKey(key, command, trigger, group)
        +clear()
        +handleInput(inputState, gameplayEnabled)
    }

    class GameProgress {
        +currentLevel: int
        +score: int
        +coins: int
        +lives: int
    }

    class Subscription {
        <<RAII token>>
        +reset()
        +connected(): bool
    }

    class EventBus {
        <<Singleton>>
        +subscribe(event, observer): Subscription
        +notify(event)
    }

    class Mario
    class GameManager {
        <<Singleton>>
        +pushState(newState)
        +changeState(newState)
    }

    IGameState <|-- PlayState
    IGameState <|-- PvpPlayState
    IObserver <|.. PlayState
    IObserver <|.. HUD

    PlayState "1" *-- "0..1" Level : m_level
    PlayState "1" *-- "1" InputHandler : m_inputHandler
    PlayState "1" *-- "1" InputHandler : m_inputHandler2 co-op
    PlayState "1" *-- "0..1" HUD : m_hud
    PlayState "1" *-- "1" GameProgress : m_progress
    PlayState "1" *-- "0..*" Subscription : event tokens
    PlayState --> EventBus : subscribe and notify
    PlayState --> GameManager : queues transitions

    PvpPlayState "1" *-- "0..1" Level : arena
    PvpPlayState "1" *-- "2" InputHandler : p1 and p2
    HUD "1" --> "1" Mario : reads authoritative values

```

In `PlayState::onEnter()`, four subscriptions are kept in
`m_eventSubscriptions`; `onExit()` releases them before stopping the music.
On receiving `GAME_PAUSED`, the state enqueues `PauseState` through
`GameManager` instead of owning the overlay itself. `HUD` holds a
`const Mario&` and optionally a second `const Mario*`, so the arrow to
`Mario` means reading data, not ownership.

<a id="world"></a>
## 3. Level, physics, and the entity collection

`Level` is the ownership boundary of a stage: the Box2D world and listener
are `unique_ptr`s, `TileMap`/`Camera` are by-value members, the primary and
second Mario (co-op/PvP) are `unique_ptr`s, and spawned entities are gathered
in a `vector<unique_ptr<Entity>>`. `TextureManager&` is a reference taken
from the global Singleton; Level does not release it. `m_pvpFireFlower` is
only a raw handle to a flower already inside `m_entities`.

```mermaid
classDiagram
    direction TB

    class Level {
        -m_world: unique_ptr b2World
        -m_contactListener: unique_ptr ContactListener
        -m_tileMap: TileMap
        -m_camera: Camera
        -m_textureManager: TextureManager reference
        -m_mario: unique_ptr Mario
        -m_mario2: optional unique_ptr Mario
        -m_entities: vector unique_ptr Entity
        -m_pvpFireFlower: FireFlower pointer
        -m_backgroundRenderer: unique_ptr BackgroundRenderer
        +loadFromFile(path, characterType): bool
        +loadPvpArena(path, playerOne, playerTwo): bool
        +update(dt)
        +render(target)
        +requestFireBallShot(mario): bool
        +getEntities(): EntityView
    }

    class b2World {
        <<external Box2D>>
    }

    class ContactListener {
        +BeginContact(contact)
        +EndContact(contact)
        +PreSolve(contact, oldManifold)
    }

    class TileMap {
        -m_physicsWorld: b2World pointer
        -m_grid: vector string
        +loadFromFile(path, mode): bool
        +createPhysicsBodies(world)
        +destroyPhysicsBodies()
        +update(dt)
        +render(target)
    }

    class Camera {
        -m_view: View
        -m_verticalMode: CameraVerticalMode
        +init(viewSize, levelBounds)
        +update(dt, targetPosition)
        +shake(duration, magnitude)
    }

    class TextureManager {
        <<Singleton>>
        +getInstance(): TextureManager
        +loadTexture(id, filename): bool
        +getTexture(id): Texture
    }

    class BackgroundRenderer {
        +setTheme(theme)
        +draw(target, camera)
    }

    class Mario
    class Entity
    class FireFlower

    class EntityView {
        <<read-only non-owning view>>
        +find(type): const Entity pointer
        +count(type): size_t
    }

    Level "1" *-- "0..1" b2World : m_world while loaded
    Level "1" *-- "0..1" ContactListener : m_contactListener while loaded
    Level "1" *-- "1" TileMap : value member
    Level "1" *-- "1" Camera : value member
    Level "1" --> "1" TextureManager : reference not owner
    Level "1" *-- "0..1" Mario : m_mario while loaded
    Level "1" *-- "0..1" Mario : m_mario2
    Level "1" *-- "0..*" Entity : m_entities owns
    Level "1" --> "0..1" FireFlower : raw handle only
    Level "1" *-- "0..1" BackgroundRenderer : unique_ptr
    Level --> EntityView : returns view
    EntityView "1" --> "0..*" Entity : non-owning pointers
    TileMap "1" --> "0..1" b2World : physics pointer
    ContactListener --> TileMap : constructor reference

```

`Level::spawnEntitiesFromTileMap()` creates a local `EntityFactory`, calls
`create(SpawnRequest, SpawnContext)`, attaches the `TextureManager`, and
pushes the result into `m_entities`; Mario is created separately beforehand.
Therefore the `Level`--`Entity` relation in the picture is true ownership,
while `getEntities()` only returns a view that expires when Level
loads/updates/mutates.

<a id="collision"></a>
## 4. Collision callback boundary

Box2D calls `ContactListener`; the listener forwards the callback to
`CollisionManager`. This utility creates/receives a `CollisionContext`
containing two type-checked `CollisionParticipant`s. The participants merely
point at live entities and bodies; they never change Level's ownership.

```mermaid
classDiagram
    direction LR

    class ContactListener {
        -m_tileMap: TileMap reference
        +BeginContact(contact)
        +EndContact(contact)
        +PreSolve(contact, oldManifold)
    }

    class CollisionManager {
        <<utility>>
        +resolve(contact, tileMap)
        +dispatch(context, tileMap)
        +end(contact)
        +preSolve(contact, tileMap)
        +defeatEnemy(victim, cause, owner): bool
    }

    class CollisionContext {
        -m_first: CollisionParticipant
        -m_second: CollisionParticipant
        -m_contact: b2Contact pointer
        +first(): CollisionParticipant
        +second(): CollisionParticipant
        +normalFrom(participant): b2Vec2
        +other(participant): CollisionParticipant
    }

    class CollisionParticipant {
        -m_entity: Entity pointer
        -m_body: b2Body pointer
        +type(): EntityType
        +subtype(): EntitySubtype
        +has(capability): bool
        +mario(): Mario pointer
        +enemy(): Enemy pointer
        +item(): Item pointer
    }

    class TileMap
    class Entity
    class Mario
    class Enemy
    class Item

    ContactListener --> CollisionManager : forwards Box2D callbacks
    ContactListener --> TileMap : borrowed reference
    CollisionManager --> TileMap : dispatch target
    CollisionManager ..> CollisionContext : typed dispatch
    CollisionContext "1" *-- "2" CollisionParticipant : value participants
    CollisionParticipant --> Entity : non-owning pointer
    CollisionManager ..> Mario : collision policy
    CollisionManager ..> Enemy : defeat policy
    CollisionManager ..> Item : pickup policy

```

This is dependency/callback, not a new object graph: `ContactListener`'s
constructor takes a `TileMap&`; `CollisionManager` has a deleted constructor
and only provides static helpers. Compatibility overloads like `isMario()`
or `isItem()` on `Entity` are not drawn as new subclasses.

<a id="entity-hierarchy"></a>
## 5. Entity inheritance tree

This diagram separates the hierarchy from Level's ownership. The enemy and
item branches list the current concrete headers; other direct world objects
such as `Elevator`, `Springboard`, `Toad`, `ScorePopup`, `BlockDebris`,
`BowserAxe`, and `BulletBillLauncher` are still `Entity`s but are omitted
from the enemy/item slice.

```mermaid
classDiagram
    direction LR

    class Entity {
        <<abstract>>
        -m_body: b2Body pointer
        -m_textureManager: TextureManager pointer
        -m_animationSystem: unique_ptr AnimationSystem
        +update(dt)
        +initPhysics(world, type, size, sensor)
        +syncPhysics()
        +getType(): EntityType
        +getSubtype(): EntitySubtype
        +takePendingSpawns(): entity collection
    }

    class Character {
        <<abstract>>
        -m_facingDirection: Direction
        -m_health: int
        -m_isGrounded: bool
        +takeDamage(amount)
        +isDead(): bool
        +isGrounded(): bool
    }

    class Mario {
        -m_marioState: MarioState
        -m_statePattern: unique_ptr IMarioState
        -m_score: int
        -m_lives: int
        +powerUp(state)
        +powerDown()
        +tryStartFireBallShot(): bool
        +loseLife()
        +respawn(position)
    }

    class Enemy {
        <<abstract>>
        +patrol()
        +onStomp()
        +onWallCollision()
        +onFireHit()
        +onStarHit()
        +tryCommitDefeat(): bool
    }

    class Item {
        <<abstract>>
        +onCollect(mario)
        +checkOverlap(other): bool
        +isCollectible(): bool
    }

    class EnemyProjectile {
        +update(dt)
    }

    class Goomba
    class Koopa
    class PiranhaPlant
    class CheepCheep
    class Blooper
    class Podoboo
    class BulletBill
    class Lakitu
    class SpinyEgg
    class Spiny
    class HammerBro
    class Bowser
    class Firebar
    class BuzzyBeetle
    class RedKoopa
    class Paratroopa
    class Hammer
    class BowserFire

    class Coin
    class Mushroom
    class FireFlower
    class Star

    class FireBall
    class QuestionBlock
    class FireballExplosion

    Entity <|-- Character
    Character <|-- Mario
    Character <|-- Enemy
    Entity <|-- Item
    Entity <|-- EnemyProjectile
    EnemyProjectile <|-- Hammer
    EnemyProjectile <|-- BowserFire

    Enemy <|-- Goomba
    Enemy <|-- Koopa
    Enemy <|-- PiranhaPlant
    Enemy <|-- CheepCheep
    Enemy <|-- Blooper
    Enemy <|-- Podoboo
    Enemy <|-- BulletBill
    Enemy <|-- Lakitu
    Enemy <|-- SpinyEgg
    Enemy <|-- Spiny
    Enemy <|-- HammerBro
    Enemy <|-- Bowser
    Enemy <|-- Firebar
    Koopa <|-- BuzzyBeetle
    Koopa <|-- RedKoopa
    Koopa <|-- Paratroopa

    Item <|-- Coin
    Item <|-- Mushroom
    Item <|-- FireFlower
    Item <|-- Star

    Entity <|-- FireBall
    Entity <|-- QuestionBlock
    Entity <|-- FireballExplosion
    Item --> Mario : onCollect reference
    FireBall --> Mario : owner pointer not owner

```

`Entity` receives a non-owning `TextureManager*`; `AnimationSystem` is the
state held by the entity via `unique_ptr`. `Enemy` and `Item` are polymorphic
behavior interfaces (patrol/defeat and onCollect), while runtime collision
identity still relies on `EntityType`, `EntitySubtype`, and capability bits.
Therefore the enum identity must not be interpreted as an extra inheritance
layer.

<a id="mario-state"></a>
## 6. Mario power-up state

Besides the game's state stack, Mario has its own State Pattern. Mario holds
the current state in a `std::unique_ptr<IMarioState>` and swaps the
implementation when `applyStateTransition()` changes `MarioState`; the four
concrete states only supply the hitbox, fireball, and brick-breaking rules.

```mermaid
classDiagram
    direction LR

    class Mario {
        -m_marioState: MarioState
        -m_statePattern: unique_ptr IMarioState
        +powerUp(state)
        +powerDown()
        +canShootFireBall(): bool
        +canBreakBricks(): bool
    }

    class MarioState {
        <<enumeration>>
        SMALL
        SUPER
        FIRE_SMALL
        FIRE_SUPER
    }

    class IMarioState {
        <<interface>>
        +onEnter(mario)
        +onExit(mario)
        +update(mario, dt)
        +getStateType(): MarioState
        +getHitboxSize(): Vector2f
        +canShootFireBall(): bool
        +canBreakBricks(): bool
    }

    class SmallMarioState
    class SuperMarioState
    class SmallFireMarioState
    class SuperFireMarioState

    Mario "1" *-- "1" IMarioState : current state
    Mario --> MarioState : enum value
    IMarioState ..> MarioState : reports type
    Mario --> IMarioState : canBreakBricks runtime delegation
    IMarioState <|.. SmallMarioState
    IMarioState <|.. SuperMarioState
    IMarioState <|.. SmallFireMarioState
    IMarioState <|.. SuperFireMarioState
    IMarioState ..> Mario : interface signature dependency only

```

These are two different state seams: `IGameState` controls the lifecycle of
game screens; `IMarioState` controls the power-up rules of one Mario. The
methods `onEnter`, `onExit`, `update`, `getHitboxSize`, and
`canShootFireBall` remain interface signatures, but there is no production
call through `m_statePattern` for them in the current `Mario.cpp`. The dashed
arrow to `Mario` is therefore only a parameter-type dependency; the single
runtime arrow in this slice is `Mario::canBreakBricks()` calling
`m_statePattern->canBreakBricks()`. Do not merge the two state seams into one
shared hierarchy, and do not assign `SaveManager` to the State Pattern.

<a id="factory"></a>
## 7. Factory Method: request -> creator -> entity

`EntityFactory::create()` is the canonical entry point. A `SpawnRequest`
carries exactly one variant payload among `EnemyType`, `ItemType`, or a tile
code; `SpawnContext` passes only a non-owning `b2World*` and the theme.
`EntityFactory` holds three creators by value; `WorldObjectCreator` in turn
holds two stateless creators by value. There is no Singleton label on the
factory: the old static helpers merely forward to the default orchestrator.

```mermaid
classDiagram
    direction TB

    class EntityFactory {
        -m_enemyCreator: EnemyCreator
        -m_itemCreator: ItemCreator
        -m_worldObjectCreator: WorldObjectCreator
        +create(request, context): unique_ptr Entity
        +createEnemy(type, position, world, theme): unique_ptr Entity
        +createItem(type, position, world, theme): unique_ptr Entity
        +createFromTileCode(tileCode, position, world, theme): unique_ptr Entity
    }

    class EntityCreator {
        <<abstract>>
        +create(request, context): unique_ptr Entity
    }

    class EnemyCreator {
        +create(request, context): unique_ptr Entity
    }

    class ItemCreator {
        +create(request, context): unique_ptr Entity
    }

    class WorldObjectCreator {
        -m_enemyCreator: EnemyCreator
        -m_itemCreator: ItemCreator
        +create(request, context): unique_ptr Entity
    }

    class SpawnRequest {
        +position: Vector2f
        +payload: variant payload
        +isEnemy(): bool
        +isItem(): bool
        +isTileCode(): bool
    }

    class SpawnContext {
        +world: b2World pointer
        +theme: LevelTheme
    }

    class Entity
    class Level

    EntityCreator <|-- EnemyCreator
    EntityCreator <|-- ItemCreator
    EntityCreator <|-- WorldObjectCreator
    EntityFactory "1" *-- "1" EnemyCreator : value member
    EntityFactory "1" *-- "1" ItemCreator : value member
    EntityFactory "1" *-- "1" WorldObjectCreator : value member
    WorldObjectCreator "1" *-- "1" EnemyCreator : delegation member
    WorldObjectCreator "1" *-- "1" ItemCreator : delegation member
    EntityFactory ..> SpawnRequest : reads payload
    EntityFactory ..> SpawnContext : borrows context
    EntityFactory --> Entity : returns unique_ptr
    EntityCreator --> Entity : factory method result
    Level ..> EntityFactory : local orchestrator

```

In `Level::spawnEntitiesFromTileMap()`, the factory creates the object and
only then does Level attach the texture and tile map for enemies and move the
`unique_ptr` into the collection. Only Level receives ownership of the
result; creators keep no entity after returning it.

<a id="observer"></a>
## 8. EventBus and Observer with the RAII token

`EventBus` implements `ISubject` and is a Singleton. `subscribe()` returns a
move-only `Subscription`; the token holds a `shared_ptr<SubscriptionLease>`,
while EventBus state stores only weak lease records. The `IObserver*` inside
a lease is a non-owning pointer. When the last token is destroyed or
`reset()`, the registration is disconnected.

```mermaid
classDiagram
    direction LR

    class ISubject {
        <<interface>>
        +subscribe(event, observer): Subscription
        +unsubscribe(event, observer)
        +notify(event)
    }

    class IObserver {
        <<interface>>
        +onNotify(event)
    }

    class EventBus {
        <<Singleton>>
        -m_state: shared_ptr EventBusState
        +getInstance(): EventBus
        +subscribe(event, observer): Subscription
        +unsubscribe(event, observer)
        +notify(gameEvent)
        +notify(eventType)
    }

    class Subscription {
        <<RAII token>>
        -m_lease: shared_ptr SubscriptionLease
        +reset()
        +connected(): bool
    }

    class EventBusState {
        <<internal>>
        -listeners: weak lease records
    }

    class SubscriptionLease {
        <<internal>>
        -bus: weak_ptr EventBusState
        -observer: IObserver pointer
        -tokenCount: size_t
        +disconnect()
        +connected(): bool
    }

    class GameEvent {
        +type: EventType
        +context: value variant
    }

    class PlayState
    class HUD
    class SoundManager {
        <<Singleton>>
        -m_eventSubscriptions: vector Subscription
        +getInstance(): SoundManager
        +onNotify(event)
    }

    ISubject <|.. EventBus
    IObserver <|.. PlayState
    IObserver <|.. HUD
    IObserver <|.. SoundManager
    EventBus "1" *-- "1" EventBusState : shared state
    EventBus ..> Subscription : returns token
    Subscription "1" *-- "0..1" SubscriptionLease : shared_ptr token lease
    EventBusState "1" o-- "0..*" SubscriptionLease : weak records
    SubscriptionLease --> EventBusState : weak bus link
    SubscriptionLease --> IObserver : non-owning pointer
    EventBus --> IObserver : dispatches callbacks
    EventBus ..> GameEvent : copies value event
    PlayState "1" *-- "0..*" Subscription : vector tokens
    HUD "1" *-- "0..*" Subscription : vector tokens
    SoundManager "1" *-- "0..*" Subscription : vector tokens
    SoundManager --> EventBus : subscribes in constructor

```

`EventBus::notify(const GameEvent&)` snapshots the listeners and re-checks
the lease before each callback, so a callback may `reset()` another token
without corrupting the loop. `PlayState::onEnter()` registers four events;
`HUD` is also an observer and keeps its own tokens. `SoundManager` is the
third `IObserver` Singleton: `Game` obtains the instance at the composition
root, the constructor subscribes to 21 events with `EventBus` and keeps the
tokens so `onNotify()` can play SFX. The EventType-only `notify` remains a
compatibility overload; the canonical payload is the value-only `GameEvent`.

<a id="command"></a>
## 9. Command and input rebinding

`InputHandler` owns commands via `unique_ptr` inside bindings; rebinding the
same key/trigger/group replaces the old object. `handleInput()` reads the
`InputState`, executes Pressed/Released/Held commands, and picks the newest
horizontal/vertical command. Concrete commands hold a non-owning Mario
pointer or receive a callback; in particular `ShootCommand` only issues a
request and never creates the `FireBall` itself.

```mermaid
classDiagram
    direction LR

    class InputHandler {
        -m_keyBindings: map key to Binding vector
        +bindKey(key, command, trigger, group)
        +unbindKey(key)
        +clear()
        +handleInput(inputState, gameplayEnabled)
        +getAction(key): ICommand pointer
    }

    class ICommand {
        <<interface>>
        +execute()
    }

    class InputState {
        +beginFrame()
        +handleEvent(event)
        +isHeld(key): bool
        +wasPressed(key): bool
        +wasReleased(key): bool
    }

    class JumpCommand {
        -m_mario: Mario pointer
        +execute()
    }

    class MoveLeftCommand {
        -m_mario: Mario pointer
        +execute()
    }

    class MoveRightCommand {
        -m_mario: Mario pointer
        +execute()
    }

    class RunCommand {
        -m_request: function callback
        +execute()
    }

    class ShootCommand {
        -m_request: function callback
        +execute()
    }

    class PauseCommand {
        +execute()
    }

    class Mario

    InputHandler "1" *-- "0..*" ICommand : unique_ptr bindings
    InputHandler --> InputState : reads per frame
    ICommand <|.. JumpCommand
    ICommand <|.. MoveLeftCommand
    ICommand <|.. MoveRightCommand
    ICommand <|.. RunCommand
    ICommand <|.. ShootCommand
    ICommand <|.. PauseCommand
    JumpCommand --> Mario : non-owning target
    MoveLeftCommand --> Mario : non-owning target
    MoveRightCommand --> Mario : non-owning target

```

`PlayState::rebindCommands()` builds the concrete commands with
`m_level->getMario()` and maps the shoot callback to
`Level::requestFireBallShot()`. This is a dependency created at the call
site, not field ownership from the command to Level.

<a id="evidence"></a>
## 10. Source evidence and representation limits

| Diagram relation/contract | Current evidence |
| --- | --- |
| `Game` loop delegates to the manager | [`include/core/Game.h:14`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/Game.h#L14), [`src/core/Game.cpp:78`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/Game.cpp#L78), [`src/core/Game.cpp:105`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/Game.cpp#L105), [`src/core/Game.cpp:129`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/Game.cpp#L129) |
| Singleton, deferred state stack, value `SaveManager` | [`include/core/GameManager.h:23`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/GameManager.h#L23), [`include/core/GameManager.h:72`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/GameManager.h#L72), [`include/core/GameManager.h:74`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/GameManager.h#L74), [`include/core/SaveManager.h:22`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/SaveManager.h#L22), [`src/core/GameManager.cpp:27`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/GameManager.cpp#L27), [`src/core/GameManager.cpp:76`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/GameManager.cpp#L76), [`src/core/GameManager.cpp:88`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/GameManager.cpp#L88) |
| change/push/pop lifecycle and overlay rendering | [`src/core/GameManager.cpp:44`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/GameManager.cpp#L44), [`src/core/GameManager.cpp:57`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/GameManager.cpp#L57), [`src/core/GameManager.cpp:64`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/GameManager.cpp#L64), [`src/core/GameManager.cpp:109`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/GameManager.cpp#L109) |
| Concrete game states and the state contract | [`include/states/IGameState.h:19`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/IGameState.h#L19), [`include/states/PlayState.h:23`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/PlayState.h#L23), [`include/states/PvpPlayState.h:24`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/PvpPlayState.h#L24), [`include/states/PauseState.h:13`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/PauseState.h#L13) |
| `PlayState` owns Level/HUD/handlers/tokens | [`include/states/PlayState.h:77`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/PlayState.h#L77), [`include/states/PlayState.h:78`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/PlayState.h#L78), [`include/states/PlayState.h:82`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/PlayState.h#L82), [`include/states/PlayState.h:106`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/PlayState.h#L106), [`include/ui/HUD.h:37`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/ui/HUD.h#L37), [`src/states/PlayState.cpp:281`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/states/PlayState.cpp#L281) |
| Level ownership and non-owning handles | [`include/level/Level.h:30`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/Level.h#L30), [`include/level/Level.h:210`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/Level.h#L210), [`include/level/Level.h:211`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/Level.h#L211), [`include/level/Level.h:212`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/Level.h#L212), [`include/level/Level.h:215`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/Level.h#L215), [`include/level/Level.h:218`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/Level.h#L218), [`src/level/Level.cpp:174`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/level/Level.cpp#L174), [`src/level/Level.cpp:175`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/level/Level.cpp#L175) |
| Level factory call and entity adoption | [`src/level/Level.cpp:685`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/level/Level.cpp#L685), [`src/level/Level.cpp:705`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/level/Level.cpp#L705), [`src/level/Level.cpp:730`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/level/Level.cpp#L730), [`src/level/Level.cpp:737`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/level/Level.cpp#L737) |
| Entity/Character/Mario/Enemy hierarchy | [`include/entities/Entity.h:31`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/Entity.h#L31), [`include/entities/Character.h:17`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/Character.h#L17), [`include/entities/Mario.h:54`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/Mario.h#L54), [`include/entities/Enemy.h:21`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/Enemy.h#L21), [`include/entities/Goomba.h:17`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/Goomba.h#L17), [`include/entities/Koopa.h:26`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/Koopa.h#L26), [`include/entities/PiranhaPlant.h:15`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/PiranhaPlant.h#L15), [`include/entities/BuzzyBeetle.h:13`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/BuzzyBeetle.h#L13) |
| Item hierarchy and collection contract | [`include/items/Item.h:14`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/items/Item.h#L14), [`include/items/Item.h:26`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/items/Item.h#L26), [`include/items/Coin.h:18`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/items/Coin.h#L18), [`include/items/Mushroom.h:19`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/items/Mushroom.h#L19), [`include/items/FireFlower.h:12`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/items/FireFlower.h#L12), [`include/items/Star.h:13`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/items/Star.h#L13) |
| Mario power-up State Pattern | [`include/states/IMarioState.h:17`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/states/IMarioState.h#L17), [`include/entities/Mario.h:205`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/entities/Mario.h#L205), [`src/entities/Mario.cpp:252`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/entities/Mario.cpp#L252), [`src/entities/Mario.cpp:963`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/entities/Mario.cpp#L963), [`src/entities/Mario.cpp:1486`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/entities/Mario.cpp#L1486), [`src/entities/Mario.cpp:1490`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/entities/Mario.cpp#L1490) |
| Read-only `EntityView` lookup results | [`include/level/EntityView.h:25`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/EntityView.h#L25), [`include/level/EntityView.h:133`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/EntityView.h#L133), [`include/level/EntityView.h:143`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/EntityView.h#L143), [`include/level/EntityView.h:156`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/EntityView.h#L156), [`include/level/EntityView.h:171`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/level/EntityView.h#L171) |
| Collision callback and typed context | [`include/physics/ContactListener.h:14`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/physics/ContactListener.h#L14), [`include/physics/CollisionManager.h:33`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/physics/CollisionManager.h#L33), [`include/physics/CollisionManager.h:61`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/physics/CollisionManager.h#L61), [`include/physics/CollisionManager.h:95`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/physics/CollisionManager.h#L95), [`src/physics/ContactListener.cpp:10`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/physics/ContactListener.cpp#L10), [`src/physics/ContactListener.cpp:14`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/physics/ContactListener.cpp#L14) |
| Factory Method creator composition | [`include/patterns/EntityFactory.h:20`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/EntityFactory.h#L20), [`include/patterns/EntityFactory.h:48`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/EntityFactory.h#L48), [`include/patterns/EntityCreator.h:14`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/EntityCreator.h#L14), [`include/patterns/WorldObjectCreator.h:13`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/WorldObjectCreator.h#L13), [`src/patterns/EntityFactory.cpp:19`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/EntityFactory.cpp#L19) |
| EventBus, Observer, and the RAII token | [`include/patterns/EventBus.h:36`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/EventBus.h#L36), [`include/patterns/IObserver.h:17`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/IObserver.h#L17), [`include/patterns/Subscription.h:23`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/Subscription.h#L23), [`src/patterns/EventBus.cpp:17`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/EventBus.cpp#L17), [`src/patterns/EventBus.cpp:51`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/EventBus.cpp#L51), [`src/patterns/EventBus.cpp:196`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/EventBus.cpp#L196), [`src/patterns/EventBus.cpp:210`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/EventBus.cpp#L210), [`src/patterns/EventBus.cpp:242`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/EventBus.cpp#L242) |
| `SoundManager` Singleton observer and subscription seam | [`include/core/SoundManager.h:116`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/SoundManager.h#L116), [`include/core/SoundManager.h:127`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/SoundManager.h#L127), [`include/core/SoundManager.h:251`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/core/SoundManager.h#L251), [`src/core/SoundManager.cpp:40`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/SoundManager.cpp#L40), [`src/core/SoundManager.cpp:45`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/SoundManager.cpp#L45), [`src/core/SoundManager.cpp:48`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/SoundManager.cpp#L48), [`src/core/SoundManager.cpp:105`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/SoundManager.cpp#L105), [`src/core/SoundManager.cpp:109`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/SoundManager.cpp#L109), [`src/core/Game.cpp:62`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/Game.cpp#L62), [`src/core/Game.cpp:65`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/core/Game.cpp#L65) |
| Command ownership and dispatch | [`include/patterns/InputHandler.h:40`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/InputHandler.h#L40), [`include/patterns/InputHandler.h:86`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/InputHandler.h#L86), [`include/patterns/ICommand.h:16`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/include/patterns/ICommand.h#L16), [`src/patterns/InputHandler.cpp:18`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/InputHandler.cpp#L18), [`src/patterns/InputHandler.cpp:46`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/patterns/InputHandler.cpp#L46), [`src/states/PlayState.cpp:62`](https://github.com/baoduong2342007/CS202-Group04-FinalProject/blob/main/05_Source_Code/src/states/PlayState.cpp#L62) |

The diagrams do not attempt to list every private field, every tile object,
or every gameplay enum. What is omitted is implementation detail that does
not change the ownership/contract boundaries drawn; a class not appearing
must not be inferred to be non-existent. A Mermaid parser/renderer is not
installed in the workspace, so the document's local verification relies on
standard `classDiagram` blocks, class IDs free of dangerous
namespace/generic characters, and delimiter balance checks.
