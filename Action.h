#ifndef Action_h__
#define Action_h__



enum ActionStatus {
	IDLING,
	TOGGLED_OFF,
	TOGGLED_ON,
	PRESSED,
};



struct Action {
	Action(const std::string name, ActionStatus as= IDLING)
		: actName(name), actStatus(as), isSelected(false)
	{}

	void transferStatus() {
		if (actStatus == TOGGLED_OFF) {
			actStatus = TOGGLED_ON;
		} else if (actStatus == TOGGLED_ON) {
			actStatus = TOGGLED_OFF;
		} else if (actStatus == IDLING) {
			actStatus = PRESSED;
		}
	}

	std::string buildActionName() {
		if (actStatus == TOGGLED_OFF) {
			return actName + ":      OFF";
		} else if (actStatus == TOGGLED_ON) {
			return actName + ":      ON";
		}
		return actName;
	}

	void (*action_cb)(void *);
	ActionStatus actStatus;
	std::string actName;
	bool isSelected;
};

#endif // Action_h__
