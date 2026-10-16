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
 * @brief Draws text whose lines are centered relative to each other.
 *
 * @param text The string to draw. May contain several lines separated by '\n'.
 * @param pixelSize The character size of the text, in pixels.
 * @param p The top-left position of the text block, in context coordinates.
 * @param c The color of the text.
 *
 * Only the line alignment is centered: each line is centered within the width of the
 * widest line. The text block itself is not centered on p; p is its top-left corner,
 * as in DrawText.
 */
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textToDraw(*mFont);
    p = Transform(p);
    textToDraw.setString(text);
    textToDraw.setFillColor(sf::Color(c.r, c.g, c.b));
    textToDraw.setPosition(sf::Vector2f(p.x, p.y));
    textToDraw.setLineAlignment(sf::Text::LineAlignment::Center);
    textToDraw.setRotation(sf::radians(mRotation));
    textToDraw.setCharacterSize(pixelSize);

    mWindow->draw(textToDraw);
}

/**
 * @brief Draws text with its top-left corner at a given point.
 *
 * @param text The string to draw.
 * @param pixelSize The character size of the text, in pixels.
 * @param p The top-left position of the text, in context coordinates.
 * @param c The color of the text.
 *
 * The text is rotated by the context's rotation.
 */
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textToDraw(*mFont);
    p = Transform(p);
    textToDraw.setString(text);
    textToDraw.setFillColor(sf::Color(c.r, c.g, c.b));
    textToDraw.setPosition(sf::Vector2f(p.x, p.y));
    textToDraw.setRotation(sf::radians(mRotation));
    textToDraw.setCharacterSize(pixelSize);

    mWindow->draw(textToDraw);
}

/**
 * @brief Draws a filled circle centered on a given point.
 *
 * @param p The center of the circle, in context coordinates.
 * @param radius The radius of the circle, in pixels.
 * @param c The fill color of the circle.
 */
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape circle(radius);
    p = Transform(p);
    circle.setPosition(sf::Vector2f(p.x, p.y));
    circle.setOrigin({radius, radius});
    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(circle);
}

/**
 * @brief Draws a filled rectangle.
 *
 * @param r The rectangle to draw, given by its top-left corner (in context coordinates),
 *          width and height in pixels.
 * @param c The fill color of the rectangle.
 *
 * The rectangle is rotated about its top-left corner by the context's rotation, so it is
 * only axis-aligned when the rotation is 0.
 */
void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape rectangle({r.width, r.height});
    Point2D topLeft = Transform(r.topLeft);
    rectangle.setPosition({topLeft.x, topLeft.y});
    rectangle.setRotation(sf::radians(mRotation));
    rectangle.setFillColor(sf::Color(c.r, c.g, c.b));

    mWindow->draw(rectangle);
}

/**
 * @brief Draws the outline of a rectangle with no fill.
 *
 * @param r The rectangle to outline, given by its top-left corner (in context coordinates),
 *          width and height in pixels.
 * @param width The thickness of the outline, in pixels.
 * @param c The color of the outline.
 *
 * The outline is drawn outside the edges of r, so the drawn frame extends
 * width pixels beyond r on every side. Like DrawRect, the rectangle is rotated about its
 * top-left corner by the context's rotation.
 */
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    sf::RectangleShape rectangle({r.width, r.height});
    Point2D topLeft = Transform(r.topLeft);
    rectangle.setPosition({topLeft.x, topLeft.y});
    rectangle.setRotation(sf::radians(mRotation));
    rectangle.setOutlineColor(sf::Color(c.r, c.g, c.b));
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineThickness(width);

    mWindow->draw(rectangle);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line, in context coordinates.
 * @param to The ending point of the line, in context coordinates.
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
    from = Transform(from);
    to = Transform(to);

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

/**
 * @brief Sets where the origin of the context coordinates is placed in the window.
 *
 * @param p The window position, in pixels, that the context point (0, 0) maps to.
 *          The engine sets this to the location of the object being drawn.
 */
void DrawContext::SetContextOffset(Point2D p) {
    mOffset = p;
}

/**
 * @brief Sets how much context coordinates are rotated about the context origin.
 *
 * @param rotation The rotation, in radians. Because y increases downward in the window,
 *                 a positive rotation turns clockwise on screen.
 */
void DrawContext::SetContextRotation(float rotation) {
    // Note for future: this might not work since positive y goes down.
    // After we complete the implementation if rotation seems wrong, fix here!
    mRotation = rotation;
}

/**
 * @brief Converts a point from context coordinates to window coordinates.
 *
 * @param p The point in context coordinates.
 * @return The point rotated by the context rotation about the origin, then moved by the
 *         context offset.
 */
Point2D DrawContext::Transform(Point2D p) {
    Point2D pNew;
    pNew.x = p.x * cos(mRotation) - p.y * sin(mRotation) + mOffset.x;
    pNew.y = p.x * sin(mRotation) + p.y * cos(mRotation) + mOffset.y;

    return pNew;
}

/**
 * @brief Converts a point from window coordinates to context coordinates.
 *
 * @param p The point in window coordinates.
 * @return The point moved back by the context offset, then rotated by the negative of the
 *         context rotation. This is the inverse of Transform(Point2D).
 */
Point2D DrawContext::ReverseTransform(Point2D p) {
    Point2D pNew;
    float tempX = p.x - mOffset.x;
    float tempY = p.y - mOffset.y;

    pNew.x = (tempX * cos(mRotation) + tempY * sin(mRotation));
    pNew.y = (tempY * cos(mRotation) - tempX * sin(mRotation));

    return pNew;
}

/**
 * @brief Converts a rectangle from context coordinates to a window-space bounding box.
 *
 * @param r The rectangle in context coordinates.
 * @return The smallest axis-aligned rectangle, in window coordinates, that contains all
 *         four transformed corners of r. When the rotation is not a multiple of 90 degrees,
 *         the result is larger than r.
 */
Rect DrawContext::Transform(Rect r) {
    Point2D topLeft = r.topLeft;
    Point2D topRight = topLeft;
    Point2D bottomLeft = topLeft;
    Point2D bottomRight = topLeft;

    topRight += {r.width, 0};
    bottomLeft += {0, r.height};
    bottomRight += {r.width, r.height};

    topLeft = Transform(topLeft);
    topRight = Transform(topRight);
    bottomLeft = Transform(bottomLeft);
    bottomRight = Transform(bottomRight);

    float minX = std::min({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
    float minY = std::min({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});
    float maxX = std::max({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
    float maxY = std::max({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});

    Rect rNew(minX, minY, maxX-minX, maxY-minY);

    return rNew;
}

}  // namespace CMPUT350
