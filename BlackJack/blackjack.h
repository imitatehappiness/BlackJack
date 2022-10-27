#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <QMainWindow>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <QVector>
#include <QMap>
#include <qapplication.h>
#include <algorithm>
#include <QtDataVisualization>
#include <QCustom3DLabel>
#include <Q3DSurface>
#include <QList>
#include <QPropertyAnimation>
#include <QFont>
#include <QVector3D>
#include <QDebug>
#include <qcustomplot.h>
#include <structs.h>
#include <montecarlo.h>
#include <td.h>

QT_BEGIN_NAMESPACE
namespace Ui { class BlackJack; }
namespace QtDataVisualization{
    class Q3DSurface;
    class QCustom3DLabel;
}
class QPropertyAnimation;
QT_END_NAMESPACE

using namespace std;

class BlackJack : public QMainWindow{
    Q_OBJECT

public:
    explicit BlackJack(QWidget *parent = 0);
    ~BlackJack();
    ///@brief Monte-Carlo
    void DrawSurfacePlotMonteCarlo();
    void SetGradientMonteCarlo();

    ///@brief TDSarsa
    void DrawTrainingGrTDSarsaLambda(TD *BlackJackTD);
    void DrawTrainingGrTDSarsaEpisode(TD *BlackJackTD);
    void TDSarsaEpisode();
    void TDSarsaLambda();
    void TDLinearSarsaEpisode();
    void TDLinearSarsaLambda();
    void BJTD(TDSarsaParam param);
    void BJMC();
private:
    Ui::BlackJack *ui;

    ///@brief Monte-Carlo
    MonteCarlo *BlackJackMC;
    QMap<int, QMap<int, QMap<Action, float>>> QMonteCarlo;
    QtDataVisualization::Q3DSurface *surfacePlotMonteCarlo;
    QVector<QMap<int, QMap<int, QMap<Action, float>>>> RMonteCarlo;

    ///@brief TDSarsa
    TDSarsa *BlackJackTDSarsa;
    QMap<int, QMap<int, QMap<Action, float>>> QTD;
    QtDataVisualization::Q3DSurface *surfacePlotTDSarsa;
    QCustomPlot *graphTrainingTDSarsaLambda;
    QCustomPlot *graphTrainingTDSarsaEpisode;

    QVector<QMap<int, QMap<int, QMap<Action, float>>>> QTDLambda;
    QVector<QMap<int, QMap<int, QMap<Action, float>>>> RTDLambda;

    ///@brief TDLinearSarsa
    TDLinearSarsa *BlackJackTDLinearSarsa;
    TDSarsaParam TDSARSAParam;
};

#endif // BLACKJACK_H
