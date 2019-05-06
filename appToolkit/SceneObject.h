#pragma once

#include "Matrix.h"
#include "Renderer2D.h"
#include <vector>
#include <cassert>

class SceneObject {
public:

	SceneObject() {}
	virtual ~SceneObject() { 
		// detach from parent
		if (m_parent != nullptr)
			m_parent->removeChild(this);

		// remove all children
		for (auto child : m_children)
			child->m_parent = nullptr;
	}

	SceneObject* getParent() const { return m_parent; }

	size_t childCount() const { return m_children.size(); }

	SceneObject* getChild(unsigned int index) const { return m_children[index]; }

	void SceneObject::addChild(SceneObject* child) {
		// make sure it doesn't have a parent already
		assert(child->m_parent == nullptr);
		// assign this as parent
		child->m_parent = this;
		// add new child to collection
		m_children.push_back(child);

		// update global transform of child
		child->updateTransform();
	}

	void SceneObject::removeChild(SceneObject* child) {
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

	void SceneObject::update(float deltaTime) {
		// run onUpdate behaviour
		onUpdate(deltaTime);
		
		// update children
		for (auto child : m_children)
			child->update(deltaTime);
	}

	void SceneObject::draw(aie::Renderer2D* renderer) {
		// run onDraw behaviour
		onDraw(renderer);

		// draw children
		for (auto child : m_children)
			child->draw(renderer);
	}

	virtual void SceneObject::onUpdate(float deltaTime) {
	}

	virtual void SceneObject::onDraw(aie::Renderer2D* renderer) {
	}

	void SceneObject::updateTransform() {

		if (m_parent != nullptr)
			m_globalTransform = m_parent->m_globalTransform * m_localTransform;
		else
			m_globalTransform = m_localTransform;

		for (auto child : m_children)
			child->updateTransform();
	}

	void SceneObject::setPosition(float x, float y) {
		m_localTransform[2] = { x, y, 1 };
		updateTransform();
	}

	void SceneObject::setRotate(float radians) {
		m_localTransform.setRotateZ(radians);
		updateTransform();
	}

	void SceneObject::setScale(float width, float height) {
		m_localTransform.setScaled(width, height, 1);
		updateTransform();
	}

	void SceneObject::translate(float x, float y) {
		m_localTransform.translate( x, y );
		updateTransform();
	}

	void SceneObject::rotate(float radians) {
		m_localTransform.rotateZ(radians);
		updateTransform();
	}

	void SceneObject::scale(float width, float height) {
		m_localTransform.scale(width, height, 1);
		updateTransform();
	}

	const Matrix3& SceneObject::getLocalTransform() const { return m_localTransform; }
	const Matrix3& SceneObject::getGlobalTransform() const { return m_globalTransform; }

protected:

	SceneObject* m_parent = nullptr;
	std::vector<SceneObject*> m_children;

	Matrix3 m_localTransform = Matrix3::identity;
	Matrix3 m_globalTransform = Matrix3::identity;
};

#include "Texture.h"

class SpriteObject : public SceneObject {
public:

	SpriteObject() {}
	SpriteObject(const char* filename) { load(filename); }
	virtual ~SpriteObject() { delete m_texture; }

	bool load(const char* filename) {
		delete m_texture;
		m_texture = nullptr;
		m_texture = new aie::Texture(filename);
		return m_texture != nullptr;
	}

	virtual void SpriteObject::onDraw(aie::Renderer2D* renderer) {
		renderer->drawSpriteTransformed3x3(m_texture, 
			(float*)&m_globalTransform, 128, 128);
	}

protected:

	aie::Texture* m_texture = nullptr;
};