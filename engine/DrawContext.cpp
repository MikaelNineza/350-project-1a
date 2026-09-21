#include "DrawContext.h"

namespace CMPUT350 {

DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textToDraw(*mFont);
    textToDraw.setString(text);
    textToDraw.setFillColor(sf::Color(c.r, c.g, c.b));
    textToDraw.setPosition(sf::Vector2f(p.x, p.y));
    textToDraw.setLineAlignment(sf::Text::LineAlignment::Center);

    mWindow->draw(textToDraw);
}

void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textToDraw(*mFont);
    textToDraw.setString(text);
    textToDraw.setFillColor(sf::Color(c.r, c.g, c.b));
    textToDraw.setPosition(sf::Vector2f(p.x, p.y));

    mWindow->draw(textToDraw);
}

void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape circle(radius);
    circle.setPosition(sf::Vector2f(p.x, p.y));
    circle.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(circle);
}

void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape rectangle({r.width, r.height});
    rectangle.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(rectangle);
}

void DrawContext::FrameRect(Rect r, float width, RGBColor c) {}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {}

int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
