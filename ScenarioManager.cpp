#include "ScenarioManager.h"
#include "Scenario.h"


Scenario* ScenarioManager::m_currentScenario = nullptr;
std::map<std::string, Scenario*> ScenarioManager::m_scenarios;

void ScenarioManager::init() {
	m_scenarios["Normal"] = new NormalScenario;
	m_scenarios["Pick"] = new PickScenario;
}

void ScenarioManager::terminate() {
	for (auto spair : m_scenarios) {
		delete spair.second;
	}
}
