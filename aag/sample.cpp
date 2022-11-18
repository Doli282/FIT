#ifndef __PROGTEST__

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <variant>
#include <vector>

using State = unsigned int;
using Symbol = uint8_t;

struct NFA
{
    std::set<State> m_States;
    std::set<Symbol> m_Alphabet;
    std::map<std::pair<State, Symbol>, std::set<State>> m_Transitions;
    State m_InitialState;
    std::set<State> m_FinalStates;
};

struct DFA
{
    std::set<State> m_States;
    std::set<Symbol> m_Alphabet;
    std::map<std::pair<State, Symbol>, State> m_Transitions;
    State m_InitialState;
    std::set<State> m_FinalStates;
};

#endif
// minimize
// -> remove unreachable states
// -> remove superflous states
// -> equivalence
// determinize

DFA determinize(const NFA &NFAutomaton)
{
    DFA DFAutomaton;
    DFAutomaton.m_Alphabet = NFAutomaton.m_Alphabet;         // copy alphabet
    DFAutomaton.m_InitialState = NFAutomaton.m_InitialState; // copy initial state

    // BFS like
    std::map<std::set<State>, State> setToState; // map newly created states to numbers
    std::queue<std::set<State>> queue;           // queue with new states
    State NOStates = 0;
    State currentState;

    // insert initial state
    std::set<State> currentSetState{DFAutomaton.m_InitialState};
    queue.push(std::move(currentSetState));

    while (!queue.empty())
    {
        currentSetState = queue.front();
        queue.pop();
        currentState = setToState.at(currentSetState);

        for (Symbol symbol : NFAutomaton.m_Alphabet) // for every symbol in the alphabet and for every state in the set find new set of states that you cen get to
        {
            std::set<State> nextStates;
            for (State state : currentSetState)
            {
                try
                {
                    nextStates.insert(NFAutomaton.m_Transitions.at(state, symbol));
                }
                catch (const std::exception &e) // to catch exception for being out of bounds when the transition does not exist
                {
                    ;
                }
            }
            if (!nextStates.empty()) // if there is any transition try to create a new state and save this transition
            {
                if (setToState.emplace(nextStates, NOStates).second) // try to insert as new state
                {
                    NOStates++;
                    queue.push(nextStates);
                }
                DFAutomaton.m_Transitions.emplace(std::make_pair(setToState.at(nextStates), symbol), currentState);
            }
        }
    }

    return DFAutomaton;
}

