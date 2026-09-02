/**
 * @file EntityView.h
 * @brief Short-lived, read-only observations of a Level's entity collection.
 *
 * EntityView does not own entities and does not extend their lifetime.  A view
 * is valid only while the owning Level is unchanged; any Level load, update,
 * or other mutation invalidates existing views and iterators.  Iteration
 * exposes const Entity references only, never the owning unique_ptr or the
 * mutable Entity pointer held by it.
 */

#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

#include "entities/Entity.h"

class Level;

class EntityView {
private:
    using EntityStorage = std::vector<std::unique_ptr<Entity>>;

public:
    /**
     * Forward iterator which transforms each owning slot into a const Entity
     * observation.  Null slots are skipped defensively.
     */
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const Entity;
        using difference_type = std::ptrdiff_t;
        using pointer = const Entity*;
        using reference = const Entity&;

        const_iterator() noexcept = default;

        reference operator*() const noexcept {
            return *(*m_storage)[m_index];
        }

        pointer operator->() const noexcept {
            return std::addressof(operator*());
        }

        const_iterator& operator++() noexcept {
            ++m_index;
            skipNullSlots();
            return *this;
        }

        const_iterator operator++(int) noexcept {
            const_iterator previous = *this;
            ++(*this);
            return previous;
        }

        friend bool operator==(const const_iterator& lhs,
                               const const_iterator& rhs) noexcept {
            return lhs.m_storage == rhs.m_storage &&
                   lhs.m_index == rhs.m_index;
        }

        friend bool operator!=(const const_iterator& lhs,
                               const const_iterator& rhs) noexcept {
            return !(lhs == rhs);
        }

    private:
        friend class EntityView;

        const_iterator(const EntityStorage* storage,
                       std::size_t index) noexcept
            : m_storage(storage), m_index(index) {
            skipNullSlots();
        }

        void skipNullSlots() noexcept {
            if (m_storage == nullptr) {
                return;
            }
            while (m_index < m_storage->size() &&
                   !(*m_storage)[m_index]) {
                ++m_index;
            }
        }

        const EntityStorage* m_storage = nullptr;
        std::size_t m_index = 0;
    };

    using iterator = const_iterator;
    using size_type = std::size_t;
    using Predicate = std::function<bool(const Entity&)>;

    /// Construct an empty, non-owning view.
    EntityView() noexcept = default;

    EntityView(const EntityView&) noexcept = default;
    EntityView& operator=(const EntityView&) noexcept = default;

    const_iterator begin() const noexcept {
        return const_iterator(m_storage, 0);
    }

    const_iterator end() const noexcept {
        return const_iterator(m_storage,
                              m_storage == nullptr ? 0 : m_storage->size());
    }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    /// Number of non-null entities currently observed by this view.
    size_type size() const noexcept {
        size_type count = 0;
        for (const auto& entity : *this) {
            (void)entity;
            ++count;
        }
        return count;
    }

    bool empty() const noexcept { return begin() == end(); }

    /// Return the first entity with the requested broad type, or nullptr.
    const Entity* find(Entity::EntityType type) const noexcept {
        for (const Entity& entity : *this) {
            if (entity.getType() == type) {
                return std::addressof(entity);
            }
        }
        return nullptr;
    }

    /// Return the first entity with the requested exact subtype, or nullptr.
    const Entity* find(Entity::EntitySubtype subtype) const noexcept {
        for (const Entity& entity : *this) {
            if (entity.getSubtype() == subtype) {
                return std::addressof(entity);
            }
        }
        return nullptr;
    }

    /**
     * Return the first entity having every bit in the requested capability
     * mask.  A zero mask matches every non-null entity.
     */
    const Entity* find(Entity::Capabilities capabilities) const noexcept {
        for (const Entity& entity : *this) {
            if ((entity.getCapabilities() & capabilities) == capabilities) {
                return std::addressof(entity);
            }
        }
        return nullptr;
    }

    /// Single-capability convenience overload with the same all-bits rule.
    const Entity* find(Entity::Capability capability) const noexcept {
        return find(Entity::capability(capability));
    }

    /// Return the first entity satisfying a predicate over const Entity data.
    const Entity* find(const Predicate& predicate) const {
        if (!predicate) {
            return nullptr;
        }
        for (const Entity& entity : *this) {
            if (predicate(entity)) {
                return std::addressof(entity);
            }
        }
        return nullptr;
    }

    /// Count entities whose broad type exactly equals @p type.
    size_type count(Entity::EntityType type) const noexcept {
        return countMatching([type](const Entity& entity) {
            return entity.getType() == type;
        });
    }

    /// Count entities whose subtype exactly equals @p subtype.
    size_type count(Entity::EntitySubtype subtype) const noexcept {
        return countMatching([subtype](const Entity& entity) {
            return entity.getSubtype() == subtype;
        });
    }

    /**
     * Return the number of entities having every requested capability bit.
     * A zero mask counts every non-null entity.
     */
    size_type count(Entity::Capabilities capabilities) const noexcept {
        return countMatching([capabilities](const Entity& entity) {
            return (entity.getCapabilities() & capabilities) == capabilities;
        });
    }

    /// Single-capability convenience overload with the same all-bits rule.
    size_type count(Entity::Capability capability) const noexcept {
        return count(Entity::capability(capability));
    }

    /// Count entities satisfying a predicate over const Entity data.
    size_type count(const Predicate& predicate) const {
        if (!predicate) {
            return 0;
        }
        return countMatching(predicate);
    }

private:
    friend class Level;

    explicit EntityView(const EntityStorage& storage) noexcept
        : m_storage(std::addressof(storage)) {}

    template <typename Matcher>
    size_type countMatching(const Matcher& matcher) const noexcept {
        size_type count = 0;
        for (const Entity& entity : *this) {
            if (matcher(entity)) {
                ++count;
            }
        }
        return count;
    }

    const EntityStorage* m_storage = nullptr;
};

static_assert(std::is_same_v<EntityView::const_iterator::pointer,
                             const Entity*>);
static_assert(std::is_same_v<EntityView::const_iterator::reference,
                             const Entity&>);
static_assert(std::is_const_v<EntityView::const_iterator::value_type>);
