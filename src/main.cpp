#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Define standard HD 16:9 window size
    const unsigned int windowWidth = 1280;
    const unsigned int windowHeight = 720;

    // Create a game window with fixed size (1280x720) and disable resizing
    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}), 
        "Super Mario - Test SFML", 
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    // Circle size and color for testing
    const float radius = 100.f;
    sf::CircleShape shape(radius);
    shape.setFillColor(sf::Color(46, 204, 113)); // More pleasant than the default green color
    
    // Center the circle dynamically based on the window size
    shape.setPosition({
        (windowWidth / 2.f) - radius,
        (windowHeight / 2.f) - radius
    });

    std::cout << "Starting SFML window. Press ESC or close the window to exit." << std::endl;

    // Game loop following SFML 3.0.0 standards
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            // Close the window when clicking the X button
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // Close the window when pressing the Escape key
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        // Clear the screen with a pleasant dark color
        window.clear(sf::Color(30, 30, 30));

        // Draw the test circle
        window.draw(shape);

        // Display the new frame
        window.display();
    }

    std::cout << "Window closed successfully." << std::endl;
    return 0;
}
