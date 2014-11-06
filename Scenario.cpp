#include "Scenario.h"
#include "Action.h"

Scenario::Scenario() {

}

Scenario::~Scenario() {
	for (auto p : m_actions) {
		delete p;
	}
	m_actions.clear();

}

NormalScenario::NormalScenario() {
	buildActions();
}

void NormalScenario::buildActions() {
	m_actions.push_back(new PickingAction("Picking"));
}


void NormalScenario::render() {

}

PickScenario::PickScenario() {

}

void PickScenario::render() {

}

