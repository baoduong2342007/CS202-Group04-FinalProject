# Phân vai nhóm — 5 thành viên

> Mỗi người "sở hữu" 1 module: tự thiết kế, tự code, tự test module đó.  
> Khi cần phối hợp qua module khác → trao đổi trước, không tự ý sửa code của người khác.

---

## TV1 — Architect / Team Lead

**Trách nhiệm chính:**
- Thiết kế tổng thể OOP: class hierarchy, interface giữa các module
- Viết & maintain class diagram (draw.io / PlantUML)
- Chịu trách nhiệm implement đúng **5 design patterns** (đảm bảo 25 điểm rubric)
- Code review cuối mỗi tuần: kiểm tra các bạn có đi đúng hướng không
- Điều phối khi 2 module cần kết nối với nhau

**Module sở hữu:**
- `GameManager` (Singleton) — điều phối vòng lặp game
- `EventBus` / `Observer` — hệ thống sự kiện toàn game
- `GameState` machine (State pattern) — menu / playing / pause / gameover

**Interfaces TV1 phải expose cho các thành viên khác:**
```cpp
// TV2 dùng để chuyển state
GameManager::getInstance().changeState(new PlayState());

// TV3/TV4/TV5 dùng để phát sự kiện
EventBus::getInstance().publish(Event::COIN_COLLECTED, data);

// TV2 dùng để lắng nghe sự kiện
EventBus::getInstance().subscribe(Event::PLAYER_DIED, callback);
```

---

## TV2 — Engine & Render

**Trách nhiệm chính:**
- Setup SFML: window, game loop, delta time
- Hệ thống render: sprite, texture manager, z-ordering
- `AnimationSystem`: sprite sheet, frame timing
- `Camera`: follow Mario, clamp theo level boundary
- Màn hình UI: Menu, GameOver, Win screen, transitions

**Module sở hữu:**
- `Game` — main loop, window, delta time
- `TextureManager` — load và cache texture
- `AnimationSystem` — quản lý animation clip
- `Camera` — viewport follow player
- `MenuState`, `GameOverState`, `WinState` — UI screens

**Output tuần 2 TV3 cần:**
```cpp
// TV2 cung cấp sẵn để TV3 dùng
void Entity::setSprite(const std::string& texturePath);
void Entity::playAnimation(const std::string& clipName);
```

---

## TV3 — Mario & Physics

**Trách nhiệm chính:**
- Class `Mario`: movement, jump, gravity, power-up states
- `PhysicsEngine`: gravity, velocity, acceleration
- `CollisionManager`: AABB collision detection, resolution
- Power-up system: Small → Big → Fire (cả hiệu ứng biến đổi)
- `FireBall` projectile nếu có FireFlower

**Module sở hữu:**
- `Entity` (base class cho tất cả object có vật lý)
- `Character` (abstract, kế thừa Entity)
- `Mario` (kế thừa Character)
- `PhysicsEngine`
- `CollisionManager`
- `FireBall`

**Collision interface TV4 cần:**
```cpp
// TV3 expose để TV4 dùng cho enemy
CollisionManager::checkCollision(Entity* a, Entity* b);
CollisionManager::resolveCollision(Entity* a, Entity* b, Direction dir);
```

---

## TV4 — Level & Enemy

**Trách nhiệm chính:**
- Format và load tilemap từ file `.txt` (định nghĩa format từ tuần 1)
- Thiết kế và dựng 3 level có độ khó tăng dần
- `Goomba`: AI patrol qua lại, bị giẫm chết
- `Koopa`: AI patrol, bị giẫm thành shell, shell có thể trượt
- `SaveManager`: save/load điểm cao qua file

**Module sở hữu:**
- `Level` — load, update, render 1 màn chơi
- `TileMap` — parse file, render tile
- `Enemy` (abstract, kế thừa Character)
- `Goomba`, `Koopa` (kế thừa Enemy)
- `SaveManager`
- File dữ liệu: `levels/level1.txt`, `level2.txt`, `level3.txt`

**Format file level (TV4 định nghĩa, cả nhóm tuân theo):**
```
# level1.txt — 0=empty, 1=ground, 2=brick, 3=coin, G=Goomba, K=Koopa, M=Mario spawn
0 0 0 0 0 0 0 0 0
0 0 0 3 0 0 0 0 0
0 0 0 1 0 0 0 0 0
1 1 G 1 1 0 K 0 M
```

---

## TV5 — UI, Sound & Items

**Trách nhiệm chính:**
- `SoundManager` (Singleton): load và play sound effect + nhạc nền
- `HUD`: hiển thị score, lives, thời gian, power-up indicator
- Tất cả items: `Coin`, `Mushroom`, `FireFlower`, `Star`
- `InputHandler` với Command pattern: map phím → action
- Thu thập và tổ chức assets (texture, sound) vào đúng thư mục

**Module sở hữu:**
- `SoundManager` (Singleton)
- `HUD`
- `Item` (abstract base)
- `Coin`, `Mushroom`, `FireFlower`, `Star`
- `ICommand`, `InputHandler` (Command pattern)
- Thư mục `assets/`

**Command pattern TV5 expose cho TV3:**
```cpp
// TV3 dùng InputHandler để lấy action
auto action = inputHandler.getAction(sf::Keyboard::Space);
if (action) action->execute(mario);
```

---

## Quy tắc phối hợp

1. **Không commit vào module của người khác** khi chưa báo trước.
2. **Interface thay đổi → thông báo cả nhóm** trước khi merge vào `develop`.
3. **Cuối tuần thứ Sáu:** TV1 merge `develop` vào `main`, tất cả cùng build lại để xác nhận không broken.
4. **TV1 có quyền quyết định** khi xảy ra conflict về design. Ý kiến khác → raise trong meeting, không tự sửa.
