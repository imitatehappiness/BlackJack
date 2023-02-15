#ifndef TD_H
#define TD_H
#include <structs.h>
#include <QMap>
#include <stdio.h>
#include <stdlib.h>
#include <QVector>
#include <algorithm>
#include <QList>
#include <QFont>
#include <QVector3D>
#include <QRandomGenerator>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>


class TD
{
public:
    TD();
    virtual ~TD();
    int StepTDSarsa(State &state, Action action);
    QMap<int, QMap<int, QMap<Action, float> > > getQTD() const;
    QVector<QMap<int, QMap<int, QMap<Action, float> > > > getRTD() const;
    int getCOUNT_GAME_TD() const;
    float getLAMBDA() const;
    CardColor GetColor();
    int GetCard();
    void setCOUNT_GAME_TD(int value);
    void setLAMBDA(float value);

protected:
    ///@brief  QTD - key [DillerCard AgentSum Action] Value
    ///@brief  NTD - key [DillerCard AgentSum Action] CountVisit

    QMap<int, QMap<int, QMap<Action, float>>> QTD;
    QMap<int, QMap<int, QMap<Action, int>>>   NTD;
    QMap<int, QMap<int, QMap<Action, float>>> ETD;
    QVector<QMap<int, QMap<int, QMap<Action, float>>>> RTDLambda;
    int  WINTD = 0, LOSETD = 0, DRAWTD = 0;

    QTime *timeAlgorithm;
    int COUNT_GAME_TD = 100000;
    float LAMBDA = 0;
};

class TDSarsa: public TD{
public:
    TDSarsa(): TD(){}
    void BlakcJackTDGame();
    void OneBlakcJackGameTDSarsa();
    void UpdateQTDSarsa(State oldState, State state, Action oldAction, Action action, int reward);
    Action GetActionTDSarsa(State state, float eps);
};

class TDLinearSarsa: public TD{
public:
    TDLinearSarsa(): TD(){}
    void BlakcJackTDGame();
    void OneBlakcJackGameTDSarsa();
    void UpdateQTDSarsa(State oldState, State state, Action oldAction, Action action, int reward);
    double LCF();
    void fillPhi(Action action, State state);
    Action GetActionTDSarsa(State state, float eps);
    QVector<int> getIndexByState(State state, Action action);
    StateAction getStateActionByIndex(int index);
private:
    QVector<QVector<double>> dealer_s{{1, 4}, {4, 7}, {7, 10}};
    QVector<QVector<double>> player_s{{1, 6}, {4, 9}, {7, 12}, {10, 15}, {13, 18}, {16, 21}};
    QVector<double> phi;
    QVector<double> teta;
    int SIZE_PHI_TETA = 36;
    QVector<double> ETDLinear;


};

#endif // TD_H
