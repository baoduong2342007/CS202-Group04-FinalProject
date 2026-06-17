# Quy tắc chung — Coding & Git

> Đây là luật của nhóm. Ai vi phạm → TV1 có quyền yêu cầu làm lại trước khi merge.

---

## 1. Git workflow

### Nhánh (branch)

```
main        ← chỉ chứa code đã ổn, merge vào cuối mỗi tuần
develop     ← nhánh tích hợp chính, mọi người merge feature vào đây
feature/... ← nhánh làm việc hàng ngày
fix/...     ← nhánh vá bug
```

**Quy trình:**
```
1. Bắt đầu việc mới:
   git checkout develop
   git pull
   git checkout -b feature/goomba-ai

2. Làm xong → commit:
   git add .
   git commit -m "[TV4] Add Goomba patrol AI with direction reversal"

3. Push lên remote:
   git push origin feature/goomba-ai

4. Tạo Pull Request vào develop trên GitHub
   → Tag 1 người review (ưu tiên TV1 nếu là core class)
   → Sau khi được approve → merge → xóa branch

5. Cuối tuần (thứ Sáu):
   TV1 merge develop vào main
   Cả nhóm pull main về và build lại để xác nhận
```

### Format commit message

```
[TVx] <Động từ> <Mô tả ngắn>

Ví dụ tốt:
[TV3] Add gravity and vertical velocity to Mario
[TV2] Fix camera clamp when Mario reaches level boundary
[TV1] Implement Observer pattern with EventBus singleton
[TV4] Design Level 2 tilemap with increasing difficulty

Ví dụ xấu (không dùng):
fix stuff
update code
TV4 done
wip
```

### Quy tắc commit

- **Commit thường xuyên**, sau mỗi unit việc nhỏ — không gom 8 tiếng vào 1 commit
- **Không commit code không compile** vào `develop`
- **Không commit file build** (`/build`, `.exe`, `.o`) — đã có `.gitignore`
- **Không commit file tạm** (`test_main.cpp`, `debug.txt`, `TODO.md` cá nhân)

---

## 2. C++ Coding conventions

### Naming

```cpp
// Class → PascalCase
class GameManager { };
class TileMap { };

// Method → camelCase
void update(float deltaTime);
bool loadFromFile(const std::string& path);

// Variable → camelCase
int playerScore = 0;
float jumpVelocity = -500.f;

// Private member → m_ prefix
class Mario {
private:
    sf::Vector2f m_position;
    int m_health;
    MarioState m_state;
};

// Constant & enum value → UPPER_SNAKE_CASE
const int TILE_SIZE = 32;
const int MAX_LIVES = 3;

enum class MarioState { SMALL, BIG, FIRE };
// dùng scoped enum (enum class), không dùng enum thường

// File → tên class (1-1)
// GameManager → GameManager.h + GameManager.cpp
```

### Class structure — thứ tự viết

```cpp
class Mario : public Character {
public:
    // 1. Constructor / Destructor
    Mario();
    ~Mario() override;

    // 2. Override methods
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // 3. Public methods (alphabetical)
    void applyPowerUp(PowerUpType type);
    void handleInput(const InputHandler& input);
    void loseLife();

    // 4. Getters / Setters
    int getLives() const;
    sf::Vector2f getPosition() const;
    MarioState getState() const;

private:
    // 5. Private methods
    void jump();
    void applyGravity(float dt);
    void updateAnimation();

    // 6. Private members
    int m_lives;
    MarioState m_state;
    sf::Vector2f m_velocity;
};
```

### Con trỏ và memory

```cpp
// ƯU TIÊN smart pointer
std::unique_ptr<Enemy> enemy = std::make_unique<Goomba>(pos);
std::shared_ptr<Texture> tex = std::make_shared<sf::Texture>();

// Dùng raw pointer KHI NÀO: khi object không sở hữu bộ nhớ (observer pointer)
void setTarget(Mario* mario);  // OK — không sở hữu

// KHÔNG dùng new/delete trực tiếp trong game logic
// BAD:
Enemy* e = new Goomba(pos);  // ai delete?

// EXCEPTION: EntityFactory trả về raw pointer — caller sở hữu
// → Factory method: caller wrap vào unique_ptr ngay lập tức
auto enemy = std::unique_ptr<Enemy>(EntityFactory::create(EnemyType::GOOMBA, pos));
```

### Hàm

