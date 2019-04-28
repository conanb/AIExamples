#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "NavMesh.h"
#include "Entity.h"

class NavMeshApp : public :Application {
public:

	NavMeshApp();
	virtual ~NavMeshApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	NavMesh* m_navMesh;

	Entity m_player;

	std::list<Pathfinding::Node*> m_path;
	std::list<Vector2> m_smoothPath;
};