#include "Blackboard.h"

void Blackboard::clearData() {

	for (auto data : m_data)
		if (data.second.type == eBlackboardDataType::OWNEDPOINTER)
			delete data.second.p;

	m_data.clear();
}

void Blackboard::remove(const std::string& name) {

	auto iter = m_data.find(name);

	if (iter != m_data.end()) {
		m_data.erase(iter);
	}
}

eBlackboardDataType Blackboard::getType(const std::string& name) const {

	auto iter = m_data.find(name);

	if (iter != m_data.end()) {
		return iter->second.type;
	}
	return eBlackboardDataType::UNKNOWN;
}

bool Blackboard::contains(const std::string& name) const {
	return m_data.find(name) != m_data.end();
}

bool Blackboard::set(const std::string& name, int value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end()) {
		// adding new data
		BlackboardData data;
		data.type = eBlackboardDataType::INT;
		data.i = value;

		m_data.insert(std::make_pair(name, data));
	}
	else {
		if (iter->second.type != eBlackboardDataType::INT)
			return false;

		// replacing the value
		iter->second.i = value;
	}

	return true;
}

bool Blackboard::set(const std::string& name, unsigned int value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end()) {
		// adding new data
		BlackboardData data;
		data.type = eBlackboardDataType::UINT;
		data.ui = value;

		m_data.insert(std::make_pair(name, data));
	}
	else {
		if (iter->second.type != eBlackboardDataType::UINT)
			return false;

		// replacing the value
		iter->second.ui = value;
	}

	return true;
}

bool Blackboard::set(const std::string& name, bool value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end()) {
		// adding new data
		BlackboardData data;
		data.type = eBlackboardDataType::BOOL;
		data.b = value;

		m_data.insert(std::make_pair(name, data));
	}
	else {
		if (iter->second.type != eBlackboardDataType::BOOL)
			return false;

		// replacing the value
		iter->second.b = value;
	}

	return true;
}

bool Blackboard::set(const std::string& name, float value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end()) {
		// adding new data
		BlackboardData data;
		data.type = eBlackboardDataType::FLOAT;
		data.f = value;

		m_data.insert(std::make_pair(name, data));
	}
	else {
		if (iter->second.type != eBlackboardDataType::FLOAT)
			return false;

		// replacing the value
		iter->second.f = value;
	}

	return true;
}

bool Blackboard::get(const std::string& name, int& value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end() ||
		iter->second.type != eBlackboardDataType::INT)
		return false;

	value = iter->second.i;
	return true;
}

bool Blackboard::get(const std::string& name, unsigned int& value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end() ||
		iter->second.type != eBlackboardDataType::UINT)
		return false;

	value = iter->second.ui;
	return true;
}

bool Blackboard::get(const std::string& name, bool& value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end() ||
		iter->second.type != eBlackboardDataType::BOOL)
		return false;

	value = iter->second.b;
	return true;
}

bool Blackboard::get(const std::string& name, float& value) {

	auto iter = m_data.find(name);

	if (iter == m_data.end() ||
		iter->second.type != eBlackboardDataType::FLOAT)
		return false;

	value = iter->second.f;
	return true;
}

bool BlackboardQuestion::arbitrate(Blackboard* blackboard) {
	
	BlackboardExpert* bestExpert = nullptr;
	float bestResponse = 0;

	// find best expert answer
	for (auto expert : m_experts) {
		float response = expert->evaluateResponse(this, blackboard);

		if (response > bestResponse) {
			bestResponse = response;
			bestExpert = expert;
		}
	}

	if (bestExpert != nullptr) {
		bestExpert->execute(this, blackboard);
	}

	return bestExpert != nullptr;
}

void Blackboard::runArbitration() {

	std::list<BlackboardQuestion*> remove;

	for (auto question : m_questions) {
		if (question->arbitrate(this))
			remove.push_back(question);
	}

	for (auto question : remove)
		m_questions.remove(question);
}