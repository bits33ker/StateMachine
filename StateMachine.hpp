#pragma once
#include <any>
#include <string>
#include <map>
#include <list>
#include <boost/signals2.hpp>
using namespace std;
namespace ssm {
	template <class S, class E> class StateController;
	typedef void(*event_task)(void *);

	template <class S, class E> class StateMachine
	{
		S state;
		map<E, event_task> events_handlers;
		map<E, S> events_transitions;
		boost::signals2::signal<void()> actions;//init actions
		//friend class StateController;

	public:
		StateMachine() {
		}
		StateMachine(S s)
		{
			state = s;
		}
		StateMachine *process_event(E ev, StateController<S, E> * controller) {
			typename map<E, event_task>::iterator hit = events_handlers.find(ev);
			if (hit == events_handlers.end())
				return this;
			event_task sig = hit->second;
			if (sig != NULL)
				sig(controller);
			typename map<E, S>::iterator it = events_transitions.find(ev);
			if (it == events_transitions.end())
				return this;
			for (pair<S, StateMachine<S, E> *> s : controller->states)
			{
				if (s.first == it->second)
					return s.second;
			}
			return this;

		}

		void actions_in(StateController<S, E> & controller) {
			actions();
		}
		S state_id() { return state; };
		
		StateMachine  &operator=(StateMachine & s) {
			this->state = s.state;
			this->events_handlers.clear();
			this->events_transitions.clear();
			for (pair<string, event_task> p : s.events_handlers)
				this->events_handlers.insert(this->events_handlers.end(), p);
			for (pair<string, int> p : s.events_transitions)
				this->events_transitions.insert(p);

			return *this;
		}


		void addevent(E ev, StateMachine * m, event_task task) {
			events_handlers.insert(pair<E, event_task>(ev, task));
			events_transitions.insert(pair<E, S>(ev, m->state));
		}
	};
};

