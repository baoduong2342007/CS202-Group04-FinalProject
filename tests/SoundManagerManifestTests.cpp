/**
 * @file SoundManagerManifestTests.cpp
 * @brief Deterministic coverage for the typed SFX manifest and failures.
 *
 * This source intentionally has no CMake registration here; the packaging and
 * test-target owner can add it to the focused audio target without changing
 * the manifest contract.
 */

#include <cassert>
#include <filesystem>
#include <string_view>
#include <unordered_set>

#include "core/SoundManager.h"

namespace {

std::filesystem::path findCompleteAssetRoot() {
    std::filesystem::path candidate = std::filesystem::current_path();
    for (;;) {
        const auto effects = candidate / "assets" / "sounds" / "effects";
        if (std::filesystem::exists(effects / "jump.wav") &&
            std::filesystem::exists(effects / "bowserfall.wav")) {
            return candidate;
        }

        const std::filesystem::path parent = candidate.parent_path();
        if (parent == candidate) {
            return std::filesystem::current_path();
        }
        candidate = parent;
    }
}

void testManifestRowsAreCompleteAndUnique() {
    const auto& manifest = soundManifest();
    assert(!manifest.empty());
    assert(manifest.size() == static_cast<std::size_t>(SoundId::COUNT));

    std::unordered_set<std::size_t> ids;
    std::unordered_set<std::string_view> keys;
    std::unordered_set<std::string_view> paths;
    for (const SoundManifestEntry& entry : manifest) {
        assert(findSoundManifestEntry(entry.id) == &entry);
        assert(entry.key != nullptr && *entry.key != '\0');
        assert(entry.relativePath != nullptr && *entry.relativePath != '\0');
        assert(ids.insert(static_cast<std::size_t>(entry.id)).second);
        assert(keys.insert(entry.key).second);

        // Multiple semantic event IDs may intentionally share a packaged
        // sample (for example shell kick/kill).  A path is nevertheless
        // required to be repository-relative and every key/path pair is
        // represented by exactly one manifest row.
        const std::string_view path(entry.relativePath);
        constexpr std::string_view effectsPrefix = "assets/sounds/effects/";
        assert(path.size() >= effectsPrefix.size());
        assert(path.compare(0, effectsPrefix.size(), effectsPrefix) == 0);
        assert(path.find("..") == std::string_view::npos);
        assert(paths.insert(path).second ||
               entry.id == SoundId::KICK ||
               entry.id == SoundId::SHELL_KICK ||
               entry.id == SoundId::SHELL_KILL ||
               entry.id == SoundId::ENEMY_FIREBALL ||
               entry.id == SoundId::ENEMY_STAR);
    }
}

void testTypedRegistrationAndLookupCoverage() {
    SoundManager& sound = SoundManager::getInstance();
    for (const SoundManifestEntry& entry : soundManifest()) {
        assert(soundIdFromKey(entry.key).has_value());
        assert(soundIdFromKey(entry.key).value() == entry.id);
        assert(sound.isSoundLoaded(entry.id));
        assert(sound.isSoundLoaded(entry.key));
    }
}

void testLegacyBadIdsResolveSemantically() {
    assert(soundIdFromKey("pipepowerdown").value() == SoundId::POWER_DOWN);
    assert(soundIdFromKey("kickkill").value() == SoundId::SHELL_KILL);
    assert(soundIdFromKey("powerdown").value() == SoundId::POWER_DOWN);
    assert(!soundIdFromKey("not-a-real-sound").has_value());
}

void testMissingAndUnknownFailuresAreObservable() {
    SoundManager& sound = SoundManager::getInstance();

    sound.clearDiagnostic();
    assert(!sound.playSound("not-a-real-sound"));
    assert(sound.getLastDiagnostic().find("Unknown sound key") !=
           std::string::npos);

    sound.clearDiagnostic();
    const auto invalidId = static_cast<SoundId>(255);
    assert(!sound.playSound(invalidId));
    assert(sound.getLastDiagnostic().find("invalid sound ID") !=
           std::string::npos);

    sound.clearDiagnostic();
    assert(!sound.loadSound(
        SoundId::JUMP,
        (std::filesystem::path("assets") / "sounds" / "effects" /
         "missing-for-audio-test.wav")
            .string()));
    assert(sound.getLastDiagnostic().find("Failed to load sound") !=
           std::string::npos);
    // Failed replacement preserves a previously valid registration.
    assert(sound.isSoundLoaded(SoundId::JUMP));

    // Restore the canonical path after the controlled missing-file probe.
    assert(sound.loadSound(SoundId::JUMP));
}

} // namespace

int main() {
    const std::filesystem::path originalWorkingDirectory =
        std::filesystem::current_path();
    const std::filesystem::path assetRoot = findCompleteAssetRoot();
    std::filesystem::current_path(assetRoot);

    testManifestRowsAreCompleteAndUnique();
    testTypedRegistrationAndLookupCoverage();
    testLegacyBadIdsResolveSemantically();
    testMissingAndUnknownFailuresAreObservable();

    std::error_code ignored;
    std::filesystem::current_path(originalWorkingDirectory, ignored);
    return 0;
}
