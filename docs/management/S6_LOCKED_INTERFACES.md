# Sprint 6 — Khóa Public Interfaces

> **Owner:** TV1 (Dương) — S6-TV1-03
> **Mục đích:** Chốt các contract public giữa các module. Thay đổi interface sau khi khóa phải được TV1 approve.
> **DoD:** Thay đổi interface sau khi khóa phải được TV1 approve.

---

## 1. Progress & Level catalog

```cpp
struct GameProgress {
    int currentLevel = 1;
    int score = 0;
    int coins = 0;
    int lives = 3;
    MarioState power = MarioState::SMALL;
};

struct LevelDefinition {
    int number;
    std::string filePath;
    std::string worldLabel;
    LevelTheme theme;
    MusicId music;
};
```

> `GameProgress` đã implement tại `include/core/GameProgress.h` (S6-TV1-08).
> `LevelDefinition` chờ TV4 xác nhận metadata (S6-TV1-05).

## 2. Save data

```cpp
struct SaveData {
    int version = 1;
    int highScore = 0;
    int highestUnlockedLevel = 1;
    float soundVolume = 80.0f;
    float musicVolume = 70.0f;
};
```

> TV4 sở hữu SaveManager; TV1 chỉ tích hợp state (S6-TV1-19).

## 3. State lifecycle

```cpp
virtual void onEnter();
virtual void onExit();
virtual void onPause();
virtual void onResume();
```

> Đã implement tại `include/states/IGameState.h` (S6-TV1-15).
> `GameManager` gọi `onPause()` khi push, `onResume()` khi pop (S6-TV1-16).

## 4. Input & Command

- `InputState` phân biệt `Pressed`, `Held`, `Released`.
- Bổ sung `RunCommand` và `ShootCommand` (TV5).
- `ICommand` đã bỏ `undo()` (S6-TV1-24).

## 5. Enemy defeat

```cpp
enum class DefeatCause {
    STOMP,
    SHELL,
    FIREBALL,
    STAR,
    PIT
};
```

## 6. Ownership

- `EntityFactory` trả `std::unique_ptr<Entity>` (S6-TV1-22).
- Raw pointer chỉ dùng làm non-owning reference.
- Box2D body không destroy trực tiếp trong contact callback.

---

## Quy trình thay đổi

1. Mọi thay đổi interface phải tạo PR ghi rõ lý do.
2. TV1 review và approve trước khi merge.
3. Cập nhật file này sau khi approve.