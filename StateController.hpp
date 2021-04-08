#pragma once
#include <string>
#include <list>
#include <optional>
#include <map>
using namespace std;
#include "StateMachine.h"

namespace ssm {
	template <class S, class E> class StateController
	{
		map<S, StateMachine<S, E> *> states;
		list<E>_events;
		optional<S> actual_state;
	protected:
		friend class StateMachine<S, E>;
	public:
		StateController() {
			actual_state = nullopt;
		}

		~StateController() {
			while (states.size())
			{
				S s = states.begin()->first;
				states.erase(s);
			}
			states.clear();
			_events.clear();
		}

		void process() {
			if (!actual_state.has_value())
				return;
			if (_events.size() > 0)
			{
				string ev = _events.front();
				_events.pop_front();
				StateMachine<S, E> *next = states[actual_state.value()]->process_event(ev, this);
				if (next->state_id() != actual_state)
				{
					actual_state = next->state_id();// optional<StateMachine>{ *next };
					states[actual_state.value()]->actions_in(*this);
				}
			}
		}
		StateMachine<S, E> * createStateMachine(S s) {
			StateMachine<S, E> *m = new StateMachine<S, E>(s);
			states.insert(pair<S, StateMachine<S, E> *>(s, m));
			if (!actual_state) actual_state = m->state_id();
			return m;
		}

		optional<S> actual(){
			return actual_state;
		}
		void addevent(E ev) {
			_events.insert(_events.end(), ev);
		}

		int events() {
			return _events.size();
		}
	};
};