void removeUnreachable(DFA &automaton)
{
    std::set<State> states;
    std::map<std::pair<State, Symbol>, State> transitions;

    // BFS
    std::queue<State> queue;
    queue.push(automaton.m_InitialState);
    State inspectedState;

    while (!queue.empty())
    {
        inspectedState = queue.front();
        queue.pop();
        // find transitions from this state to others
        for (auto transition : automaton.m_Transitions)
        {
            if (transition.first.first == inspectedState)
            { // save transitions from the inspected state and push the next state to the queue, if not visisted yet
                if (states.emplace(transition.second).second)
                {
                    queue.push(transition.second);
                }
                transitions.emplace(std::move(transition));
            }
        }
    }

    // update final states -> remove unreachable states
    for (auto it = automaton.m_FinalStates.begin(); it != automaton.m_FinalStates.end();)
    {
        if (states.find(*it) != states.end()) // if a final state was not found in the new set of states, erase it
        {
            it = automaton.m_FinalStates.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // update new set of states and transitions
    automaton.m_States = states;
    automaton.m_Transitions = transitions;
    return;
}

void removeSuperfluous(DFA &automaton)
{
    std::set<State> states;
    std::map<std::pair<State, Symbol>, State> transitions;

    // BFS
    std::queue<State> queue;
    for (auto finalState : automaton.m_FinalStates)
    { // fill the queue with final states
        queue.push(finalState);
    }

    State inspectedState;

    while (!queue.empty())
    {
        inspectedState = queue.front();
        queue.pop();
        // find transitions from this state to others
        for (auto transition : automaton.m_Transitions)
        {
            if (transition.second == inspectedState)
            { // save transitions to the inspected state and push the previous state to the queue, if not visisted yet
                if (states.emplace(transition.first.first).second)
                {
                    queue.push(transition.first.first);
                }
                transitions.emplace(std::move(transition));
            }
        }
    }
    states.emplace(automaton.m_InitialState);

    // update new set of states and transitions
    automaton.m_States = states;
    automaton.m_Transitions = transitions;
    return;
}

void equivalence(DFA &automaton)
{
    std::map<State, unsigned int> stateToClass;                         // map states to their equivalence classes
    std::map<unsigned int, std::set<State>> classToStates;              // map classes to set of states in the class
    std::map<State, std::map<Symbol, unsigned int>> tableOfTransitions; // table of states and their transitions to equivalence classes
    State NOclasses = 2;                                                // number of classes after the iteration
    State NOclassesPrev = 0;                                            // number of classes before the iteration

    automaton.m_States.clear();    // erase the original states
    automaton.m_States.emplace(0); // fill two initial classes
    automaton.m_States.emplace(1);

    // first iteration -> divide to terminal and non-terminal states
    for (State state : automaton.m_States)
    {
        if (automaton.m_FinalStates.find(state) != automaton.m_FinalStates.end())
        {
            stateToClass.emplace(state, 1);
            classToStates[1].emplace(state);
        }
        else
        {
            stateToClass.emplace(state, 0);
            classToStates[0].emplace(state);
        }
    }

    // create and check transition tables
    while (NOclassesPrev != NOclasses)
    {
        NOclassesPrev = NOclasses;
        tableOfTransitions.clear();                     // wipe the previous table clean
        for (auto transition : automaton.m_Transitions) // for every transition fill the table
        {
            tableOfTransitions[transition.first.first].emplace(transition.first.second, stateToClass.at(transition.second));
        }

        for (unsigned int i = 0; i < NOclasses; i++) // for every equivalence class ...
        {
            std::vector<unsigned int> derivedClasses; // vector with classes that are derived from the current class
            for (State state1 : classToStates.at(i))  // ... for every state in the class
            {
                // .. check its transitions with every other state
                for (State state2 : classToStates.at(i))
                {
                    if (tableOfTransitions.at(state1) != tableOfTransitions.at(state2)) // if the transitions are different, move the state2 to different class
                    {
                        classToStates.at(i).erase(state2);
                        for (unsigned int j : derivedClasses) // try to find out, if it does not fit in already created !derived! class
                        {
                            if (tableOfTransitions.at(state2) == tableOfTransitions.at(*(classToStates.at(j).begin()))) // if it belongs to already created class, put it there
                            {
                                stateToClass.at(state2) = j;         // assign new class
                                classToStates.at(j).emplace(state2); // move the state to different class
                                goto jump;
                            }
                        }
                        derivedClasses.push_back(NOclasses);         // create new class
                        stateToClass.at(state2) = NOclasses;         // assign new class
                        classToStates.at(NOclasses).emplace(state2); // move the state to different class
                        automaton.m_States.emplace(NOclasses);       // save this class to the automaton
                        NOclasses++;
                    jump:
                        break;
                    }
                }
            }
        }
    }
    // table is finished

    // recombine transition table
    automaton.m_Transitions.clear();
    for (auto it = tableOfTransitions.begin(); it != tableOfTransitions.end(); ++it)
    {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            automaton.m_Transitions.emplace(std::make_pair(stateToClass.at(it->first), it2->first), it2->second);
        }
    }

    automaton.m_InitialState = stateToClass.at(automaton.m_InitialState); // change the initial state
    // upadte final states
    std::set<State> finalStates;
    for (State state : automaton.m_FinalStates) // save all classes that contain final state
    {
        finalStates.emplace(stateToClass.at(state));
    }
    automaton.m_FinalStates = finalStates;
    return;
}

void minimize(DFA &automaton)
{
    removeUnreachable(automaton);
    removeSuperfluous(automaton);
    equivalence(automaton);
}

DFA unify(const NFA &a, const NFA &b);
DFA intersect(const NFA &a, const NFA &b);

#ifndef __PROGTEST__

// You may need to update this function or the sample data if your state naming strategy differs.
bool operator==(const DFA &a, const DFA &b)
{
    return std::tie(a.m_States, a.m_Alphabet, a.m_Transitions, a.m_InitialState, a.m_FinalStates) == std::tie(b.m_States, b.m_Alphabet, b.m_Transitions, b.m_InitialState, b.m_FinalStates);
}

int main()
{
    NFA a1{
        {0, 1, 2},
        {'a', 'b'},
        {
            {{0, 'a'}, {0, 1}},
            {{0, 'b'}, {0}},
            {{1, 'a'}, {2}},
        },
        0,
        {2},
    };
    NFA a2{
        {0, 1, 2},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{1, 'a'}, {2}},
            {{2, 'a'}, {2}},
            {{2, 'b'}, {2}},
        },
        0,
        {2},
    };
    DFA a{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{1, 'a'}, {2}},
            {{2, 'a'}, {2}},
            {{2, 'b'}, {3}},
            {{3, 'a'}, {4}},
            {{3, 'b'}, {3}},
            {{4, 'a'}, {2}},
            {{4, 'b'}, {3}},
        },
        0,
        {2},
    };
    assert(intersect(a1, a2) == a);

    NFA b1{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{0, 'b'}, {2}},
            {{2, 'a'}, {2, 3}},
            {{2, 'b'}, {2}},
            {{3, 'a'}, {4}},
        },
        0,
        {1, 4},
    };
    NFA b2{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'b'}, {1}},
            {{1, 'a'}, {2}},
            {{2, 'b'}, {3}},
            {{3, 'a'}, {4}},
            {{4, 'a'}, {4}},
            {{4, 'b'}, {4}},
        },
        0,
        {4},
    };
    DFA b{
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{0, 'b'}, {2}},
            {{2, 'a'}, {3}},
            {{2, 'b'}, {4}},
            {{3, 'a'}, {5}},
            {{3, 'b'}, {6}},
            {{4, 'a'}, {7}},
            {{4, 'b'}, {4}},
            {{5, 'a'}, {5}},
            {{5, 'b'}, {4}},
            {{6, 'a'}, {8}},
            {{6, 'b'}, {4}},
            {{7, 'a'}, {5}},
            {{7, 'b'}, {4}},
            {{8, 'a'}, {8}},
            {{8, 'b'}, {8}},
        },
        0,
        {1, 5, 8},
    };
    assert(unify(b1, b2) == b);

    NFA c1{
        {0, 1, 2, 3, 4},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{0, 'b'}, {2}},
            {{2, 'a'}, {2, 3}},
            {{2, 'b'}, {2}},
            {{3, 'a'}, {4}},
        },
        0,
        {1, 4},
    };
    NFA c2{
        {0, 1, 2},
        {'a', 'b'},
        {
            {{0, 'a'}, {0}},
            {{0, 'b'}, {0, 1}},
            {{1, 'b'}, {2}},
        },
        0,
        {2},
    };
    DFA c{
        {0},
        {'a', 'b'},
        {},
        0,
        {},
    };
    assert(intersect(c1, c2) == c);

    NFA d1{
        {0, 1, 2, 3},
        {'i', 'k', 'q'},
        {
            {{0, 'i'}, {2}},
            {{0, 'k'}, {1, 2, 3}},
            {{0, 'q'}, {0, 3}},
            {{1, 'i'}, {1}},
            {{1, 'k'}, {0}},
            {{1, 'q'}, {1, 2, 3}},
            {{2, 'i'}, {0, 2}},
            {{3, 'i'}, {3}},
            {{3, 'k'}, {1, 2}},
        },
        0,
        {2, 3},
    };
    NFA d2{
        {0, 1, 2, 3},
        {'i', 'k'},
        {
            {{0, 'i'}, {3}},
            {{0, 'k'}, {1, 2, 3}},
            {{1, 'k'}, {2}},
            {{2, 'i'}, {0, 1, 3}},
            {{2, 'k'}, {0, 1}},
        },
        0,
        {2, 3},
    };
    DFA d{
        {0, 1, 2, 3},
        {'i', 'k', 'q'},
        {
            {{0, 'i'}, {1}},
            {{0, 'k'}, {2}},
            {{2, 'i'}, {3}},
            {{2, 'k'}, {2}},
            {{3, 'i'}, {1}},
            {{3, 'k'}, {2}},
        },
        0,
        {1, 2, 3},
    };
    assert(intersect(d1, d2) == d);
}
#endif
