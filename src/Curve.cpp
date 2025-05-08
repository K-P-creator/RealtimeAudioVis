#include "../include/Curve.hpp"

sf::Curve::Curve(const sf::Vector2f startPosition, const sf::Vector2f endPosition, const sf::Vector2f controlPosition,
	const unsigned int vertexCount, const float thickness):
	m_startPosition(startPosition),
	m_endPosition(endPosition),
	m_controlPosition(controlPosition),
	m_vertexCount(vertexCount),
	m_vertices(sf::PrimitiveType::LineStrip, vertexCount),
	m_thickness(thickness)
{
	m_color = sf::Color::White; // Default color is white
	if (m_thickness > 0.1f) m_thickness = true;
	else m_thickness = false;
	updateVertices();
}


// Equation used: B(t) = (1 - t)^2 * P0 + 2 * (1 - t) * t * P1 + t^2 * P2 from t = [0, 1]
// B(t) being the point of the vertex, p0 being the start point, p1 being control, and p2 as the end
void sf::Curve::updateVertices() {
	const float step = 1.0f / (m_vertexCount - 1);

	for (unsigned int i = 0; i < m_vertexCount; i++) {
		float t = i * step;
		float u = 1.0f - t;

		// Calculate the vertex position using the quadratic Bezier curve formula
		sf::Vector2f point = u * u * m_startPosition + 2 * u * t * m_controlPosition + t * t * m_endPosition;
		m_vertices[i].position = point;
		m_vertices[i].color = m_color;
	}
}


void sf::Curve::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();

	// Draw the curve as a line strip if not thick
	if (!isThick) target.draw(m_vertices, states);

	// Draw the curve as rectangles with thickness otherwise
	else {
		sf::RectangleShape r;
		r.setFillColor(m_color);

		for (unsigned int i = 0; i < m_vertexCount - 1; i++) {
			float xDiff = m_vertices[i + 1].position.x - m_vertices[i].position.x;
			float yDiff = m_vertices[i + 1].position.y - m_vertices[i].position.y;

			r.setPosition(m_vertices[i].position);
			r.setRotation(sf::degrees(atan2(yDiff, xDiff)));
			r.setSize({ m_thickness, std::max(sqrt(xDiff * xDiff + yDiff * yDiff), m_thickness) });

			target.draw(r, states);
		}
	}
}
