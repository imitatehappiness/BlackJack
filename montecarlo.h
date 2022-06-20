#ifndef MONTECARLO_H
#define MONTECARLO_H
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

class MonteCarlo
{
public:
    MonteCarlo();
    ~MonteCarlo();
    CardColor GetColor();
    int GetCard();

    ///@brief Monte-Carlo
    void BlakcJackGameMonteCarlo();
    QVector<ValueStep> OneBlakcJackGameMonteCarlo();
    void UpdateQMonteCarlo(QVector<ValueStep> episode);
    Action GetActionMonteCarlo(State state, float eps);
    int StepMonteCarlo(State &state, Action action);
    void DrawSurfacePlotMonteCarlo();
    void SetGradientMonteCarlo();
    QMap<int, QMap<int, QMap<Action, float> > > getQMonteCarlo() const;
    QVector<QMap<int, QMap<int, QMap<Action, float> > > > getRMonteCarlo() const;
    int getCOUNT_GAME_MC() const;
    void setCOUNT_GAME_MC(int value);

private:
    ///@brief  QMonteCarlo - key [DillerCard AgentSum Action] Value
    ///@brief  NMonteCarlo - key [DillerCard AgentSum Action] CountVisit
    QMap<int, QMap<int, QMap<Action, float>>> QMonteCarlo;
    QMap<int, QMap<int, QMap<Action, int>>>   NMonteCarlo;
    QVector<QMap<int, QMap<int, QMap<Action, float>>>> RMonteCarlo;

    QTime *timeAlgorithm;
    int COUNT_GAME_MC;
};

#endif // MONTECARLO_H
