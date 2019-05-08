#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "NavMesh.h"
#include "Entity.h"

class NavMeshApp : public app::Application {
public:

	NavMeshApp();
	virtual ~NavMeshApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*			m_font;

	NavMesh* m_navMesh;

	ai::Entity m_player;

	std::list<graph::Node*> m_path;
	std::list<glm::vec3> m_smoothPath;
};