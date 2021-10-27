#pragma once
#include <Windows.h>
#include <TraceLoggingActivity.h>

// Tracks the nesting of activities
template<TraceLoggingHProvider const& provider>
class ActivityStack {
	struct activity {
		activity *previous;
		TraceLoggingThreadActivityIdSetter* pManualParent;
		TraceLoggingThreadActivity<provider>* pThreadActivity;
	};

	activity *pTop = nullptr;

public:
	TraceLoggingThreadActivity<provider>* AddActivity() {
		activity *newTop = new activity();
		newTop->pThreadActivity = new TraceLoggingThreadActivity<provider>();
		newTop->pManualParent = nullptr;
		newTop->previous = pTop;
		pTop = newTop;
		return pTop->pThreadActivity;
	}

	~ActivityStack() {
		while (pTop != nullptr) Pop();
	}

	TraceLoggingThreadActivity<provider>* AddActivityWithParent(GUID& activityGuid)
	{
		auto parent = new TraceLoggingThreadActivityIdSetter(activityGuid);
		AddActivity();
		pTop->pManualParent = parent;
		return pTop->pThreadActivity;
	}

	TraceLoggingThreadActivity<provider>* Top() {
		if (pTop == nullptr) return nullptr;
		return pTop->pThreadActivity;
	}

	void Pop() {
		if (pTop == nullptr) return;

		activity *pToRemove = pTop;
		pTop = pTop->previous;

		delete pToRemove->pThreadActivity;
		delete pToRemove->pManualParent; // Benign if null;
		delete pToRemove;
	}
};
