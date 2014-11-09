#ifndef ScenarioManager_h__
#define ScenarioManager_h__

#include <string>
#include <map>

class Scenario;
class ScenarioManager {
public:
	static Scenario* getCurrentScenario() { return m_currentScenario; }
	
	static void init();
	static void terminate();
private:
	static Scenario* m_currentScenario;
	static std::map<std::string, Scenario*> m_scenarios;
};
#endif // ScenarioManager_h__
