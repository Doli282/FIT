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

DFA determinize(const NFA &NFAutomaton)
{
    //    std::cout << "determinizing..." << std::endl;
    //    printNFA(NFAutomaton);
    DFA DFAutomaton;
    DFAutomaton.m_Alphabet = NFAutomaton.m_Alphabet;         // copy alphabet
    DFAutomaton.m_InitialState = NFAutomaton.m_InitialState; // copy initial state

    // BFS like
    std::map<std::set<State>, State> setToState; // map newly created states to numbers
    std::queue<std::set<State>> queue;           // queue with new states
    State NOStates = 0;

    // insert initial state
    std::set<State> currentSetState{DFAutomaton.m_InitialState};
    DFAutomaton.m_States.emplace(NOStates);
    if (NFAutomaton.m_FinalStates.find(NFAutomaton.m_InitialState) != NFAutomaton.m_FinalStates.end()) // add initial as final if it was final
    {
        DFAutomaton.m_FinalStates.emplace(NOStates);
    }
    setToState.emplace(currentSetState, NOStates++);
    queue.push(std::move(currentSetState));

    while (!queue.empty())
    {
        currentSetState = queue.front();
        queue.pop();

        for (Symbol symbol : NFAutomaton.m_Alphabet) // for every symbol in the alphabet and for every state in the set find new set of states that you cen get to
        {
            std::set<State> nextStates;
            bool final = false;
            for (State state : currentSetState)
            {
                try
                {
                    std::set<State> tmp = NFAutomaton.m_Transitions.at(std::make_pair(state, symbol));
                    for (State nextState : tmp)
                    {
                        nextStates.emplace(nextState);
                        if (final || NFAutomaton.m_FinalStates.find(nextState) != NFAutomaton.m_FinalStates.end())
                        {
                            final = true;
                        }
                    }
                }
                catch (...) // to catch exception for being out of bounds when the transition does not exist
                {
                    ;
                }
            }
            if (!nextStates.empty()) // if there is any transition try to create a new state and save this transition
            {
                if (setToState.emplace(nextStates, NOStates).second) // try to insert as new state
                {
                    if (final)
                    {
                        DFAutomaton.m_FinalStates.emplace(NOStates);
                    }
                    DFAutomaton.m_States.emplace(NOStates++);
                    queue.push(nextStates);
                }
                DFAutomaton.m_Transitions.emplace(std::make_pair(setToState.at(currentSetState), symbol), setToState.at(nextStates));
            }
        }
    }
    //    std::cout << "determinized" << std::endl;
    //    printDFA(DFAutomaton);
    //    std::cout << "---exiting determinization" << std::endl;
    return DFAutomaton;
}

void removeUnreachable(DFA &automaton)
{
    //    std::cout << "removing unreachable..." << std::endl;
    //    printDFA(automaton);
    std::set<State> states;
    states.emplace(automaton.m_InitialState);
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
        if (states.find(*it) == states.end()) // if a final state was not found in the new set of states, erase it
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
    //    std::cout << "unreachable removed" << std::endl;
    //    printDFA(automaton);
    //    std::cout << "---exiting removeUnreachable" << std::endl;
    return;
}

