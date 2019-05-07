#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace ai {

enum class eBlackboardDataType {
	UNKNOWN = 0,
	INT,
	UINT,
	BOOL,
	FLOAT,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	MATRIX33,
	MATRIX44,
	QUATERNION,

	POINTER,
	OWNEDPOINTER,
};

class Blackboard;
class BlackboardQuestion;

class BlackboardExpert {
public:

	BlackboardExpert() {}
	virtual ~BlackboardExpert() {}

	virtual float	evaluateResponse(BlackboardQuestion* question, Blackboard* blackboard) = 0;

	virtual void	execute(BlackboardQuestion* question, Blackboard* blackboard) = 0;
};

class BlackboardQuestion {
public:

	BlackboardQuestion(int id) : m_id(id) {};
	virtual ~BlackboardQuestion() {}

	int getType() const { return m_id; }

	void addExpert(BlackboardExpert* expert) { m_experts.push_back(expert); }

	void clearExperts() { m_experts.clear(); }

protected:

	friend class Blackboard;

	bool arbitrate(Blackboard* blackboard);

	int	m_id;
	std::vector<BlackboardExpert*>	m_experts;
};

class Blackboard {
public:

	Blackboard() {}
	~Blackboard() { clearData(); }

	void	clearData();

	void	remove(const std::string& name);

	bool	contains(const std::string& name) const;

	eBlackboardDataType	getType(const std::string& name) const;

	// returns false if exists but different type
	bool	set(const std::string& name, int value);
	bool	set(const std::string& name, unsigned int value);
	bool	set(const std::string& name, bool value);
	bool	set(const std::string& name, float value);
	bool	set(const std::string& name, const glm::vec2& value);
	bool	set(const std::string& name, const glm::vec3& value);
	bool	set(const std::string& name, const glm::vec4& value);
	bool	set(const std::string& name, const glm::mat3& value);
	bool	set(const std::string& name, const glm::mat4& value);
	bool	set(const std::string& name, const glm::quat& value);

	// return false if doesn't exist or wrong type
	bool	get(const std::string& name, int& value);
	bool	get(const std::string& name, unsigned int& value);
	bool	get(const std::string& name, bool& value);
	bool	get(const std::string& name, float& value);
	bool	get(const std::string& name, glm::vec2& value);
	bool	get(const std::string& name, glm::vec3& value);
	bool	get(const std::string& name, glm::vec4& value);
	bool	get(const std::string& name, glm::mat3& value);
	bool	get(const std::string& name, glm::mat4& value);
	bool	get(const std::string& name, glm::quat& value);

	template <typename T>
	bool	set(const std::string& name, T* value, bool own = false) {
		auto iter = m_data.find(name);

		if (iter == m_data.end()) {
			BlackboardData data;
			data.type = own ? eBlackboardDataType::OWNEDPOINTER : eBlackboardDataType::POINTER;
			data.p = value;

			m_data.insert(std::make_pair(name, data));
		}
		else {
			// make sure we're replacing a pointer
			// if it was an owned pointer, delete it first
			if (iter->second.type != eBlackboardDataType::POINTER &&
				iter->second.type != eBlackboardDataType::OWNEDPOINTER)
				return false;

			if (iter->second.type == eBlackboardDataType::OWNEDPOINTER)
				delete iter->second.p;

			iter->second.type = own ? eBlackboardDataType::OWNEDPOINTER : eBlackboardDataType::POINTER;
			iter->second.p = value;
		}

		return true;
	}

	template <typename T>
	bool	get(const std::string& name, T** value) {
		auto iter = m_data.find(name);

		if (iter == m_data.end() ||
			!(iter->second.type == eBlackboardDataType::POINTER ||
			  iter->second.type == eBlackboardDataType::OWNEDPOINTER))
			return false;

		*value = (T*)iter->second.p;
		return true;
	}

	// arbitration
	void	addQuestion(BlackboardQuestion* question) { m_questions.push_back(question);  }
	void	removeQuestion(BlackboardQuestion* question) { m_questions.remove(question); }
	void	clearQuestions() { m_questions.clear(); }

	std::list<BlackboardQuestion*>& getQuestions() { return m_questions; }

	void runArbitration();

private:

	struct BlackboardData {
		eBlackboardDataType type;

		union {
			float f;
			int i;
			unsigned int ui;
			bool b;

			glm::vec2 v2;
			glm::vec3 v3;
			glm::vec4 v4;
			glm::mat3 m3;
			glm::mat4 m4;
			glm::quat q;

			void* p;
		};
	};

	std::map<std::string, BlackboardData>	m_data;

	std::list<BlackboardQuestion*>	m_questions;
};

} // namespace ai