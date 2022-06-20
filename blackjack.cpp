#include "blackjack.h"
#include "ui_blackjack.h"

int COUNT_GAME = 10000; /* if  TDSarsaParam == Lambda || LinearLambda => COUNT_GAME = 11000 else COUNT_GAME = 10000; */
double LAMBDA_TD = 0;
using namespace QtDataVisualization;

BlackJack::BlackJack(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BlackJack)
{
    ui->setupUi(this);

    /// @brief MC
    BJMC();

    /// @brief TDSARSA
    /// @param Lambda - зависимость среднеквадр. ошибки от lambda #2
    /// @param Episode - зависимость среднеквадратической ошибки от номера эпизода #2
    /// @param LinearLambda - зависимость среднеквадр. ошибки от lambda #3
    /// @param LinearEpisode - зависимость среднеквадратической ошибки от номера эпизода #3
    BJTD(LinearEpisode);
}

BlackJack::~BlackJack(){
    delete ui;
}

void BlackJack::BJMC(){
    BlackJackMC = new MonteCarlo();
    BlackJackMC->setCOUNT_GAME_MC(COUNT_GAME);
    BlackJackMC->BlakcJackGameMonteCarlo();
    QMonteCarlo = BlackJackMC->getQMonteCarlo();
    RMonteCarlo = BlackJackMC->getRMonteCarlo();
    DrawSurfacePlotMonteCarlo();
}

void BlackJack::BJTD(TDSarsaParam param){
    switch(param){
    case Episode: TDSarsaEpisode(); break;
    case Lambda: TDSarsaLambda(); break;
    case LinearEpisode: TDLinearSarsaEpisode(); break;
    case LinearLambda: TDLinearSarsaLambda(); break;
    default: break;
    };
}

void BlackJack::TDSarsaEpisode(){
    BlackJackTDSarsa = new TDSarsa();
    BlackJackTDSarsa->setLAMBDA(LAMBDA_TD);
    BlackJackTDSarsa->setCOUNT_GAME_TD(COUNT_GAME);
    BlackJackTDSarsa->BlakcJackTDGame();
    QTD = BlackJackTDSarsa->getQTD();
    RTDLambda = BlackJackTDSarsa->getRTD();
    DrawTrainingGrTDSarsaEpisode(BlackJackTDSarsa);
    delete BlackJackTDSarsa;
    delete BlackJackMC;
}

void BlackJack::TDSarsaLambda(){
    for(double lambda = 0;lambda<1.01;lambda+=0.1){
        BlackJackTDSarsa = new TDSarsa();
        BlackJackTDSarsa->setLAMBDA(lambda);
        BlackJackTDSarsa->setCOUNT_GAME_TD(1000);
        BlackJackTDSarsa->BlakcJackTDGame();
        QTDLambda.append(BlackJackTDSarsa->getQTD());
        delete BlackJackTDSarsa;
    }
    DrawTrainingGrTDSarsaLambda(BlackJackTDSarsa);
    delete BlackJackMC;
}

void BlackJack::TDLinearSarsaEpisode(){
    BlackJackTDLinearSarsa = new TDLinearSarsa();
    BlackJackTDLinearSarsa->setLAMBDA(LAMBDA_TD);
    BlackJackTDLinearSarsa->setCOUNT_GAME_TD(COUNT_GAME);
    BlackJackTDLinearSarsa->BlakcJackTDGame();
    QTD = BlackJackTDLinearSarsa->getQTD();
    RTDLambda = BlackJackTDLinearSarsa->getRTD();
    DrawTrainingGrTDSarsaEpisode(BlackJackTDLinearSarsa);
    delete BlackJackTDLinearSarsa;
    delete BlackJackMC;
}

void BlackJack::TDLinearSarsaLambda(){
    for(double lambda = 0;lambda<1.01;lambda+=0.1){
        BlackJackTDLinearSarsa = new TDLinearSarsa();
        BlackJackTDLinearSarsa->setLAMBDA(lambda);
        BlackJackTDLinearSarsa->setCOUNT_GAME_TD(1000);
        BlackJackTDLinearSarsa->BlakcJackTDGame();
        QTDLambda.append(BlackJackTDLinearSarsa->getQTD());
        delete BlackJackTDLinearSarsa;
    }
    DrawTrainingGrTDSarsaLambda(BlackJackTDLinearSarsa);
    delete BlackJackMC;
}

