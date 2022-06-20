#ifndef STRUCTS_H
#define STRUCTS_H
enum CardColor{Black = 1, Red  = -1};
enum Action{Hit = 1 /*еще*/, Stick  = 2/*хватит*/};
enum TDSarsaParam{Episode, Lambda, LinearEpisode, LinearLambda};
#define DEBUG_PRINT

struct State{
    int cardDiler;
    int sumAgent;
};

struct StateAction{
    State state;
    Action action;
};

struct ValueStep{
    void SetParams(State s, Action a, float r){
        state = s;
        action = a;
        reward = r;
    }

    State state;
    Action action;
    float reward;
};
#endif // STRUCTS_H
