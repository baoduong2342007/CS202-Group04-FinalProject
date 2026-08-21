/**
 * @file EntityViewTests.cpp
 * @brief Read-only Level entity observations and typed query contracts.
 */

#include <cassert>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include "entities/Entity.h"
#include "level/EntityView.h"
#include "level/Level.h"

namespace {

using ViewIterator = EntityView::const_iterator;

template <typename T, typename = void>
struct has_erase : std::false_type {};

template <typename T>
struct has_erase<T, std::void_t<decltype(std::declval<T&>().erase(
                              std::declval<typename T::const_iterator>()))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_push_back : std::false_type {};

template <typename T>
struct has_push_back<T, std::void_t<decltype(std::declval<T&>().push_back(
                                  std::declval<const Entity&>()))>>
    : std::true_type {};

static_assert(std::is_same_v<std::iterator_traits<ViewIterator>::pointer,
                             const Entity*>);
static_assert(std::is_same_v<std::iterator_traits<ViewIterator>::reference,
                             const Entity&>);
static_assert(std::is_const_v<
              std::iterator_traits<ViewIterator>::value_type>);
static_assert(!std::is_convertible_v<
              std::iterator_traits<ViewIterator>::pointer, Entity*>);
static_assert(!std::is_same_v<std::iterator_traits<ViewIterator>::value_type,
                              std::unique_ptr<Entity>>);
static_assert(!has_erase<EntityView>::value);
static_assert(!has_push_back<EntityView>::value);
static_assert(!std::is_assignable_v<
              std::iterator_traits<ViewIterator>::reference, Entity>);
static_assert(std::is_constructible_v<EntityView::Predicate,
                                      bool (*)(const Entity&)>);
static_assert(!std::is_constructible_v<EntityView::Predicate,
                                       bool (*)(Entity&)>);
static_assert(std::is_same_v<
              decltype(std::declval<const Level&>().getEntities()),
              EntityView>);

void testDefaultNullSafeView() {
    const EntityView view;

    assert(view.empty());
    assert(view.size() == 0);
    assert(view.begin() == view.end());
    assert(view.find(Entity::EntityType::UNKNOWN) == nullptr);
    assert(view.find(Entity::EntitySubtype::UNKNOWN) == nullptr);
    assert(view.find(static_cast<Entity::Capabilities>(0)) == nullptr);
    assert(view.find(EntityView::Predicate(
               [](const Entity& entity) { return entity.isActive(); })) ==
           nullptr);
    assert(view.count(Entity::EntityType::UNKNOWN) == 0);
    assert(view.count(Entity::EntitySubtype::UNKNOWN) == 0);
    assert(view.count(static_cast<Entity::Capabilities>(0)) == 0);
    assert(view.count(EntityView::Predicate(
               [](const Entity& entity) { return entity.isActive(); })) == 0);
}

void testEmptyLevelView() {
    Level level;
    const Level& constLevel = level;
    const EntityView view = constLevel.getEntities();

    assert(view.empty());
    assert(view.size() == 0);
    assert(view.begin() == view.end());
    assert(view.find(Entity::EntityType::ENEMY) == nullptr);
    assert(view.find(Entity::EntitySubtype::GOOMBA) == nullptr);
    assert(view.find(Entity::Capability::STOMPABLE) == nullptr);
    assert(view.count(Entity::EntityType::ENEMY) == 0);
    assert(view.count(Entity::EntitySubtype::GOOMBA) == 0);
    assert(view.count(Entity::Capability::STOMPABLE) == 0);
}

void testIterationAndTypedQueries() {
    Level level;
    assert(level.loadFromFile("levels/level4.txt"));

    const EntityView view = level.getEntities();
    assert(!view.empty());

    std::size_t iterated = 0;
    for (const Entity& entity : view) {
        (void)entity;
        ++iterated;
    }
    assert(iterated == view.size());

    assert(view.count(Entity::EntityType::ENEMY) >= 1);
    assert(view.count(Entity::EntitySubtype::BOWSER) >= 1);
    assert(view.count(Entity::EntitySubtype::BOWSER_AXE) >= 1);
    assert(view.count(Entity::EntitySubtype::TOAD) >= 1);

    const Entity* bowser =
        view.find(Entity::EntitySubtype::BOWSER);
    assert(bowser != nullptr);
    assert(bowser->getType() == Entity::EntityType::ENEMY);

    const Entity::Capabilities solidStompable =
        Entity::capability(Entity::Capability::SOLID) |
        Entity::capability(Entity::Capability::STOMPABLE);
    const std::size_t stompableCount =
        view.count(Entity::Capability::STOMPABLE);
    const std::size_t solidStompableCount = view.count(solidStompable);
    assert(solidStompableCount <= stompableCount);
    assert(view.find(solidStompable) != nullptr);

    const Entity* activeEnemy = view.find([](const Entity& entity) {
        return entity.getType() == Entity::EntityType::ENEMY &&
               entity.isActive();
    });
    assert(activeEnemy != nullptr);
    assert(activeEnemy->getType() == Entity::EntityType::ENEMY);

    const std::size_t activeEnemyCount = view.count(
        EntityView::Predicate([](const Entity& entity) {
            return entity.getType() == Entity::EntityType::ENEMY &&
                   entity.isActive();
        }));
    assert(activeEnemyCount >= 1);
}

void testConstLevelAndShortLivedViewContract() {
    Level level;
    assert(level.loadFromFile("levels/level1.txt"));

    const Level& constLevel = level;
    const EntityView viewBeforeUpdate = constLevel.getEntities();
    assert(viewBeforeUpdate.count(Entity::EntitySubtype::MARIO) == 0);

    // EntityView is deliberately used only before this next Level mutation;
    // retaining it beyond update/load is outside its documented lifetime.
    level.update(0.0f);
    const EntityView viewAfterUpdate = constLevel.getEntities();
    assert(viewAfterUpdate.size() > 0);
}

} // namespace

int main() {
    testDefaultNullSafeView();
    testEmptyLevelView();
    testIterationAndTypedQueries();
    testConstLevelAndShortLivedViewContract();
    return 0;
}
