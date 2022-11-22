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

void printNFA(const NFA &automaton)
{
    std::cout << "NFA";
    for (Symbol symbol : automaton.m_Alphabet)
    {
        std::cout << " " << symbol;
    }
    std::cout << std::endl;
    for (State state : automaton.m_States)
    {
        if (state == automaton.m_InitialState)
            std::cout << ">";
        if (automaton.m_FinalStates.find(state) != automaton.m_FinalStates.end())
            std::cout << "<";
        std::cout << state;
        for (Symbol symbol : automaton.m_Alphabet)
        {
            try
            {
                std::set<State> nextStates = automaton.m_Transitions.at(std::make_pair(state, symbol));
                std::cout << " ";
                bool first = true;
                for (State nextState : nextStates)
                {
                    if (!first)
                        std::cout << "|";
                    first = false;
                    std::cout << nextState;
                }
            }
            catch (...)
            {
                std::cout << " -";
            }
        }
        std::cout << std::endl;
    }
}

void printDFA(const DFA &automaton)
{
    std::cout << "DFA";
    for (Symbol symbol : automaton.m_Alphabet)
    {
        std::cout << " " << symbol;
    }
    std::cout << std::endl;
    for (State state : automaton.m_States)
    {
        if (state == automaton.m_InitialState)
            std::cout << ">";
        if (automaton.m_FinalStates.find(state) != automaton.m_FinalStates.end())
            std::cout << "<";
        std::cout << state;
        for (Symbol symbol : automaton.m_Alphabet)
        {
            try
            {
                State next = automaton.m_Transitions.at(std::make_pair(state, symbol));
                std::cout << " " << next;
            }
            catch (...)
            {
                std::cout << " -";
            }
        }
        std::cout << std::endl;
    }
}

// determinize + make total
DFA determinize(const NFA &original, std::set<Symbol> secondAlphabet)
{
    DFA result;
    result.m_Alphabet = original.m_Alphabet; // copy alphabet
    result.m_Alphabet.merge(secondAlphabet);

    // BFS like
    std::queue<std::set<State>> queue;
    std::map<std::set<State>, State> setToState;
    std::set<State> currentSetState{original.m_InitialState};
    State NOStates = 0;

    // solve the initial state
    result.m_InitialState = NOStates;                                                         // add as initial stte
    if (original.m_FinalStates.find(original.m_InitialState) != original.m_FinalStates.end()) // check for final state
    {
        result.m_FinalStates.emplace(NOStates);
    }
    result.m_States.emplace(NOStates); // add to states

    // add failState to make the automaton total
    State failState = -1;
    result.m_States.emplace(failState);
    for (Symbol symbol : result.m_Alphabet)
    {
        result.m_Transitions.emplace(std::make_pair(failState, symbol), failState); // add transition to the failState
    }

    queue.emplace(currentSetState);
    setToState.emplace(currentSetState, NOStates++);
    while (!queue.empty())
    {
        currentSetState = queue.front();
        queue.pop();
        // insert every possible next State I can get to from the current set of states to every symbol
        for (Symbol symbol : result.m_Alphabet)
        {
            std::set<State> nextSetState; // set of next states
            bool isFinal = false;
            for (State state : currentSetState)
            {
                try
                { // insert next states of the one state from the set to overall set of next states
                    std::set<State> nextStates = original.m_Transitions.at(std::make_pair(state, symbol));
                    for (State nextState : nextStates)
                    {
                        nextSetState.emplace(nextState);
                        // find out if the next state could be final state
                        if (isFinal || original.m_FinalStates.find(nextState) != original.m_FinalStates.end())
                        {
                            isFinal = true;
                        }
                    }
                }
                catch (...)
                { // the transition for this state does not exist
                    ;
                }
            }
            if (nextSetState.empty())
            {                                                                                                    // there are no transitions
                result.m_Transitions.emplace(std::make_pair(setToState.at(currentSetState), symbol), failState); // add transition to the failState
            }
            else
            {                                                          // there is some next state
                if (setToState.emplace(nextSetState, NOStates).second) // try to insert the new state in the map
                {                                                      // new state was inserted -> it is a NEW state
                    queue.emplace(nextSetState);
                    result.m_States.emplace(NOStates); // add new state
                    NOStates++;
                }
                result.m_Transitions.emplace(std::make_pair(setToState.at(currentSetState), symbol), setToState.at(nextSetState)); // add new transition
                if (isFinal)
                { // add final state if one of the next states in the set was final
                    result.m_FinalStates.emplace(setToState.at(nextSetState));
                }
            }
        }
    }

    return result;
}

