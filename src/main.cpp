#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Định nghĩa kích thước cửa sổ chuẩn HD 16:9
    const unsigned int windowWidth = 1280;
    const unsigned int windowHeight = 720;

    // Tạo cửa sổ game cố định kích thước (1280x720) và không cho thay đổi kích cỡ
    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}), 
        "Super Mario - Test SFML", 
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    // Kích thước và màu sắc hình tròn kiểm tra
    const float radius = 100.f;
    sf::CircleShape shape(radius);
    shape.setFillColor(sf::Color(46, 204, 113)); // Hài hòa hơn màu xanh lá mặc định
    
    // Căn giữa hình tròn động dựa trên kích thước cửa sổ
    shape.setPosition({
        (windowWidth / 2.f) - radius,
        (windowHeight / 2.f) - radius
    });

    std::cout << "Starting SFML window. Press ESC or close the window to exit." << std::endl;

    // Vòng lặp game (Game Loop) theo chuẩn SFML 3.0.0
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            // Nhấn nút X ở góc cửa sổ để thoát
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // Nhấn phím Escape để thoát
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        // Xóa màn hình với tông màu tối dễ chịu
        window.clear(sf::Color(30, 30, 30));

        // Vẽ hình tròn test
        window.draw(shape);

        // Hiển thị khung hình mới
        window.display();
    }

    std::cout << "Window closed successfully." << std::endl;
    return 0;
}
