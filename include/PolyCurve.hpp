// PolyCurve will create a complex curve that will pull towards 4+ points. It uses
// a cubic Bezier curve that requires at least 4 points to draw one curve.

#ifndef POLY_CURVE_HPP
#define POLY_CURVE_HPP

#include <SFML/Graphics.hpp>


namespace sf {
	class PolyCurve : public sf::Drawable, public sf::Transformable
	{
	public:
		PolyCurve(const std::vector <sf::Vector2f>, const unsigned int vertexCount = 10, const float thickness = 10);
		void setColor(const sf::Color);
		void setVertexCount(const unsigned int count);
		void setPoints(const std::vector <sf::Vector2f>);
		void setThickness(const float thickness);

	private:
		sf::VertexArray m_vertices;

		float m_thickness;
		bool isThick;

		std::vector <sf::Vector2f> m_points;

		unsigned int m_vertexCount;
		sf::Color m_color;

		void updateVertices();
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};
}



inline void sf::PolyCurve::setColor(const sf::Color color)
{
	m_color = color;
	for (unsigned int i = 0; i < m_vertexCount; i++)
	{
		m_vertices[i].color = m_color;
	}
}

inline void sf::PolyCurve::setVertexCount(const unsigned int count)
{
	m_vertexCount = count;
	m_vertices.resize(m_vertexCount);
	updateVertices();
}

inline void sf::PolyCurve::setPoints(const std::vector<sf::Vector2f> points)
{
	m_points = points;
	updateVertices();
}

inline void sf::PolyCurve::setThickness(const float thickness)
{
	m_thickness = thickness;
	if (m_thickness > 0.1f) isThick = true;
	else isThick = false;
}

#endif // POLY_CURVE_HPP