void removeSuperfluous(DFA &automaton)
{
    //    std::cout << "removing superfluous..." << std::endl;
    //    printDFA(automaton);
    std::set<State> states;
    std::map<std::pair<State, Symbol>, State> transitions;

    // BFS
    std::queue<State> queue;
    for (auto finalState : automaton.m_FinalStates)
    { // fill the queue with final states
        queue.push(finalState);
        states.emplace(finalState);
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
    //    std::cout << "superfluous removed" << std::endl;
    //    printDFA(automaton);
    //    std::cout << "---exiting removeSuperfluous" << std::endl;
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
            //            std::cout << "i=" << i << std::endl;
            std::vector<unsigned int> derivedClasses; // vector with classes that are derived from the current class
            for (State state1 : classToStates.at(i))  // ... for every state in the class
            {
                // .. check its transitions with every other state
                // for (State state2 : classToStates.at(i))
                for (auto itSet = classToStates.at(i).begin(); itSet != classToStates.at(i).end();)
                {
                    //                    std::cout << "in for loops" << std::endl;
                    //                    std::cout << "s1 = " << state1 << ", s2 = " << state2 << std::endl;
                    if (tableOfTransitions.at(state1) != tableOfTransitions.at(*itSet)) // if the transitions are different, move the state2 to different class
                    {
                        //                        std::cout << "here1" << std::endl;
                        //                        std::cout << "here2" << std::endl;
                        for (unsigned int j : derivedClasses) // try to find out, if it does not fit in already created !derived! class
                        {
                            if (tableOfTransitions.at(*itSet) == tableOfTransitions.at(*(classToStates.at(j).begin()))) // if it belongs to already created class, put it there
                            {
                                //                        std::cout << "here3" << std::endl;

                                stateToClass.at(*itSet) = j;         // assign new class
                                classToStates.at(j).emplace(*itSet); // move the state to different class
                                                                     //                        std::cout << "here5" << std::endl;

                                goto jump;
                            }
                        }
                        //                       std::cout << "assigning" << std::endl;
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

void minimize(DFA &automaton)
{
    //    std::cout << "minimizing..." << std::endl;
    removeUnreachable(automaton);
    removeSuperfluous(automaton);
    equivalence(automaton);
    //    std::cout << "---minimized" << std::endl;
}

void unifyOne(const NFA &originalNFA, NFA &resultNFA, bool inverted)
{
    std::set<Symbol> alphabet = originalNFA.m_Alphabet;
    resultNFA.m_Alphabet.merge(alphabet); // union of alphabets

    State NOStatesInB = (inverted ? -1 : 1);
    std::map<State, State> convert; // map states from original new numbers

    // add original automaton to the new automaton
    for (State state : originalNFA.m_States) // add set of all states
    {
        NOStatesInB += (inverted ? -1 : 1);
        convert.emplace(state, NOStatesInB);
        resultNFA.m_States.emplace(NOStatesInB);
    }

    State previousState;
    for (auto it = originalNFA.m_Transitions.begin(); it != originalNFA.m_Transitions.end(); ++it) // add transitions
    {
        previousState = it->first.first;
        if (previousState == originalNFA.m_InitialState) // if the transition is from the original initial state add it to the new initial state
        {
            previousState = resultNFA.m_InitialState;
        }
        else
        {
            previousState = convert.at(previousState);
        }
        std::set<State> nextStates;
        for (State state2 : it->second)
        {
            nextStates.emplace(convert.at(state2));
        }
        if (!resultNFA.m_Transitions.emplace(std::make_pair(previousState, it->first.second), nextStates).second)
        {
            resultNFA.m_Transitions.at(std::make_pair(previousState, it->first.second)).merge(nextStates);
        }
    }

    for (State state : originalNFA.m_FinalStates) // add final states
    {
        resultNFA.m_FinalStates.emplace(convert.at(state));
        if (state == originalNFA.m_InitialState) // if the original intial state is a final one as well, make the new initial state a final one, too
        {
            resultNFA.m_FinalStates.emplace(resultNFA.m_InitialState);
        }
    }
}

DFA unify(const NFA &a, const NFA &b) // unify using epsilon transition in the process
{
    //    std::cout << "unifying..." << std::endl;
    //    printNFA(a);
    //    printNFA(b);
    NFA resultNFA; // create new automaton

    resultNFA.m_InitialState = 0; // add new initial state
    resultNFA.m_States.emplace(0);

    unifyOne(a, resultNFA, false); // add the first automaton
    unifyOne(b, resultNFA, true);  // add the second automaton

    //    std::cout << "---unified" << std::endl;
    //    printNFA(resultNFA);
    DFA resultDFA = determinize(resultNFA);
    minimize(resultDFA);
    return resultDFA;
}

NFA rename(const NFA &original, bool invert, State &TotalStates)
{
    NFA newNFA;
    newNFA.m_Alphabet = original.m_Alphabet;
    State NOStates = 0;

    std::map<State, State> names; // dictionary

    for (State state : original.m_States) // rename states
    {
        NOStates += invert ? -1 : 1;
        names.emplace(state, NOStates);
        newNFA.m_States.emplace(NOStates);
    }
    for (auto transition : original.m_Transitions) // rename transitions
    {
        auto pair = std::make_pair(names.at(transition.first.first), transition.first.second);
        newNFA.m_Transitions.emplace(pair, std::set<State>());
        for (State next : transition.second)
        {
            newNFA.m_Transitions.at(pair).emplace(names.at(next));
        }
    }
    for (State state : original.m_FinalStates) // rename final states
    {
        newNFA.m_FinalStates.emplace(names.at(state));
    }
    newNFA.m_InitialState = names.at(original.m_InitialState); // rename initial state
    TotalStates = NOStates;
    return newNFA;
}

DFA intersect(const NFA &a, const NFA &b)
{
    //    std::cout << "intersecting ..." << std::endl;
    //    printNFA(a);
    //    printNFA(b);

    NFA resultNFA;
    if (a.m_FinalStates.find(a.m_InitialState) != a.m_FinalStates.end() && b.m_FinalStates.find(b.m_InitialState) != b.m_FinalStates.end())
    { // add final state, if both states are final
        resultNFA.m_FinalStates.insert(0);
    }

    State lastStateFromA;
    NFA B = rename(b, true, lastStateFromA);
    NFA A = rename(a, false, lastStateFromA);
    //    std::cout << "renamed (" << lastStateFromA << ")" << std::endl;
    //    printNFA(A);
    //    printNFA(B);

    // unify alphabet
    resultNFA.m_Alphabet = a.m_Alphabet;
    std::set<Symbol> alphabet = b.m_Alphabet;
    resultNFA.m_Alphabet.merge(alphabet);

    // prepare maps for ceonvertions
    std::queue<std::pair<State, State>> queue;
    std::map<std::pair<State, State>, State> setToNewState;
    std::map<State, std::pair<State, State>> newToSetState;

    // add initial state
    std::pair<State, State> currentSetState{A.m_InitialState, B.m_InitialState};
    resultNFA.m_InitialState = 0;
    resultNFA.m_States.emplace(0);
    setToNewState.emplace(currentSetState, 0);
    newToSetState.emplace(0, currentSetState);
    queue.push(currentSetState);
    State NONewStates = 1;

    while (!queue.empty()) // while there are some states that need to be taken care of
    {
        currentSetState = queue.front();
        queue.pop();
        for (Symbol symbol : resultNFA.m_Alphabet) // for every symbol and every state in the set find next states
        {
            std::set<State> nextStatesA;
            std::set<State> nextStatesB;
            try // try to find transitions for the states and symbols, if not found continue to the next symbol
            {
                nextStatesA = A.m_Transitions.at(std::make_pair(currentSetState.first, symbol));
                nextStatesB = B.m_Transitions.at(std::make_pair(currentSetState.second, symbol));
            }
            catch (...)
            {
                continue;
            }
            // crossover of transitions
            auto keyPair = std::make_pair(setToNewState.at(currentSetState), symbol);
            auto it = resultNFA.m_Transitions.emplace(keyPair, std::set<State>());
            for (State stateA : nextStatesA)
            {
                for (State stateB : nextStatesB)
                {
                    std::pair<State, State> pair(stateA, stateB);
                    // if the state is new: add as new state, add to transitions, add to maps
                    if (setToNewState.emplace(pair, NONewStates).second)
                    {
                        newToSetState.emplace(NONewStates, pair);
                        resultNFA.m_Transitions.at(keyPair).emplace(NONewStates);
                        if (A.m_FinalStates.find(stateA) != A.m_FinalStates.end() && B.m_FinalStates.find(stateB) != B.m_FinalStates.end())
                        { // add final state, if both states are final
                            resultNFA.m_FinalStates.insert(NONewStates);
                        }
                        resultNFA.m_States.emplace(NONewStates++);
                        queue.emplace(std::move(pair));
                    }
                    else
                    { // if already created add transition
                        resultNFA.m_Transitions.at(keyPair).emplace(setToNewState.at(pair));
                    }
                }
            }
            if (it.first->second.empty())
                resultNFA.m_Transitions.erase(it.first); // if all transitions are empty erase it
        }
    }

    // convert the result to minimal DFA
//    std::cout << "--intersected" << std::endl;
//    printNFA(resultNFA);
    DFA resultDFA = determinize(resultNFA);
    minimize(resultDFA);
    return resultDFA;
}

/* DFA intersect(const NFA &a, const NFA &b)
{
    std::cout << "intersecting ..." << std::endl;
    printNFA(a);
    printNFA(b);
    NFA resultNFA;

    // unify alphabet
    resultNFA.m_Alphabet = a.m_Alphabet;
    std::set<Symbol> alphabet = b.m_Alphabet;
    resultNFA.m_Alphabet.merge(alphabet);

    // convert names of states
    std::map<State, State> originalToNewA; // convert original names to new ones
    std::map<State, State> newToOriginalA; // convert new names to original ones
    std::map<State, State> originalToNewB; // convert original names to new ones
    std::map<State, State> newToOriginalB; // convert new names to original ones
    State NOStatesA = 1;
    for (State state : a.m_States)
    {
        originalToNewA.emplace(state, NOStatesA);
        newToOriginalA.emplace(NOStatesA++, state);
    }
    State NOStatesB = -1;
    for (State state : b.m_States)
    {
        originalToNewB.emplace(state, NOStatesB);
        newToOriginalB.emplace(NOStatesB--, state);
    }

    // BFS like
    std::map<std::set<State>, State> setToState;
    std::queue<std::set<State>> queue;
    State NOStates = 0;

    // add initial state
    // current states are newly created
    std::set<State> currentSetState{originalToNewA.at(a.m_InitialState), originalToNewB.at(b.m_InitialState)};
    resultNFA.m_InitialState = NOStates;
    setToState.emplace(currentSetState, NOStates++);

    queue.push(std::move(currentSetState));
    // intersect and convert transitions
    while (!queue.empty())
    {
        currentSetState = queue.front();
        queue.pop();

        for (Symbol symbol : resultNFA.m_Alphabet) // for every symbol in the alphabet ...
        {
            std::set<State> nextStates;         // set with following states
            for (State state : currentSetState) // ... for every state in the current set
            {
                std::set<State> tmp1;
                try
                {
                    State origState = newToOriginalA.at(state); // find out from which automaton is the state => throws exception if not from A
                    // state is from automaton 'a'
                    try
                    {
                        std::set<State> tmp2 = a.m_Transitions.at(std::make_pair(origState, symbol));
                        for (State origState : tmp2) // rename all states
                        {
                            tmp1.emplace(originalToNewA.at(origState));
                        }
                    }
                    catch (...) // state is from 'a' but there are no transitions defined for it
                    {
                        goto jumpIntersect;
                    }
                }
                catch (...) // it is from the second automaton
                {
                    try
                    {
                        State origState = newToOriginalB.at(state);
                        std::set<State> tmp2 = b.m_Transitions.at(std::make_pair(origState, symbol));
                        for (State origState : tmp2) // rename all states
                        {
                            tmp1.emplace(originalToNewB.at(origState));
                        }
                    }
                    catch (...) // state is from 'b' but there are no transitions defined for it
                    {
                        goto jumpIntersect;
                    }
                }
                nextStates.merge(tmp1);
            }
            if (setToState.emplace(nextStates, NOStates).second) // try to insert as new state
            {
                NOStates++;
                queue.push(nextStates);
            }
            resultNFA.m_Transitions.emplace(std::make_pair(setToState.at(currentSetState), symbol), std::set<State>());
            resultNFA.m_Transitions.at(std::make_pair(setToState.at(currentSetState), symbol)).emplace(setToState.at(nextStates));
        jumpIntersect:
            continue;
        }
    }

    // convert states and add final states
    for (auto it = setToState.begin(); it != setToState.end(); ++it)
    {
        resultNFA.m_States.emplace(it->second);
        for (State state : it->first) // if all states in the set are final states, add final state
        {
            if ((a.m_FinalStates.find(state) == a.m_FinalStates.end()) && (b.m_FinalStates.find(state) == b.m_FinalStates.end()))
            {
                goto notFinal; // jump over the addition into finals, if a state is not final
            }
        }
        resultNFA.m_FinalStates.emplace(it->second);
    notFinal:
        continue;
    }

    std::cout << "--intersected" << std::endl;
    printNFA(resultNFA);
    DFA resultDFA = determinize(resultNFA);
    minimize(resultDFA);
    return resultDFA;
} */

#ifndef __PROGTEST__

// You may need to update this function or the sample data if your state naming strategy differs.
bool operator==(const DFA &a, const DFA &b)
{
    return std::tie(a.m_States, a.m_Alphabet, a.m_Transitions, a.m_InitialState, a.m_FinalStates) == std::tie(b.m_States, b.m_Alphabet, b.m_Transitions, b.m_InitialState, b.m_FinalStates);
}

int main()
{
    std::cout << "START" << std::endl;
    NFA test {
 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19} ,
 {'1','2'},
        {
                {{0,'1'},{3}},
                {{0,'1'},{18}},
                {{0,'2'},{5}},
                {{1,'1'},{11}},
                {{1,'2'},{16}},
                {{2,'1'},{6}},
                {{3,'2'},{11}},
                {{4,'1'},{6}},
                {{4,'1'},{8}},
                {{4,'2'},{9}},
                {{5,'1'},{6}},
                {{5,'1'},{14}},
                {{5,'1'},{19}},
                {{5,'2'},{4}},
                {{5,'2'},{7}},
                {{7,'2'},{1}},
                {{7,'2'},{2}},
                {{7,'2'},{7}},
                {{7,'2'},{9}},
                {{8,'1'},{8}},
                {{8,'1'},{13}},
                {{9,'1'},{11}},
                {{9,'2'},{12}},
                {{10,'1'},{3}},
                {{10,'1'},{4}},
                {{10,'1'},{15}},
                {{11,'1'},{2}},
                {{11,'2'},{13}},
                {{13,'1'},{12}},
                {{13,'1'},{14}},
                {{13,'1'},{17}},
                {{15,'2'},{12}},
                {{16,'1'},{7}},
                {{16,'2'},{6}},
                {{16,'2'},{17}},
                {{17,'1'},{1}},
                {{18,'1'},{4}},
                {{18,'1'},{10}},
                {{18,'2'},{1}},
                {{19,'1'},{1}},
        },
 0 ,
 {2, 3, 6, 11, 12, 14, 15, 17, 19}
};

NFA test2 {
 {0, 1, 2, 3, 4} ,
 {'0','2'},
        {
                {{0,'0'},{1}},
                {{0,'0'},{2}},
                {{0,'0'},{3}},
                {{0,'2'},{3}},
                {{0,'2'},{4}},
                {{1,'0'},{0}},
                {{1,'0'},{2}},
                {{1,'2'},{1}},
                {{2,'2'},{1}},
                {{2,'2'},{3}},
                {{2,'2'},{4}},
                {{3,'0'},{0}},
                {{3,'2'},{0}},
                {{3,'2'},{1}},
                {{3,'2'},{2}},
                {{3,'2'},{3}},
                {{3,'2'},{4}},
                {{4,'0'},{3}},
                {{4,'2'},{2}},
                {{4,'2'},{4}},
        },
 0 ,
 {0, 1, 4}
};

printDFA(determinize(test2));
    
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
    /* DFA a b
    >0 2 -
    <1 1 3
    2 1 -
    3 4 3
    4 1 3 */
    std::cout << "Start intersecting..." << std::endl;
    printDFA(a);
    printDFA(intersect(a1, a2));
    // assert(intersect(a1, a2) == a);
    std::cout << "SUCCESS intersecting" << std::endl;

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
    std::cout << "Start unifying..." << std::endl;
    /* DFA a b
    >0 1 2
    <1 - -
    2 5 8
    <3 3 8
    <4 4 4
    5 3 6
    6 4 8
    7 3 8
    8 7 8 */
    printDFA(b);
    printDFA(unify(b1, b2));
    // assert(unify(b1, b2) == b);
    std::cout << "SUCCESS unifying" << std::endl;

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
    std::cout << "Start intersecting..." << std::endl;
    assert(intersect(c1, c2) == c);
    std::cout << "SUCCESS intersecting" << std::endl;

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
    std::cout << "Start intersecting..." << std::endl;
    assert(intersect(d1, d2) == d);
    std::cout << "SUCCESS intersecting" << std::endl;;

    std::cout << "!!! SUCCESS !!!" << std::endl;
}
#endif
