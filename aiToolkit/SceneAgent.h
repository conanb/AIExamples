#pragma once

#include "Agent.h"
#include <vector>
#include <cassert>

namespace app {
	class Renderer2D;
	class Texture;
}

namespace ai {

	class SceneAgent : public Agent {
	public:

		SceneAgent() {}
		virtual ~SceneAgent() {
			// detach from parent
			if (m_parent != nullptr)
				m_parent->removeChild(this);

			// remove all children
			for (auto child : m_children)
				child->m_parent = nullptr;
		}

		SceneAgent* getParent() const { return m_parent; }

		size_t childCount() const { return m_children.size(); }

		SceneAgent* getChild(unsigned int index) const { return m_children[index]; }

		void addChild(SceneAgent* child) {
			// make sure it doesn't have a parent already
			assert(child->m_parent == nullptr);
			// assign this as parent
			child->m_parent = this;
			// add new child to collection
			m_children.push_back(child);

			// update global transform of child
			child->updateTransform();
		}

		void removeChild(SceneAgent* child) {
			// find the child in the collection
			auto iter = std::find(m_children.begin(), m_children.end(), child);

			// if found, remove it
			if (iter != m_children.end()) {
				m_children.erase(iter);
				// also unassign parent
				child->m_parent = nullptr;

				// update global transform of child
				child->updateTransform();
			}
		}

		virtual void executeBehaviours() {
			// run behaviours
			Agent::executeBehaviours();

			// update children
			for (auto child : m_children)
				child->executeBehaviours();
		}

		void draw(app::Renderer2D* renderer) {
			// run onDraw behaviour
			onDraw(renderer);

			// draw children
			for (auto child : m_children)
				child->draw(renderer);
		}

		virtual void onDraw(app::Renderer2D * renderer) {}

		void updateTransform() {

			if (m_parent != nullptr)
				m_globalTransform = m_parent->m_globalTransform * m_transform;
			else
				m_globalTransform = m_transform;

			for (auto child : m_children)
				child->updateTransform();
		}

		virtual void setPosition(const glm::vec3& v) {
			m_transform[3] = { v, 1 };
			updateTransform();
		}

		virtual void translate(const glm::vec3& v) {
			m_transform[3] += glm::vec4(v,0);
			updateTransform();
		}

		const glm::mat4& getGlobalTransform() const { return m_globalTransform; }

	protected:

		SceneAgent* m_parent = nullptr;
		std::vector<SceneAgent*> m_children;

		glm::mat4 m_globalTransform = glm::mat4(1);
	};

	class SpriteAgent : public SceneAgent {
	public:

		SpriteAgent() {}
		SpriteAgent(const char* filename) { load(filename); }
		virtual ~SpriteAgent();

		bool load(const char* filename);

		virtual void onDraw(app::Renderer2D* renderer);

	protected:

		app::Texture* m_texture = nullptr;
	};

} // namespace app