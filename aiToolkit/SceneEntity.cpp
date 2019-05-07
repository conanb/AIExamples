#include "SceneEntity.h"
#include <Renderer2D.h>
#include <Texture.h>

namespace ai {

	SpriteEntity::~SpriteEntity() {
		delete m_texture;
	}

	bool SpriteEntity::load(const char* filename) {
		delete m_texture;
		m_texture = nullptr;
		m_texture = new app::Texture(filename);
		return m_texture != nullptr;
	}

	void SpriteEntity::onDraw(app::Renderer2D* renderer) {
		renderer->drawSpriteTransformed3x3(m_texture,
			(float*)& m_globalTransform, 128, 128);
	}
}