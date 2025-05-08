#include "../include/PolyCurve.hpp"

sf::PolyCurve::PolyCurve(const std::vector <sf::Vector2f> points, unsigned int vertexCount, float thickness)
{
	m_points = points;
	m_color = sf::Color::White; // Default color is white
	if (m_thickness > 0.1f) m_thickness = true;
	else m_thickness = false;
	updateVertices();
}



void sf::PolyCurve::updateVertices() {
	const float step = 1.0f / (m_vertexCount - 1);

	
}


void sf::PolyCurve::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	
}
