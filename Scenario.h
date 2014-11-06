#ifndef Scenario_h__
#define Scenario_h__

#include <vector>

class Action;

class Scenario {
public:
	Scenario();
	virtual ~Scenario();
	virtual void render() = 0;
	virtual const std::vector<Action*>& getActions() const {
		return m_actions;
	}
protected:
	std::vector<Action*> m_actions;
};


class NormalScenario : public Scenario {
public:
	NormalScenario();
	~NormalScenario();
	virtual void render();

private:
	void buildActions();
};


class PickScenario : public Scenario {
public:
	PickScenario();
	~PickScenario();
	virtual void render();
};


#endif // Scenario_h__
