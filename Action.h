#ifndef Action_h__
#define Action_h__

#include <string>




class Action {
public:
	Action(const std::string& name)
		: actName(name)
	{}
	virtual const std::string& getName() const { return actName; }
	virtual	void act() = 0;	
protected:
	std::string actName;
};


class PickingAction : public Action {
public:
	PickingAction(const std::string& name) : Action(name) {

	}
	~PickingAction() {

	}

	void act() {}
};

#endif // Action_h__
