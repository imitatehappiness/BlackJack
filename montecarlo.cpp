#include "montecarlo.h"

const float N0 = 500;
const float nA = 2;

bool gameNotOverMonteCarlo;
int  WINMC = 0, LOSEMC = 0, DRAWMC = 0;

MonteCarlo::MonteCarlo(){
}

MonteCarlo::~MonteCarlo(){
    delete timeAlgorithm;
}

QMap<int, QMap<int, QMap<Action, float> > > MonteCarlo::getQMonteCarlo() const{
    return QMonteCarlo;
}

QVector<QMap<int, QMap<int, QMap<Action, float> > > > MonteCarlo::getRMonteCarlo() const{
    return RMonteCarlo;
}

int MonteCarlo::getCOUNT_GAME_MC() const{
    return COUNT_GAME_MC;
}

void MonteCarlo::setCOUNT_GAME_MC(int value){
    COUNT_GAME_MC = value;
}

/// ====== Monte-Carlo ======///
void MonteCarlo::BlakcJackGameMonteCarlo(){
    qDebug()<<"BlakcJackMCGame";
    timeAlgorithm = new QTime;
    timeAlgorithm->start();

    for(int idGame=1;idGame<=COUNT_GAME_MC;idGame++){
        QVector<ValueStep> episode =  OneBlakcJackGameMonteCarlo();
        if(idGame % 1000 == 0){
            RMonteCarlo.append(QMonteCarlo);
        }
        UpdateQMonteCarlo(episode);
    }

    qDebug()<<"WINMC:  "<<WINMC; qDebug()<<"LOSEMC: "<<LOSEMC; qDebug()<<"DRAWMC: "<<DRAWMC;
    qDebug()<<"Time Algotithm MC:"<<timeAlgorithm->elapsed()<<"ms"<<"\n";
}

QVector<ValueStep> MonteCarlo::OneBlakcJackGameMonteCarlo(){
    QVector<ValueStep> episode;
    State state;
    state.cardDiler = GetCard();
    state.sumAgent  = GetCard();
    gameNotOverMonteCarlo = true;
    while(gameNotOverMonteCarlo){
        int Nst = NMonteCarlo[state.cardDiler][state.sumAgent][Hit] + NMonteCarlo[state.cardDiler][state.sumAgent][Stick];
        float eps = static_cast<float>(N0) / static_cast<float>(N0 + Nst);
        State tmpState = state;
        Action action =  GetActionMonteCarlo(tmpState, eps);
        int reward = StepMonteCarlo(state, action);
        ValueStep tmpValueStep;
        tmpValueStep.SetParams(tmpState, action, reward);
        episode.append(tmpValueStep);
        NMonteCarlo[tmpState.cardDiler][tmpState.sumAgent][action] += 1;
        qApp->processEvents();

    }
    return episode;
}

int MonteCarlo::StepMonteCarlo(State& state, Action action){
    int reward = 0;
    if(action == Hit){
        state.sumAgent += GetCard() * GetColor();
        if (state.sumAgent > 21 || state.sumAgent < 0){
            reward = -1;
            gameNotOverMonteCarlo = false;
            LOSEMC++;
            return reward;
        }
    }else if(action == Stick){
        int sumDealer = state.cardDiler;
        while(true){
            if(sumDealer < 17 && sumDealer >=0){
                sumDealer += GetCard() * GetColor();
            }else {
                break;
            }
        }//end while

        if(sumDealer > 21 || sumDealer < 0){
            reward = 1;
        }else if(state.sumAgent > sumDealer){
            reward = 1;
        }else if(state.sumAgent < sumDealer){
            reward = -1;
        }
        gameNotOverMonteCarlo = false;

        if(reward == 1){
            WINMC++;
        }else if(reward == 0){
            DRAWMC++;
        }else if(reward == -1){
            LOSEMC++;
        }
    }//end Stick
    return reward;
}

void MonteCarlo::UpdateQMonteCarlo(QVector<ValueStep> episode){
    for(ValueStep tmp : episode){
        int ind = -1;

        for(int x = 0;x<episode.size(); x++){
            if(episode[x].state.cardDiler == tmp.state.cardDiler){
                ind = x;
            }
        }

        float G = 0;
        for(int i = ind;i<episode.size();i++){
            G += episode[i].reward;
        }
        float alpha =  (static_cast<float>(1)/static_cast<float>(NMonteCarlo[tmp.state.cardDiler][tmp.state.sumAgent][tmp.action]));
        float Qsa = QMonteCarlo[tmp.state.cardDiler][tmp.state.sumAgent][tmp.action];
        QMonteCarlo[tmp.state.cardDiler][tmp.state.sumAgent][tmp.action] =
                Qsa + alpha*(G - Qsa);
    }
}

Action MonteCarlo::GetActionMonteCarlo(State state, float eps){
    float k = static_cast<float>(QRandomGenerator::global()->bounded(1, 101)) / static_cast<float>(100);

    if(k <= eps/nA + 1 - eps){
        if(QMonteCarlo[state.cardDiler][state.sumAgent][Hit] <= QMonteCarlo[state.cardDiler][state.sumAgent][Stick]){
            return Stick;
        }else{
            return Hit;
        }
    }else if(QMonteCarlo[state.cardDiler][state.sumAgent][Hit] > QMonteCarlo[state.cardDiler][state.sumAgent][Stick]){
        return Stick;
    }else{
        return Hit;
    }
}

CardColor MonteCarlo::GetColor(){
    return QRandomGenerator::global()->bounded(1, 4) > 1 ? Black : Red;
}

int MonteCarlo::GetCard(){
    return QRandomGenerator::global()->bounded(1, 11);
}