void removeSuperfluous(DFA &automaton)
{
    std::map<std::pair<State, Symbol>, State> newTransitions;

    std::set<State> visited;
    std::queue<State> queue;

    // start at the final states
    for (State finalState : automaton.m_FinalStates)
    {
        visited.emplace(finalState);
        queue.emplace(finalState);
    }

    State currentState;
    while (!queue.empty())
    {
        currentState = queue.front();
        queue.pop();

        // find transitions from this state to others
        for (auto transition : automaton.m_Transitions)
        {
            if (transition.second == currentState)
            { // save transitions to the inspected state and push the previous state to the queue, if not visisted yet
                if (visited.emplace(transition.first.first).second)
                {
                    queue.push(transition.first.first);
                }
                newTransitions.emplace(std::move(transition));
            }
        }
    }

    visited.emplace(automaton.m_InitialState); // add the initial state to visited, even if it is not useful

    // update new set of states and transitions
    automaton.m_States = visited;
    automaton.m_Transitions = newTransitions;
    return;
}

void removeUnreachable(DFA &automaton)
{
    std::map<std::pair<State, Symbol>, State> newTransitions;

    std::set<State> visited;
    std::queue<State> queue;

    State currentState = automaton.m_InitialState;
    queue.emplace(currentState);
    visited.emplace(currentState);

    while (!queue.empty())
    {
        currentState = queue.front();
        queue.pop();
        // find every next state, if not visited, push into queue
        for (auto it = automaton.m_Transitions.lower_bound(std::make_pair(currentState, 0)); it != automaton.m_Transitions.lower_bound(std::make_pair(currentState + 1, 0)); ++it)
        {
            if (visited.emplace(it->second).second)
            {
                queue.emplace(it->second);
            }
            newTransitions.emplace(*it); // !!!! PRi ZRYCHLOVANI ZKUSIT STD::MOVE !!!!!
        }
    }

    // erase invalid final states
    for (auto it = automaton.m_FinalStates.begin(); it != automaton.m_FinalStates.end();)
    {
        if (visited.find(*it) != visited.end())
        {
            ++it;
        }
        else
        {
            it = automaton.m_FinalStates.erase(it);
        }
    }
    automaton.m_States = visited;             // update states
    automaton.m_Transitions = newTransitions; // update transitions
    return;
}

