#include "DrawContext.h"

namespace CMPUT350 {

/**
 * @brief Constructs a drawing context that renders to a shared window using a shared font.
 *
 * @param window The SFML render window to draw into. Shared with the game engine, which creates it.
 * @param font The font used by DrawText and DrawCenteredText. Shared with the game engine, which loads it.
 */
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

/**
 * @brief Draws text centered on a given point.
 *
 * @param text The string to draw.
 * @param pixelSize The character size of the text, in pixels.
 * @param p The point, in window pixel coordinates, that the center of the text is placed on.
 * @param c The color of the text.
 *
 * The text's origin is set to the center of its bounding box, so the text is
 * centered both horizontally and vertically on p.
 */
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textToDraw(*mFont);
    textToDraw.setString(text);
    textToDraw.setFillColor(sf::Color(c.r, c.g, c.b));
    textToDraw.setPosition(sf::Vector2f(p.x, p.y));
    textToDraw.setLineAlignment(sf::Text::LineAlignment::Center);
    textToDraw.setCharacterSize(pixelSize);

    mWindow->draw(textToDraw);
}

/**
 * @brief Draws text with its top-left corner at a given point.
 *
 * @param text The string to draw.
 * @param pixelSize The character size of the text, in pixels.
 * @param p The top-left position of the text, in window pixel coordinates.
 * @param c The color of the text.
 */
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textToDraw(*mFont);
    textToDraw.setString(text);
    textToDraw.setFillColor(sf::Color(c.r, c.g, c.b));
    textToDraw.setPosition(sf::Vector2f(p.x, p.y));
    textToDraw.setCharacterSize(pixelSize);

    mWindow->draw(textToDraw);
}

/**
 * @brief Draws a filled circle centered on a given point.
 *
 * @param p The center of the circle, in window pixel coordinates.
 * @param radius The radius of the circle, in pixels.
 * @param c The fill color of the circle.
 */
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape circle(radius);
    circle.setPosition(sf::Vector2f(p.x, p.y));
    circle.setOrigin({radius, radius});
    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(circle);
}

/**
 * @brief Draws a filled axis-aligned rectangle.
 *
 * @param r The rectangle to draw, given by its top-left corner, width and height in pixels.
 * @param c The fill color of the rectangle.
 */
void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape rectangle({r.width, r.height});
    rectangle.setPosition({r.topLeft.x, r.topLeft.y});
    rectangle.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(rectangle);
}

/**
 * @brief Draws the outline of an axis-aligned rectangle with no fill.
 *
 * @param r The rectangle to outline, given by its top-left corner, width and height in pixels.
 * @param width The thickness of the outline, in pixels.
 * @param c The color of the outline.
 *
 * The outline is drawn outside the edges of r, so the drawn frame extends
 * width pixels beyond r on every side.
 */
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    sf::RectangleShape rectangle({r.width, r.height});
    rectangle.setPosition({r.topLeft.x, r.topLeft.y});
    rectangle.setOutlineColor(sf::Color(c.r, c.g, c.b));
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineThickness(width);

    mWindow->draw(rectangle);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line, in window pixel coordinates.
 * @param to The ending point of the line, in window pixel coordinates.
 * @param width The thickness of the line, in pixels.
 * @param c The color of the line.
 *
 * SFML cannot draw lines with a width directly, so this builds a rectangle
 * as an sf::ConvexShape whose length is the distance between the two points.
 * The rectangle is pivoted on the midpoint of its left edge, placed at from,
 * and rotated by the angle from from to to, so the line is centered on the
 * from-to path with half its width on each side.
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    sf::ConvexShape line(4);
    float len = static_cast<float>(from.Distance(to));
    float angle = std::atan2(to.y - from.y, to.x - from.x);
    
    line.setPoint(0, {0.f, 0.f});
    line.setPoint(1, {len, 0.f});
    line.setPoint(2, {len, width});
    line.setPoint(3, {0.f, width});
    // Sets the midpoint of the left side as the pivot, moves it to "from" and rotates it towards "to"
    line.setOrigin({0.f, width / 2.f});
    line.setPosition({from.x, from.y});
    line.setRotation(sf::radians(angle));

    line.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(line);
}

/**
 * @brief Gets the width of the window being drawn to.
 *
 * @return The window width, in pixels.
 */
int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

/**
 * @brief Gets the height of the window being drawn to.
 *
 * @return The window height, in pixels.
 */
int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