///// ====== Monte-Carlo ======///
void BlackJack::DrawSurfacePlotMonteCarlo(){
    surfacePlotMonteCarlo = new Q3DSurface;

    surfacePlotMonteCarlo->axisX()->setTitle(QStringLiteral("Player sum"));
    surfacePlotMonteCarlo->axisY()->setTitle(QStringLiteral("Reward"));
    surfacePlotMonteCarlo->axisZ()->setTitle(QStringLiteral("Dealer card"));

    surfacePlotMonteCarlo->axisX()->setTitleVisible(true);
    surfacePlotMonteCarlo->axisY()->setTitleVisible(true);
    surfacePlotMonteCarlo->axisZ()->setTitleVisible(true);

    QWidget *w = QWidget::createWindowContainer(surfacePlotMonteCarlo);
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(w);
    ui->widgetMonteCarloSurface->setLayout(lay);
    QSurface3DSeries *series = new QSurface3DSeries;

    const int COUNT_X = 10;
    const int COUNT_Z = 21;

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(COUNT_X);

    for (int i = 1 ; i <= COUNT_X ; i++) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(COUNT_Z+1-12);
        double x = i;
        int index = 0;
        for (int j = 12; j <= COUNT_Z ; j++) {
            double z = j;
            double y;
            QMonteCarlo[x][z][Hit]<QMonteCarlo[x][z][Stick] ?  y = QMonteCarlo[x][z][Stick] : y = QMonteCarlo[x][z][Hit];
            (*newRow)[index++].setPosition(QVector3D(z, y, x));
        }
        *dataArray << newRow;
    }

    series->dataProxy()->resetArray(dataArray);
    series->setBaseColor(Qt::black);

    surfacePlotMonteCarlo->addSeries(series);
    SetGradientMonteCarlo();
    surfacePlotMonteCarlo->show();
}

void BlackJack::SetGradientMonteCarlo(){
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    surfacePlotMonteCarlo->seriesList().at(0)->setBaseGradient(gr);
    surfacePlotMonteCarlo->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    surfacePlotMonteCarlo->activeTheme()->setBackgroundColor(Qt::darkGray);
    surfacePlotMonteCarlo->activeTheme()->setWindowColor(Qt::white);
    surfacePlotMonteCarlo->activeTheme()->setLabelTextColor(Qt::black);
    surfacePlotMonteCarlo->activeTheme()->setFont(QFont(QStringLiteral("Times"), 35));
    surfacePlotMonteCarlo->activeTheme()->setLabelBackgroundColor(QColor(220, 220, 220));
    surfacePlotMonteCarlo->activeTheme()->setLightColor(Qt::white);
    surfacePlotMonteCarlo->activeTheme()->setGridLineColor(Qt::black);
}

///// ====== TD ======///
void BlackJack::DrawTrainingGrTDSarsaLambda(TD *BlackJackTD){
    graphTrainingTDSarsaLambda = new QCustomPlot();

    graphTrainingTDSarsaLambda->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    graphTrainingTDSarsaLambda->legend->setFont(legendFont);
    graphTrainingTDSarsaLambda->legend->setBrush(QBrush(QColor(255,255,255,230)));

    ui->gridLayout_3->addWidget(graphTrainingTDSarsaLambda);
    graphTrainingTDSarsaLambda->setBackground(QBrush(QColor("#EEEEEE")));
    graphTrainingTDSarsaLambda->plotLayout()->insertRow(0);
    graphTrainingTDSarsaLambda->plotLayout()->addElement(0, 0, new QCPTextElement(graphTrainingTDSarsaLambda, "SARSA LAMBDA", QFont("arial", 12, QFont::Bold)));

    graphTrainingTDSarsaLambda->setInteraction(QCP::iRangeZoom, true);
    graphTrainingTDSarsaLambda->setInteraction(QCP::iRangeDrag, true);
    graphTrainingTDSarsaLambda->axisRect()->setRangeDrag(Qt::Horizontal);
    graphTrainingTDSarsaLambda->axisRect()->setRangeZoom(Qt::Vertical);

    graphTrainingTDSarsaLambda->xAxis->setLabel("lambda");
    graphTrainingTDSarsaLambda->yAxis->setLabel("RMS between MC and TDSarsa");

    const int DilerC = 10;
    const int AgentS = 21;

    int sizeVec = BlackJackMC->getCOUNT_GAME_MC() == BlackJackTD->getCOUNT_GAME_TD() ? sizeVec = BlackJackMC->getCOUNT_GAME_MC() / 1000 : sizeVec = 0;

    QVector<double> x1 , y1;

    int maxSum = 0;
    for(int i=0;i<QTDLambda.size();i++){
        double sum = 0;

        for(int keyDilerCard = 1; keyDilerCard <= DilerC; keyDilerCard++){
            for(int keyAgentSum = 0; keyAgentSum <= AgentS; keyAgentSum++){
                sum += (QTDLambda[i][keyDilerCard][keyAgentSum][Hit] - QMonteCarlo[keyDilerCard][keyAgentSum][Hit]) *
                        (QTDLambda[i][keyDilerCard][keyAgentSum][Hit] - QMonteCarlo[keyDilerCard][keyAgentSum][Hit]);

                sum += (QTDLambda[i][keyDilerCard][keyAgentSum][Stick] - QMonteCarlo[keyDilerCard][keyAgentSum][Stick]) *
                        (QTDLambda[i][keyDilerCard][keyAgentSum][Stick] - QMonteCarlo[keyDilerCard][keyAgentSum][Stick]);

            }
        }

        if(maxSum < sum){
            maxSum = sum;
        }

        x1.append(static_cast<double>(i)/10);
        y1.append(sum);
    }

    graphTrainingTDSarsaLambda->xAxis2->setVisible(true);
    graphTrainingTDSarsaLambda->xAxis2->setTickLabels(false);
    graphTrainingTDSarsaLambda->yAxis2->setVisible(true);
    graphTrainingTDSarsaLambda->yAxis2->setTickLabels(false);

    ///@brief MC-TD
    graphTrainingTDSarsaLambda->addGraph(graphTrainingTDSarsaLambda->xAxis, graphTrainingTDSarsaLambda->yAxis);
    graphTrainingTDSarsaLambda->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 5));
    graphTrainingTDSarsaLambda->graph(0)->setPen(QPen("#0D7377"));
    graphTrainingTDSarsaLambda->graph(0)->setData(x1,y1);
    graphTrainingTDSarsaLambda->graph(0)->setName("зависимость среднеквадр. ошибки от lambda");

    graphTrainingTDSarsaLambda->xAxis->setRange(-0.1, 1.1);
    graphTrainingTDSarsaLambda->yAxis->setRange(0, maxSum + maxSum/2);
    graphTrainingTDSarsaLambda->replot();
}