```cpp
// Tối đa 40–50 dòng mỗi hàm
// Nếu dài hơn → tách thành hàm private nhỏ hơn

// Hàm không thay đổi state → const
int getScore() const;
bool isAlive() const;

// Tham số lớn → pass by const reference
void loadLevel(const std::string& filename);
void render(const sf::RenderWindow& window) const;

// Trả về bool nếu có thể fail
bool loadTexture(const std::string& path);  // false nếu file không tồn tại
```

### Không dùng

```cpp
// Không dùng using namespace std; trong .h — ô nhiễm namespace
// BAD trong header:
using namespace std;

// OK trong .cpp (và chỉ ở đó):
using namespace std;  // chấp nhận được, nhưng ưu tiên viết std:: rõ ràng

// Không dùng magic number
// BAD:
if (mario.y > 720) mario.die();

// GOOD:
const float SCREEN_HEIGHT = 720.f;
if (mario.getY() > SCREEN_HEIGHT) mario.die();

// Không để code chết (commented-out code) trong PR
// BAD:
// void oldUpdate() { ... }   ← xóa đi
```

---

## 3. Header comment bắt buộc

Mỗi file `.cpp` phải có comment header như sau:

```cpp
/**
 * @file Goomba.cpp
 * @author TV4
 * @brief Goomba enemy: patrol AI, death on stomp, Factory-compatible
 * @note Tuần 3 — sử dụng EntityFactory để spawn
 */
```

---

## 4. Quy tắc về Design Patterns

Mỗi pattern phải được ghi nhận rõ trong code bằng comment:

```cpp
// ============================================================
// PATTERN: Factory Method
// Lý do: tránh hard-code new Goomba(), new Koopa() ở nhiều nơi;
//         cho phép thêm enemy type mới mà không sửa Level.cpp
// ============================================================
Enemy* EntityFactory::create(EnemyType type, sf::Vector2f pos) {
    switch (type) {
        case EnemyType::GOOMBA: return new Goomba(pos);
        case EnemyType::KOOPA:  return new Koopa(pos);
        default: return nullptr;
    }
}
```

5 pattern cần implement và ghi nhận:

| # | Pattern | File chính | Người implement |
|---|---|---|---|
| 1 | Factory | `EntityFactory.h/.cpp` | TV1 |
| 2 | Singleton | `GameManager.h`, `SoundManager.h` | TV1, TV5 |
| 3 | Observer | `EventBus.h`, `IObserver.h` | TV1 |
| 4 | State | `IGameState.h`, `*State.cpp` | TV1, TV2 |
| 5 | Command | `ICommand.h`, `InputHandler.h` | TV5 |

---

## 5. Quy tắc Pull Request

Trước khi tạo PR, tự kiểm tra:

- [ ] Code compile không warning
- [ ] Không có code chết (commented-out code vô nghĩa)
- [ ] Không có `std::cout` debug bừa bãi — dùng `#ifdef DEBUG` nếu cần
- [ ] Đã test tay scenario mình đang implement
- [ ] Tên branch và commit message đúng format

Người review kiểm tra:
- [ ] Logic đúng với thiết kế trong class diagram
- [ ] Không vi phạm quy tắc naming
- [ ] Không break build (checkout về branch đó, build thử)
- [ ] Pattern được dùng đúng nếu liên quan

---

## 6. Quy tắc họp nhóm

- **Họp ngắn đầu tuần** (~30 phút): mỗi người nói công việc tuần này là gì
- **Sync nhanh giữa tuần** (~15 phút): ai bị block → báo ngay để người khác hỗ trợ
- **Demo cuối tuần**: ngồi lại, chạy game, tick checklist — không bỏ bước này
- **Kênh liên lạc**: dùng Discord/Zalo riêng cho nhóm; đặt câu hỏi kỹ thuật trong thread riêng

---

## 7. Checklist trước khi nộp bài

Tuần 6 TV1 dùng checklist này:

- [ ] Build từ đầu trên máy sạch (chưa clone bao giờ) thành công
- [ ] Chạy game 3 level không crash
- [ ] `docs/class_diagram.png` có đủ tất cả class chính
- [ ] `docs/design_patterns.md` mô tả đủ 5 pattern
- [ ] Code có comment đủ (đặc biệt chỗ dùng pattern)
- [ ] Video demo quay xong, có giọng giải thích
- [ ] Rubric 65+35+15: mỗi dòng đã check done hoặc ghi rõ lý do bỏ
- [ ] Nộp đúng deadline, đúng định dạng yêu cầu
