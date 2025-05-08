#ifndef CURVE_HPP
#define CURVE_HPP

#include <SFML/Graphics.hpp>


namespace sf{
	class Curve : public sf::Drawable, public sf::Transformable
	{
	public:
		Curve(const sf::Vector2f startPosition, const sf::Vector2f endPosition, const sf::Vector2f controlPosition, const unsigned int vertexCount = 10, const float thickness=10);
		void setColor(const sf::Color);
		void setVertexCount(const unsigned int count);
		void setPoints(const sf::Vector2f startPosition, const sf::Vector2f endPosition, const sf::Vector2f controlPosition);
		void setThickness(const float thickness);

	private:
		sf::VertexArray m_vertices;

		float m_thickness;
		bool isThick;
		
		sf::Vector2f m_startPosition;
		sf::Vector2f m_endPosition;
		sf::Vector2f m_controlPosition;

		unsigned int m_vertexCount;
		sf::Color m_color;

		void updateVertices();
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};
}



inline void sf::Curve::setColor(const sf::Color color)
{
	m_color = color;
	for (unsigned int i = 0; i < m_vertexCount; i++)
	{
		m_vertices[i].color = m_color;
	}
}

inline void sf::Curve::setVertexCount(const unsigned int count)
{
	m_vertexCount = count;
	m_vertices.resize(m_vertexCount);
	updateVertices();
}

inline void sf::Curve::setPoints(const sf::Vector2f startPosition, const sf::Vector2f endPosition, const sf::Vector2f controlPosition)
{
	m_startPosition = startPosition;
	m_endPosition = endPosition;
	m_controlPosition = controlPosition;
	updateVertices();
}

inline void sf::Curve::setThickness(const float thickness)
{
	m_thickness = thickness;
	if (m_thickness > 0.1f) isThick = true;
	else isThick = false;
}

#endif // CURVE_HPP