void BlackJack::DrawTrainingGrTDSarsaEpisode(TD *BlackJackTD){

    graphTrainingTDSarsaEpisode = new QCustomPlot();

    graphTrainingTDSarsaEpisode->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    graphTrainingTDSarsaEpisode->legend->setFont(legendFont);
    graphTrainingTDSarsaEpisode->legend->setBrush(QBrush(QColor(255,255,255,230)));

    ui->gridLayout_5->addWidget(graphTrainingTDSarsaEpisode);
    graphTrainingTDSarsaEpisode->setBackground(QBrush(QColor("#EEEEEE")));
    graphTrainingTDSarsaEpisode->plotLayout()->insertRow(0);
    graphTrainingTDSarsaEpisode->plotLayout()->addElement(0, 0, new QCPTextElement(graphTrainingTDSarsaEpisode, "SARSA EPISODE", QFont("arial", 12, QFont::Bold)));

    graphTrainingTDSarsaEpisode->setInteraction(QCP::iRangeZoom, true);
    graphTrainingTDSarsaEpisode->setInteraction(QCP::iRangeDrag, true);
    graphTrainingTDSarsaEpisode->axisRect()->setRangeDrag(Qt::Horizontal);
    graphTrainingTDSarsaEpisode->axisRect()->setRangeZoom(Qt::Vertical);

    graphTrainingTDSarsaEpisode->xAxis->setLabel("episode");
    graphTrainingTDSarsaEpisode->yAxis->setLabel("RMS between MC and TDSarsa");

    const int DilerC = 10;
    const int AgentS = 21;

    int sizeVec = BlackJackMC->getCOUNT_GAME_MC() == BlackJackTD->getCOUNT_GAME_TD() ? sizeVec = BlackJackMC->getCOUNT_GAME_MC() / 1000 : sizeVec = 0;

    QVector<double> x1 , y1;
    int maxSum = 0;

    for(int i=0;i<RTDLambda.size();i++){
        double sum0 = 0;

        for(int keyDilerCard = 1; keyDilerCard <= DilerC; keyDilerCard++){
            for(int keyAgentSum = 0; keyAgentSum <= AgentS; keyAgentSum++){
                sum0 += (RTDLambda[i][keyDilerCard][keyAgentSum][Hit] - QMonteCarlo[keyDilerCard][keyAgentSum][Hit]) *
                        (RTDLambda[i][keyDilerCard][keyAgentSum][Hit] - QMonteCarlo[keyDilerCard][keyAgentSum][Hit]);

                sum0 += (RTDLambda[i][keyDilerCard][keyAgentSum][Stick] - QMonteCarlo[keyDilerCard][keyAgentSum][Stick]) *
                        (RTDLambda[i][keyDilerCard][keyAgentSum][Stick] - QMonteCarlo[keyDilerCard][keyAgentSum][Stick]);

            }
        }

        if(maxSum < sum0){
            maxSum = sum0;
        }

        x1.append(i+1);
        y1.append(sum0);

    }

    graphTrainingTDSarsaEpisode->xAxis2->setVisible(true);
    graphTrainingTDSarsaEpisode->xAxis2->setTickLabels(false);
    graphTrainingTDSarsaEpisode->yAxis2->setVisible(true);
    graphTrainingTDSarsaEpisode->yAxis2->setTickLabels(false);

    graphTrainingTDSarsaEpisode->addGraph(graphTrainingTDSarsaEpisode->xAxis, graphTrainingTDSarsaEpisode->yAxis);
    graphTrainingTDSarsaEpisode->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 5));
    graphTrainingTDSarsaEpisode->graph(0)->setPen(QPen("#0D7377"));
    graphTrainingTDSarsaEpisode->graph(0)->setData(x1,y1);
    graphTrainingTDSarsaEpisode->graph(0)->setName("lambda " + QString::number(LAMBDA_TD));

    graphTrainingTDSarsaEpisode->xAxis->setRange(-RTDLambda.size()/100, RTDLambda.size() + RTDLambda.size()/100);
    graphTrainingTDSarsaEpisode->yAxis->setRange(0, maxSum + maxSum/8);

    graphTrainingTDSarsaEpisode->replot();
}