void equivalence(DFA &automaton)
{
    //    std::cout << "equivalizing..." << std::endl;
    //    printDFA(automaton);
    std::map<State, unsigned int> stateToClass;                         // map states to their equivalence classes
    std::map<unsigned int, std::set<State>> classToStates;              // map classes to set of states in the class
    std::map<State, std::map<Symbol, unsigned int>> tableOfTransitions; // table of states and their transitions to equivalence classes
    State NOclasses = 1;                                                // number of classes after the iteration
    State NOclassesPrev = 0;                                            // number of classes before the iteration

    // first iteration -> divide to terminal and non-terminal states
    classToStates.emplace(0, std::set<State>());
    if (!automaton.m_FinalStates.empty()) // do not create second class if there are no final states
    {
        classToStates.emplace(NOclasses++, std::set<State>());
    }
    for (State state : automaton.m_States)
    {
        tableOfTransitions.emplace(state, std::map<Symbol, unsigned int>());
        if (automaton.m_FinalStates.find(state) != automaton.m_FinalStates.end())
        {
            stateToClass.emplace(state, 1);
            classToStates[1].emplace(state);
        }
        else
        {
            stateToClass.emplace(state, 0);
            classToStates[0].emplace(state);
            //      std::cout << "class 0: " << state << std::endl;
        }
    }

    automaton.m_States.clear();           // erase the original states
    automaton.m_States.emplace(0);        // fill two initial classes
    if (!automaton.m_FinalStates.empty()) // do not create second class if there are no final states
    {
        automaton.m_States.emplace(1);
    }

    // std::cout << "determinizing - entering while cycle" << std::endl;

    // create and check transition tables
    while (NOclassesPrev != NOclasses)
    {
        NOclassesPrev = NOclasses;
        // wipe the previous table clean
        for (auto it = tableOfTransitions.begin(); it != tableOfTransitions.end(); ++it)
        {
            it->second.clear();
        }
        for (auto transition : automaton.m_Transitions) // for every transition fill the table
        {
            tableOfTransitions[transition.first.first].emplace(transition.first.second, stateToClass.at(transition.second));
        }

        for (unsigned int i = 0; i < NOclassesPrev; i++) // for every equivalence class ...
        {
            std::vector<unsigned int> derivedClasses; // vector with classes that are derived from the current class
            for (State state1 : classToStates.at(i))  // ... for every state in the class
            {
                // .. check its transitions with every other state
                // for (State state2 : classToStates.at(i))
                for (auto itSet = classToStates.at(i).begin(); itSet != classToStates.at(i).end();)
                {
                    if (tableOfTransitions.at(state1) != tableOfTransitions.at(*itSet)) // if the transitions are different, move the state2 to different class
                    {
                        for (unsigned int j : derivedClasses) // try to find out, if it does not fit in already created !derived! class
                        {
                            if (tableOfTransitions.at(*itSet) == tableOfTransitions.at(*(classToStates.at(j).begin()))) // if it belongs to already created class, put it there
                            {
                                stateToClass.at(*itSet) = j;         // assign new class
                                classToStates.at(j).emplace(*itSet); // move the state to different class
                                goto jump;
                            }
                        }
                        derivedClasses.push_back(NOclasses); // create new class
                        stateToClass.at(*itSet) = NOclasses; // assign new class
                        classToStates.emplace(NOclasses, std::set<State>());
                        classToStates.at(NOclasses).emplace(*itSet); // move the state to different class
                        automaton.m_States.emplace(NOclasses);       // save this class to the automaton
                        NOclasses++;
                    jump:;
                        itSet = classToStates.at(i).erase(itSet);
                    }
                    else
                    {
                        ++itSet;
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
    //    std::cout << "equivalized" << std::endl;
    //    printDFA(automaton);
    //    std::cout << "---exiting equivalization" << std::endl;
    return;
}

void rename(DFA &automaton)
{
    std::map<std::pair<State, Symbol>, State> newTransitions;
    
    std::queue<State> queue;
    std::map<State, State> dict;
    State NOStates = 0;
    State currentState = automaton.m_InitialState;

    queue.emplace(currentState);
    dict.emplace(currentState, NOStates++);
    while(!queue.empty())
    {
        currentState = queue.front();
        queue.pop();

        for (auto it = automaton.m_Transitions.lower_bound(std::make_pair(currentState, 0)); it != automaton.m_Transitions.lower_bound(std::make_pair(currentState + 1, 0)); ++it)
        {
            if (dict.emplace(it->second, NOStates).second)
            {
                queue.emplace(it->second);
                NOStates++;
            }
            newTransitions.emplace(std::make_pair(dict.at(it->first.first), it->first.second),dict.at(it->second)); // !!!! PRi ZRYCHLOVANI ZKUSIT STD::MOVE !!!!!
        }
    }

    automaton.m_InitialState = dict.at(automaton.m_InitialState);
    automaton.m_Transitions = newTransitions;
    std::set<State> newFinals;
    for(State state : automaton.m_FinalStates)
    {
        newFinals.emplace(dict.at(state));
    }
    automaton.m_FinalStates = newFinals;
    automaton.m_States.clear();
    for(State i = 0; i < NOStates; i++)
    {
        automaton.m_States.emplace(i);
    }

    return;
}

void minimize(DFA &automaton)
{
    //    std::cout << "minimizing..." << std::endl;
    // printDFA(automaton);
    removeSuperfluous(automaton);
    removeUnreachable(automaton);
    equivalence(automaton);
    //printDFA(automaton);
    //std::cout << "rename" << std::endl;
    rename(automaton);
    // printDFA(automaton);
    //     std::cout << "---minimized" << std::endl;
}

DFA unify(const NFA &a, const NFA &b)
{
    // printNFA(a);
    // printNFA(b);

    DFA result;
    DFA A = determinize(a, b.m_Alphabet);
    DFA B = determinize(b, a.m_Alphabet);

    result.m_Alphabet = A.m_Alphabet;

    std::map<std::pair<State, State>, State> setToState;
    State NOStates = 0;
    std::queue<std::pair<State, State>> queue;
    std::pair<State, State> currentSetState{A.m_InitialState, B.m_InitialState};
    std::pair<State, State> nextSetState;

    // add the initial state
    result.m_InitialState = NOStates;
    if ((A.m_FinalStates.find(A.m_InitialState) != A.m_FinalStates.end()) || (B.m_FinalStates.find(A.m_InitialState) != B.m_FinalStates.end()))
    {
        result.m_FinalStates.emplace(NOStates);
    }
    result.m_States.emplace(NOStates);

    queue.emplace(currentSetState);
    setToState.emplace(currentSetState, NOStates++);

    while (!queue.empty())
    {
        currentSetState = queue.front();
        queue.pop();

        for (Symbol symbol : result.m_Alphabet)
        {
            nextSetState.first = A.m_Transitions.at(std::make_pair(currentSetState.first, symbol));
            nextSetState.second = B.m_Transitions.at(std::make_pair(currentSetState.second, symbol));
            if (setToState.emplace(nextSetState, NOStates).second)
            {
                queue.emplace(nextSetState);
                result.m_States.emplace(NOStates); // add state
                if ((A.m_FinalStates.find(nextSetState.first) != A.m_FinalStates.end()) || (B.m_FinalStates.find(nextSetState.second) != B.m_FinalStates.end()))
                { // add final state if one of them was final
                    result.m_FinalStates.emplace(NOStates);
                }
                NOStates++;
            }

            result.m_Transitions.emplace(std::make_pair(setToState.at(currentSetState), symbol), setToState.at(nextSetState)); // add transition
        }
    }

    minimize(result);
    // printDFA(result);
    return result;
}

DFA intersect(const NFA &a, const NFA &b)
{
    // printNFA(a);
    // printNFA(b);

    DFA result;
    DFA A = determinize(a, b.m_Alphabet);
    DFA B = determinize(b, a.m_Alphabet);
    // printDFA(A);
    // printDFA(B);

    result.m_Alphabet = A.m_Alphabet;

    std::map<std::pair<State, State>, State> setToState;
    State NOStates = 0;
    std::queue<std::pair<State, State>> queue;
    std::pair<State, State> currentSetState{A.m_InitialState, B.m_InitialState};
    std::pair<State, State> nextSetState;

    // add the initial state
    result.m_InitialState = NOStates;
    if ((A.m_FinalStates.find(A.m_InitialState) != A.m_FinalStates.end()) && (B.m_FinalStates.find(A.m_InitialState) != B.m_FinalStates.end()))
    {
        result.m_FinalStates.emplace(NOStates);
    }
    result.m_States.emplace(NOStates);

    queue.emplace(currentSetState);
    setToState.emplace(currentSetState, NOStates++);

    while (!queue.empty())
    {
        currentSetState = queue.front();
        queue.pop();

        for (Symbol symbol : result.m_Alphabet)
        {
            nextSetState.first = A.m_Transitions.at(std::make_pair(currentSetState.first, symbol));
            nextSetState.second = B.m_Transitions.at(std::make_pair(currentSetState.second, symbol));
            if (setToState.emplace(nextSetState, NOStates).second)
            {
                queue.emplace(nextSetState);
                result.m_States.emplace(NOStates); // add state
                if ((A.m_FinalStates.find(nextSetState.first) != A.m_FinalStates.end()) && (B.m_FinalStates.find(nextSetState.second) != B.m_FinalStates.end()))
                { // add final state if both of them were final
                    result.m_FinalStates.emplace(NOStates);
                }
                NOStates++;
            }

            result.m_Transitions.emplace(std::make_pair(setToState.at(currentSetState), symbol), setToState.at(nextSetState)); // add transition
        }
    }

    // printDFA(result);
    minimize(result);
    // printDFA(result);
    return result;
}

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

    NFA test1{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
        {'1', '2'},
        {
            {{0, '1'}, {3}},
            {{0, '1'}, {6}},
            {{0, '1'}, {17}},
            {{0, '2'}, {7}},
            {{0, '2'}, {19}},
            {{3, '1'}, {11}},
            {{3, '1'}, {19}},
            {{3, '2'}, {3}},
            {{3, '2'}, {18}},
            {{6, '1'}, {7}},
            {{6, '1'}, {18}},
            {{6, '2'}, {9}},
            {{6, '2'}, {13}},
            {{6, '2'}, {15}},
            {{7, '1'}, {7}},
            {{7, '1'}, {16}},
            {{7, '2'}, {4}},
            {{7, '2'}, {8}},
            {{8, '2'}, {2}},
            {{8, '2'}, {13}},
            {{8, '2'}, {14}},
            {{8, '2'}, {15}},
            {{10, '1'}, {1}},
            {{11, '1'}, {7}},
            {{11, '1'}, {8}},
            {{11, '1'}, {17}},
            {{11, '2'}, {2}},
            {{12, '1'}, {9}},
            {{12, '2'}, {15}},
            {{13, '1'}, {12}},
            {{13, '1'}, {16}},
            {{13, '2'}, {12}},
            {{13, '2'}, {14}},
            {{14, '1'}, {5}},
            {{15, '2'}, {16}},
            {{16, '1'}, {14}},
            {{17, '1'}, {10}},
            {{19, '1'}, {13}},
            {{19, '1'}, {18}},
            {{19, '2'}, {12}},
        },
        0,
        {1, 2, 4, 5, 9, 12, 15, 16, 18}};

    NFA test{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
        {'0', '1', '2'},
        {
            {{0, '0'}, {7}},
            {{0, '1'}, {1}},
            {{0, '1'}, {15}},
            {{0, '1'}, {17}},
            {{0, '2'}, {19}},
            {{1, '1'}, {1}},
            {{1, '1'}, {4}},
            {{1, '1'}, {17}},
            {{1, '2'}, {0}},
            {{1, '2'}, {6}},
            {{2, '0'}, {18}},
            {{2, '1'}, {9}},
            {{2, '2'}, {4}},
            {{3, '0'}, {11}},
            {{3, '0'}, {16}},
            {{3, '2'}, {8}},
            {{4, '0'}, {10}},
            {{4, '1'}, {14}},
            {{4, '2'}, {1}},
            {{5, '0'}, {4}},
            {{5, '1'}, {17}},
            {{5, '2'}, {4}},
            {{6, '0'}, {14}},
            {{6, '0'}, {19}},
            {{6, '1'}, {1}},
            {{6, '2'}, {0}},
            {{6, '2'}, {8}},
            {{7, '0'}, {13}},
            {{7, '1'}, {6}},
            {{8, '2'}, {11}},
            {{9, '0'}, {8}},
            {{9, '0'}, {18}},
            {{9, '1'}, {6}},
            {{9, '1'}, {9}},
            {{9, '2'}, {17}},
            {{10, '0'}, {5}},
            {{11, '0'}, {1}},
            {{12, '2'}, {0}},
            {{12, '2'}, {2}},
            {{12, '2'}, {5}},
            {{12, '2'}, {8}},
            {{12, '2'}, {14}},
            {{13, '0'}, {19}},
            {{13, '1'}, {0}},
            {{14, '2'}, {19}},
            {{15, '1'}, {3}},
            {{15, '1'}, {10}},
            {{15, '1'}, {19}},
            {{15, '2'}, {1}},
            {{15, '2'}, {2}},
            {{15, '2'}, {16}},
            {{16, '0'}, {19}},
            {{16, '1'}, {2}},
            {{16, '1'}, {9}},
            {{17, '2'}, {6}},
            {{18, '2'}, {11}},
            {{19, '0'}, {12}},
            {{19, '1'}, {6}},
            {{19, '1'}, {18}},
            {{19, '2'}, {3}},
        },
        0,
        {2, 3, 4, 5, 6, 10, 11, 13, 16, 18}};

    NFA test3{
        {0, 1, 2, 3, 4},
        {'0', '2'},
        {
            {{0, '0'}, {1, 2, 3}},
            {{0, '2'}, {3, 4}},
            {{1, '0'}, {0, 2}},
            {{1, '2'}, {1}},
            {{2, '2'}, {1, 3, 4}},
            {{3, '0'}, {0}},
            {{3, '2'}, {0, 1, 2, 3, 4}},
            {{4, '0'}, {3}},
            {{4, '2'}, {2, 4}},
        },
        0,
        {0, 1, 4}};
    NFA test4{
        {0, 1, 2, 3, 4, 5},
        {'0', '2'},
        {
            {{0, '0'}, {3, 5}},
            {{0, '2'}, {4}},
            {{1, '0'}, {2, 4}},
            {{1, '2'}, {2}},
            {{2, '0'}, {0, 4, 5}},
            {{2, '2'}, {0, 3, 4}},
            {{3, '0'}, {1, 3, 5}},
            {{3, '2'}, {0, 1}},
            {{4, '0'}, {1, 4, 5}},
            {{4, '2'}, {1, 2, 3}},
            {{5, '0'}, {0, 2, 3}},
            {{5, '2'}, {1, 2, 3}},
        },
        0,
        {2, 3}};
    
    NFA z1{
        {0},
        {'a'},
        {   },
        0,
        {}};
    NFA z2{
        {0, 1, 2, 3},
        {'a', 'b'},
        {
            {{0, 'a'}, {0, 1}},
            {{0, 'b'}, {1}},
            {{1, 'a'}, {2}},
            {{1, 'b'}, {2}},
            {{2, 'a'}, {3}},
            {{2, 'b'}, {3}},
        },
        0,
        {3}};
    DFA z{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        {'a', 'b'},
        {
            {{0, 'a'}, {1}},
            {{0, 'b'}, {2}},
            {{1, 'a'}, {3}},
            {{1, 'b'}, {4}},
            {{2, 'a'}, {5}},
            {{2, 'b'}, {5}},
            {{3, 'a'}, {6}},
            {{3, 'b'}, {7}},
            {{4, 'a'}, {8}},
            {{4, 'b'}, {8}},
            {{5, 'a'}, {9}},
            {{5, 'b'}, {9}},
            {{6, 'a'}, {6}},
            {{6, 'b'}, {7}},
            {{7, 'a'}, {8}},
            {{7, 'b'}, {8}},
            {{8, 'a'}, {9}},
            {{8, 'b'}, {9}},
        },
        0,
        {6, 7, 8, 9},
    };
    // printNFA(a1);
    // printNFA(a2);
    // printDFA(determinize(a1, a2.m_Alphabet));
    // printDFA(determinize(a2, a1.m_Alphabet));

    std::cout << "START" << std::endl;
    // printDFA(determinize(test3, a1.m_Alphabet));
    // minimize(determinize(test4, test4.m_Alphabet));
    //printDFA(a);
     assert(intersect(a1, a2) == a);
    //printDFA(b);
     assert(unify(b1, b2) == b);
    //printDFA(c);
    assert(intersect(c1, c2) == c);
    //printDFA(d);
    assert(intersect(d1, d2) == d);
    //printDFA(z);
    assert(unify(z1,z2) == z);
    std::cout << "FINISH" << std::endl;
}
#endif
