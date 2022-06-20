#include "td.h"

const float N0 = 500;
const float nA = 2;
const double GAMMA = 1;

bool gameNotOverTD;

TD::TD(){
    QTD.clear();
    NTD.clear();
    ETD.clear();
    RTDLambda.clear();
}

TD::~TD(){
    delete timeAlgorithm;
}

QMap<int, QMap<int, QMap<Action, float> > > TD::getQTD() const{
    return QTD;
}

QVector<QMap<int, QMap<int, QMap<Action, float> > > > TD::getRTD() const{
    return RTDLambda;
}

int TD::getCOUNT_GAME_TD() const{
    return COUNT_GAME_TD;
}

void TD::setCOUNT_GAME_TD(int value){
    COUNT_GAME_TD = value;
}

float TD::getLAMBDA() const{
    return LAMBDA;
}

void TD::setLAMBDA(float value){
    LAMBDA = value;
}

int TD::StepTDSarsa(State &state, Action action){
    int reward = 0;
    if(action == Hit){
        state.sumAgent += GetCard() * GetColor();
        if (state.sumAgent > 21 || state.sumAgent < 0){
            reward = -1;
            gameNotOverTD = false;
            LOSETD++;
            return reward;
        }
    }else if(action == Stick){
        int sumDealer = state.cardDiler;
        while(true){
            if(sumDealer < 17 && sumDealer >= 0){
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
        gameNotOverTD = false;

        if(reward == 1){
            WINTD++;
        }else if(reward == 0){
            DRAWTD++;
        }else if(reward == -1){
            LOSETD++;
        }
    }//end Stick
    return reward;
}



CardColor TD::GetColor(){
    return QRandomGenerator::global()->bounded(1, 4) > 1 ? Black : Red;
}

int TD::GetCard(){
    return QRandomGenerator::global()->bounded(1, 11);
}

///// ====== TDSarsa ======///

void TDSarsa::BlakcJackTDGame(){
    qDebug() << "BlakcJackTDGame";
    timeAlgorithm = new QTime;
    timeAlgorithm->start();

    qDebug() << "LAMBDA: " << LAMBDA;
    for(int idGame=1;idGame<=COUNT_GAME_TD;idGame++){
        OneBlakcJackGameTDSarsa();
        RTDLambda.append(QTD);

        if(idGame % 1000 == 0){
            qDebug() << idGame / 1000 << "/" << COUNT_GAME_TD / 1000;
        }

    }

    qDebug() << "WINTD:  " << WINTD; qDebug() << "LOSETD: " << LOSETD; qDebug() << "DRAWTD: "<<DRAWTD;
    qDebug() << "Time Algotithm TD:" << timeAlgorithm->elapsed() << "ms" << "\n";
}

void TDSarsa::OneBlakcJackGameTDSarsa(){
    State state;
    state.cardDiler = GetCard();
    state.sumAgent  = GetCard();
    gameNotOverTD = true;
    int Nst = NTD[state.cardDiler][state.sumAgent][Hit] + NTD[state.cardDiler][state.sumAgent][Stick];
    float eps = static_cast<float>(N0) / static_cast<float>(N0 + Nst);
    Action action =  GetActionTDSarsa(state, eps);

    while(gameNotOverTD){
        State tmpState = state;
        Action tmpAction = action;

        int reward = StepTDSarsa(state, action);
        NTD[tmpState.cardDiler][tmpState.sumAgent][tmpAction] += 1;

        Nst = NTD[state.cardDiler][state.sumAgent][Hit] + NTD[state.cardDiler][state.sumAgent][Stick];
        eps = static_cast<float>(N0) / static_cast<float>(N0 + Nst);
        Action action =  GetActionTDSarsa(state, eps);
        UpdateQTDSarsa(tmpState, state, tmpAction, action, reward);
        qApp->processEvents();

    }
}

Action TDSarsa::GetActionTDSarsa(State state, float eps){
    float k = static_cast<float>(QRandomGenerator::global()->bounded(1, 101)) / static_cast<float>(100);

    if(k <= eps / nA + 1 - eps){
        if(QTD[state.cardDiler][state.sumAgent][Hit] <= QTD[state.cardDiler][state.sumAgent][Stick]){
            return Stick;
        }else{
            return Hit;
        }
    }else if(QTD[state.cardDiler][state.sumAgent][Hit] > QTD[state.cardDiler][state.sumAgent][Stick]){
        return Stick;
    }else{
        return Hit;
    }
}

void TDSarsa::UpdateQTDSarsa(State oldState, State state, Action oldAction, Action action, int reward){
    float delta = static_cast<float>(reward) + GAMMA*QTD[state.cardDiler][state.sumAgent][action] - QTD[oldState.cardDiler][oldState.sumAgent][oldAction];
    ETD[oldState.cardDiler][oldState.sumAgent][oldAction] += 1;

    foreach (int keyCardDiller, QTD.keys()){
        foreach (int keySumAgent, QTD[keyCardDiller].keys()){
            foreach (Action keyAction, QTD[keyCardDiller][keySumAgent].keys()){
                float alpha =  (static_cast<float>(1)/static_cast<float>(NTD[keyCardDiller][keySumAgent][keyAction] + 1));
                QTD[keyCardDiller][keySumAgent][keyAction] += alpha * delta * ETD[keyCardDiller][keySumAgent][keyAction];
                if(QTD[keyCardDiller][keySumAgent][keyAction] > 1)
                    QTD[keyCardDiller][keySumAgent][keyAction] = 1;
                if(QTD[keyCardDiller][keySumAgent][keyAction] < -1)
                    QTD[keyCardDiller][keySumAgent][keyAction] = -1;
                ETD[keyCardDiller][keySumAgent][keyAction] *= static_cast<float>(GAMMA * LAMBDA);
            }
        }
    }
}

///// ====== TDLinearSarsa ======///

void TDLinearSarsa::BlakcJackTDGame(){
    qDebug() << "BlakcJackTDLinearSarsaGame";
    timeAlgorithm = new QTime;
    timeAlgorithm->start();
    qDebug() << "LAMBDA: " << LAMBDA;

    SIZE_PHI_TETA = dealer_s.size() * player_s.size() * 2;
    teta.clear();
    teta.resize(SIZE_PHI_TETA);

    for(int idGame=1;idGame<=COUNT_GAME_TD;idGame++){
        OneBlakcJackGameTDSarsa();
        RTDLambda.append(QTD);

        if(idGame % 1000 == 0){
            qDebug() << idGame / 1000 << "/" << COUNT_GAME_TD / 1000;
        }
    }
    qDebug() << "WINTD:  " << WINTD; qDebug() << "LOSETD: " << LOSETD; qDebug() << "DRAWTD: " << DRAWTD;
    qDebug() << "Time Algotithm TD:" << timeAlgorithm->elapsed() << "ms"<<"\n";
}

void TDLinearSarsa::OneBlakcJackGameTDSarsa(){
    const float eps = 0.05;

    State state;
    state.cardDiler = GetCard();
    state.sumAgent  = GetCard();
    gameNotOverTD = true;

    ETDLinear.clear();
    ETDLinear.resize(SIZE_PHI_TETA);

    Action action =  GetActionTDSarsa(state, eps);

    while(gameNotOverTD){
        State tmpState = state;
        Action tmpAction = action;
        int reward = StepTDSarsa(state, action);
        Action action = GetActionTDSarsa(state, eps);

        UpdateQTDSarsa(tmpState, state, tmpAction, action, reward);
        qApp->processEvents();

    }

    for(int keyDilerCadr = 1; keyDilerCadr <= 10; keyDilerCadr++){
        for(int keyAgentsum = 1; keyAgentsum <= 21; keyAgentsum++){
            State state;
            state.cardDiler = keyDilerCadr;
            state.sumAgent  = keyAgentsum;

            fillPhi(Hit, state);
            QTD[keyDilerCadr][keyAgentsum][Hit] = LCF();

            fillPhi(Stick, state);
            QTD[keyDilerCadr][keyAgentsum][Stick] = LCF();
        }
    }
}

Action TDLinearSarsa::GetActionTDSarsa(State state, float eps){
    float k = static_cast<float>(QRandomGenerator::global()->bounded(1, 101)) / static_cast<float>(100);

    fillPhi(Hit, state);
    double qHit = LCF();

    fillPhi(Stick, state);
    double qStick = LCF();

    if(k <= 1- eps){
        if(qHit >= qStick){
            return Hit;
        }else{
            return Stick;
        }
    }else if(qHit < qStick){
        return Hit;
    }else{
        return Stick;
    }
}


void TDLinearSarsa::UpdateQTDSarsa(State oldState, State state, Action oldAction, Action action, int reward){
    /// @param z - ETDLinear
    /// @param w - teta
    /// @param F(s,a) - phi
    const float ALPHA = 0.01;
    float DELTA = static_cast<double>(reward);

    fillPhi(oldAction, oldState);
    for(int i = 0; i< SIZE_PHI_TETA; i++){
        if(phi[i] == 1){
            DELTA -= teta[i];
            ETDLinear[i] += 1;
        }
    }

    if(reward == - 1 || action == Stick){ //заключительное состояние
        for(int i = 0; i < SIZE_PHI_TETA; i++){
            teta[i] += ALPHA * DELTA * ETDLinear[i];
        }
        return;
    }

    fillPhi(action, state);
    for(int i = 0; i < SIZE_PHI_TETA; i++){
        if(phi[i] == 1){
            DELTA += GAMMA * teta[i];
        }
    }

    for(int i = 0; i < SIZE_PHI_TETA; i++){
        teta[i] += ALPHA * DELTA * ETDLinear[i];
    }

    for (int keyCardDiller = 1; keyCardDiller <= 10; keyCardDiller++){
        for (int keySumAgent = 1; keySumAgent <= 21; keySumAgent++){
            State state;
            QVector<int> indexs;
            state.cardDiler = keyCardDiller;
            state.sumAgent = keySumAgent;

            indexs = getIndexByState(state, Hit);
            for(int i = 0 ; i < indexs.size(); i++){
                ETDLinear[indexs[i]] = GAMMA * LAMBDA * ETDLinear[indexs[i]];
            }

            indexs = getIndexByState(state, Stick);
            for(int i = 0 ; i < indexs.size(); i++){
                ETDLinear[indexs[i]] = GAMMA * LAMBDA * ETDLinear[indexs[i]];
            }
        }
    }
}

QVector<int> TDLinearSarsa::getIndexByState(State state, Action action){
    QVector<int> res;
    for(int i = 0; i< dealer_s.size();i++){
        for(int j = 0; j< player_s.size();j++){
            if(state.cardDiler >= dealer_s[i][0] && state.cardDiler <= dealer_s[i][1]
                    && state.sumAgent >= player_s[j][0] && state.sumAgent <= player_s[j][1]){
                if(action == Hit){
                    res.append(12 * i + 2 * j );
                }
                if(action == Stick){
                    res.append(12 * i + 2 * j + 1);
                }
            }
        }
    }
    return res;
}

StateAction TDLinearSarsa::getStateActionByIndex(int index){
    StateAction stateAction;
    for(int i = 0; i< dealer_s.size();i++){
        for(int j = 0; j< player_s.size();j++){
            if(index == (12 * i + 2 * j) || index == (12 * i + 2 * j + 1)){
                stateAction.state.cardDiler = index / 12 ;
                stateAction.state.sumAgent = index / 6;
                int act = index % 2 + 1;
                if (act == 1){stateAction.action = Hit;}
                if (act == 2){stateAction.action = Stick;}

                return stateAction;
            }//end if
        }//edn j
    }//edn i
}


double TDLinearSarsa::LCF(){
    double sum = 0;
    for(int i = 0; i < SIZE_PHI_TETA; i++){
        sum += phi[i] * teta[i];
    }
    return sum;
}

void TDLinearSarsa::fillPhi(Action action, State state){
    phi.clear();
    phi.resize(SIZE_PHI_TETA);
    for(int i = 0; i < dealer_s.size();i++){
        for(int j = 0; j < player_s.size();j++){
            if(state.cardDiler >= dealer_s[i][0] && state.cardDiler <= dealer_s[i][1]
                    && state.sumAgent >= player_s[j][0] && state.sumAgent <= player_s[j][1]){
                if(action == Hit){
                    phi[12 * i + 2 * j ] = 1;
                }
                if(action == Stick){
                    phi[12 * i + 2 * j + 1] = 1;
                }
            }
        }
    }
}
