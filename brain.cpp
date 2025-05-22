#include "brain.h"

using namespace std;
static thread_local std::mt19937 RNG{ std::random_device{}() };
static thread_local std::uniform_real_distribution<double> UNI01(-1.0, 1.0);
static thread_local std::uniform_real_distribution<double> NOISE(-0.1, 0.1);

int brain::ID = 0;

double brain::sigmoid(const double &x){
    const double t = std::clamp(x, -60.0, 60.0); // защита от overflow
    return 1.0 / (1.0 + std::exp(-t));
}

brain::brain()
{
    S = 5;

    l.reserve(S);
    l = { first, 6, 5, 4, last };

    A.reserve(S);
    W.reserve(S-1);
    for (int i = 0; i < S-1; ++i){ //создаем веса
        A.emplace_back(Eigen::RowVectorXd::Zero(l[i]));
        W.push_back(Eigen::MatrixXd::Random(l[i],l[i+1])); //на первое время и отрицательные веса тоже
        E.emplace_back(W.back().rows(), W.back().cols());
        E.back().setOnes();            // все связи активны изначально
    }
    A.emplace_back(Eigen::RowVectorXd::Zero(l[S-1]));
    ++ID;
}

brain::brain(brain &a, brain &b, double dmnc)
{
    ++ID;
    this->S=a.S*dmnc+b.S*(1-dmnc)+0.5;      //+0.5 для правильного окргуления дробных чисел
    //тут влияние dmnc
    this->l.clear();
    this->l.reserve(this->S);

    for (int i = 0; i < a.S-1 && i < b.S-1; ++i){ //используем существующие размерности в обеих сетях-родителях
        this->l.emplace_back(a.l[i]*dmnc+b.l[i]*(1-dmnc)+0.5); //тут влияние dmnc
    }
    int missingLayersCnt = this->S - (a.S<b.S ? a.S : b.S); //количество недостающих слоёв в новой сети

    int temp = (a.S < b.S ? a.S : b.S); //нужен для отсчета от последнего слоя меньшей сети

    for (temp=temp-1; temp+1 < (a.S>b.S ? a.S : b.S) && missingLayersCnt>0; ++temp){ // добавляем недостающие слои копируя у бо'льшего родителя
        this->l.emplace_back((a.S>b.S ? a.l : b.l)[temp]);
        --missingLayersCnt; //уменьшаем кол-во недостающих слоев
    }

    this->l.emplace_back(last); //выходной всегда last
    //итого у нас информация о количестве нейронов в каждом слое

    inheritWeights(a,b,dmnc);
    noiseWeights();
}

void brain::inheritWeights(brain &a, brain &b, double dmnc){
    this->A.clear();
    this->A.reserve(this->S);

    this->W.clear();
    this->W.reserve(this->S-1);

    this->E.clear();
    this->E.reserve(this->S-1);

    auto lt = 0;
    //наследование пересекающихся матриц слева-направо зеленые + красные стекла
    for (; lt < a.S-2 && lt < b.S-2 && lt != this->S-2;){ //не берем последний слой (синее стекло)
        this->A.emplace_back(Eigen::RowVectorXd::Zero(l[lt]));
        this->W.emplace_back(l[lt],l[lt+1]);
        // 1) Заводим новую маску точно такого же размера:
        this->E.emplace_back(l[lt], l[lt+1]);
        // 2) Для начала обнуляем (будем включать только те связи, что были в одном из родителей)
        this->E.back().setZero();
        for (int j = 0; j < a.l[lt+1] && j < b.l[lt+1] && j < l[lt+1]; ++j){
            for (int i = 0; i < a.l[lt] && i < b.l[lt] && i < l[lt]; ++i){
                this->W[lt](i,j) = (a.W[lt](i,j)+b.W[lt](i,j))/2.0;
                this->E[lt](i,j) = a.E[lt](i,j) || b.E[lt](i,j);
            }
        }

        //восстанавливаем потерянные столбцы
        {
            auto &abMax = (a.l[lt+1] >= b.l[lt+1]? a : b); //сравниваем по количеству столбцов
            auto &abMin = (a.l[lt+1] <  b.l[lt+1]? a : b);
            for (int temp = abMin.l[lt+1];
                 temp < this->l[lt+1]; ++temp){
                for (int i = 0; i < abMax.l[lt] && i < l[lt]; ++i){
                    this->W[lt](i, temp) = abMax.W[lt](i, temp);
                    this->E[lt](i, temp) = abMax.E[lt](i, temp);
                }
            }
        }

        //восстанавливаем потерянные строки
        {
            auto &abMax = (a.l[lt] >= b.l[lt]? a : b); //сравниваем по количеству строк
            auto &abMin = (a.l[lt] <  b.l[lt]? a : b);
            for (int temp = abMin.l[lt];
                 temp < this->l[lt]; ++temp){
                for (int i = 0; i < abMax.l[lt+1] && i < l[lt+1]; ++i){
                    this->W[lt](temp, i) = abMax.W[lt](temp, i);
                    this->E[lt](temp, i) = abMax.E[lt](temp, i);
                }
            }
        }
        ++lt;
    }

    //наследование матриц только от одного родителя без стекл
    auto &abMax = (a.S > b.S? a : b); //ищем большего родителя
    for (; lt < abMax.S-2 && lt != this->S-2; ){
        this->A.emplace_back(Eigen::RowVectorXd::Zero(l[lt]));
        this->W.emplace_back(l[lt],l[lt+1]);
        this->E.emplace_back(l[lt], l[lt+1]);
        this->E.back().setZero();

        for (int j = 0; j < abMax.l[lt+1] && j < l[lt+1]; ++j){
            for (int i = 0; i < abMax.l[lt] && i < l[lt]; ++i){
                this->W[lt](i,j) = abMax.W[lt](i,j);
                this->E[lt](i,j) = abMax.E[lt](i,j);
            }
        }
        ++lt;
    }

    //наследование пересекающихся матриц синие стекла
    this->A.emplace_back(Eigen::RowVectorXd::Zero(l[lt]));
    this->W.emplace_back(l[lt],l[lt+1]);
    // 1) Заводим новую маску точно такого же размера:
    this->E.emplace_back(l[lt], l[lt+1]);
    // 2) Для начала обнуляем (будем включать только те связи, что были в одном из родителей)
    this->E.back().setZero();
    this->A.emplace_back(Eigen::RowVectorXd::Zero(l[lt+1]));

    for (int j = 0; j < a.l[a.S-1] && j < b.l[b.S-1]; ++j){
        for (int i = 0; i < a.l[a.S-2] && i < b.l[b.S-2]; ++i){
            this->W[S-2](i,j) = (a.W[a.S-2](i,j)+b.W[b.S-2](i,j))/2.0;
            this->E[S-2](i,j) = a.E[a.S-2](i,j) || b.E[b.S-2](i,j);
        }
    }

    //потерянных столбоцов быть не может, так как выходных нейронов всегда last
    //восстанавливаем потерянные строки
    {
        auto &abMax = (a.l[a.S-2] >= b.l[b.S-2]? a : b); //сравниваем по количеству строк
        auto &abMin = (a.l[a.S-2] <  b.l[b.S-2]? a : b);
        for (int temp = abMin.l[abMin.S-2]; //берем за начальную точку конец меньшей матрицы
             temp < this->l[S-2] && temp < abMax.l[abMax.S-2]; ++temp){ //пока точка не вышла за пределы текущей матрицы
            for (int i = 0; i < abMax.l[abMax.S-1]; ++i){
                this->W[S-2](temp,i) = abMax.W[abMax.S-2](temp,i);
                this->E[S-2](temp,i) = abMax.E[abMax.S-2](temp,i);
                //берем веса большей матрицы
            }
        }
    }
}

double brain::round_to_one(const double &subject)
{
    if(abs(subject)>1){
        return subject/subject;
    }
    else{
        return subject;
    }
}

void brain::noiseWeights()
{
    for (std::size_t lidx = 0; lidx < W.size(); ++lidx) {
        auto & wMat = W[lidx];
        auto & mMat = E[lidx];
        for (Eigen::Index k = 0; k < wMat.size(); ++k) {
            if (mMat(k)) {
                double & ref = *(wMat.data() + k);
                ref += NOISE(RNG);
                ref = std::clamp(ref, -1.0, 1.0);
            }
        }
    }
}

void brain::think(){
    int i = 0;
    for (int k = 0; k < A[0].size(); ++k) {
        A[0](k) /= 10.0;
    }

    for (auto &w: W) {
        A[i + 1] = A[i] *
                   ((w.array() * E[i].cast<double>())   // обнуляем вырезанные связи
                        .matrix());
        for(auto &a: A[i + 1]){
            a = sigmoid(a);
        }
        ++i;
    }
    i = 0;
}

void brain::toggleConnection(std::size_t layer,
                             std::size_t src, std::size_t dst, bool en)
{
    if (layer >= E.size()) return;
    if (src   >= E[layer].rows()) return;
    if (dst   >= E[layer].cols()) return;
    E[layer](src, dst) = en ? 1 : 0;
}

void brain::addRandomConnection()
{
    if (S < 2) return;
    std::uniform_int_distribution<std::size_t> lay(0, S - 2);
    std::size_t layer = lay(RNG);
    std::uniform_int_distribution<std::size_t> rsrc(0, l[layer] - 1);
    std::uniform_int_distribution<std::size_t> rdst(0, l[layer + 1] - 1);
    std::size_t src = rsrc(RNG);
    std::size_t dst = rdst(RNG);

    if (!E[layer](src, dst)) {                     // было отключено
        E[layer](src, dst) = 1;
        W[layer](src, dst) = UNI01(RNG); // [-1;1]
    }
}

void brain::splitConnection(std::size_t layer,
                            std::size_t src, std::size_t dst)
{
    /*  add-node: «расколоть» существующую связь, добавив НОВЫЙ нейрон
        в (layer+1).  Старая связь → off; две новые связи:
            src -> newNeuron (вес = 1.0)
            newNeuron -> dst  (вес = oldWeight)
    */
    if (layer >= W.size()) return;
    if (!E[layer](src, dst)) return;               // связь должна быть активна

    double oldW = W[layer](src, dst);
    E[layer](src, dst) = 0;                        // отключаем

    /* 1. добавляем столбец в W[layer] (+маску) – новый нейрон */
    std::size_t newNeuron = l[layer + 1];          // индекс нового нейрона
    ++l[layer + 1];

    W[layer].conservativeResize(Eigen::NoChange, l[layer + 1]);
    E[layer].conservativeResize(Eigen::NoChange, l[layer + 1]);
    W[layer].col(newNeuron).setZero();
    E[layer].col(newNeuron).setZero();
    W[layer](src, newNeuron) = 1.0;
    E[layer](src, newNeuron) = 1;

    /* 2. добавляем строку в W[layer+1] (+маску) и
          копируем выходы старого dst:  newNeuron наследует их,
          а у dst все исходящие обнуляем  */
    if (layer + 1 < W.size()) {
        W[layer + 1].conservativeResize(l[layer + 1], Eigen::NoChange);
        E[layer + 1].conservativeResize(l[layer + 1], Eigen::NoChange);

        W[layer + 1].row(newNeuron) = W[layer + 1].row(dst) * oldW;
        E[layer + 1].row(newNeuron) = E[layer + 1].row(dst);

        W[layer + 1].row(dst).setZero();
        E[layer + 1].row(dst).setZero();
    }

    /* 3. активируем буфер A – увеличиваем размер слоя (для корректности) */
    A[layer + 1].conservativeResize(l[layer + 1]);
    A[layer + 1](newNeuron) = 0.0;
}

void brain::disableRandomConnection()
{
    /*  Выбираем случайную ВКЛЮЧЁННУЮ связь и выключаем её.
    Если таковых нет – тихо выходим.                        */
    std::vector<std::tuple<std::size_t,std::size_t,std::size_t>> pool;
    for (std::size_t lidx = 0; lidx < E.size(); ++lidx)
        for (std::size_t i = 0; i < (std::size_t)E[lidx].rows(); ++i)
            for (std::size_t j = 0; j < (std::size_t)E[lidx].cols(); ++j)
                if (E[lidx](i,j))
                    pool.emplace_back(lidx,i,j);

    if (pool.empty()) return;

    std::uniform_int_distribution<std::size_t> pick(0, pool.size()-1);
    auto [l,s,d] = pool[pick(RNG)];
    E[l](s,d) = 0;
}

void brain::pruneIsolatedNeurons()
{
    /*  Удаляем нейроны, у которых НЕТ активных входов и выходов.
    Работаем только со скрытыми слоями: 1 … S-2.            */
    for (std::size_t layer = 1; layer + 1 < (std::size_t)S; ++layer) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (std::size_t n = 0; n < (std::size_t)l[layer]; ++n) {
                bool hasIn  = E[layer-1].col(n).any();
                bool hasOut = E[layer  ].row(n).any();
                if (!hasIn && !hasOut) {
                    /* -- удаляем столбец/строку n -- */
                    // входящие
                    W[layer-1].block(0,n, W[layer-1].rows(), l[layer]-n-1) =
                        W[layer-1].block(0,n+1, W[layer-1].rows(), l[layer]-n-1);
                    E[layer-1].block(0,n, E[layer-1].rows(), l[layer]-n-1) =
                        E[layer-1].block(0,n+1, E[layer-1].rows(), l[layer]-n-1);
                    W[layer-1].conservativeResize(Eigen::NoChange, l[layer]-1);
                    E[layer-1].conservativeResize(Eigen::NoChange, l[layer]-1);

                    // исходящие
                    W[layer].block(n,0, l[layer]-n-1, W[layer].cols()) =
                        W[layer].block(n+1,0, l[layer]-n-1, W[layer].cols());
                    E[layer].block(n,0, l[layer]-n-1, E[layer].cols()) =
                        E[layer].block(n+1,0, l[layer]-n-1, E[layer].cols());
                    W[layer].conservativeResize(l[layer]-1, Eigen::NoChange);
                    E[layer].conservativeResize(l[layer]-1, Eigen::NoChange);

                    // значения нейрона
                    Eigen::RowVectorXd tmp(l[layer]-1);
                    tmp.head(n)                = A[layer].head(n);
                    tmp.tail(l[layer]-n-1)     = A[layer].tail(l[layer]-n-1);
                    A[layer] = std::move(tmp);

                    --l[layer];
                    changed = true;
                    break;  // перезапускаем скан после изменения индексов
                }
            }
        }
    }
}

void brain::insertHiddenLayer(std::size_t pos, int neurons)
{
    /*  pos – индекс ПЕРЕД КОТОРЫМ вставляется слой
    допустимые pos: 0 … S-2 (т.е. между входом и выходом)   */
    if (pos >= (std::size_t)S-1) return;
    if (neurons <= 0) return;

    /* --- 1. обновляем счётчики --- */
    l.insert(l.begin()+pos+1, neurons);
    A.insert(A.begin()+pos+1, Eigen::RowVectorXd::Zero(neurons));
    ++S;

    /* --- 2. разбиваем старую матрицу W[pos] --- */

    Eigen::MatrixXd w1 = Eigen::MatrixXd::Random(l[pos],   neurons);
    Eigen::MatrixXd w2 = Eigen::MatrixXd::Random(neurons,  l[pos+2]);

    Mask m1(l[pos], neurons);   m1.setOnes();
    Mask m2(neurons, l[pos+2]); m2.setOnes();

    W[pos] = std::move(w1);   E[pos] = std::move(m1);
    W.insert(W.begin()+pos+1, std::move(w2));
    E.insert(E.begin()+pos+1, std::move(m2));
}

void brain::removeHiddenLayer(std::size_t pos)
{
    /*  Удаляем скрытый слой pos (1 … S-2)                     */
    if (pos == 0 || pos + 1 >= (std::size_t)S) return; // вход/выход трогать нельзя

    /* --- 1. формируем «сквозную» матрицу --- */
    Eigen::MatrixXd newW = W[pos-1] * W[pos];
    Mask newM(W[pos-1].rows(), W[pos].cols());
    newM.setZero();
    for (Eigen::Index i = 0; i < newM.rows(); ++i)
        for (Eigen::Index j = 0; j < newM.cols(); ++j)
            for (Eigen::Index k = 0; k < E[pos-1].cols(); ++k)
                if (E[pos-1](i,k) && E[pos](k,j)) { newM(i,j)=1; break; }

    /* --- 2. удаляем слой и его структуры --- */
    W[pos-1] = std::move(newW);
    E[pos-1] = std::move(newM);

    W.erase(W.begin()+pos);
    E.erase(E.begin()+pos);
    A.erase(A.begin()+pos);
    l.erase(l.begin()+pos);
    --S;
}

//========================================================================
QDataStream &operator<<(QDataStream &out, const brain &item){
    QDataStream::FloatingPointPrecision prev = out.floatingPointPrecision();
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);
    out << item.S;
    for (auto &l_: item.l)
        out << l_;
    for (auto &w: item.W){
        for(auto &r : w.reshaped())
            out << r;
    }
    for (auto & m : item.E)
        for (char c : m.reshaped())
            out << c;
    out.setFloatingPointPrecision(prev);
    return out;
}

QDataStream &operator>>(QDataStream &in, brain &item){
    QDataStream::FloatingPointPrecision prev = in.floatingPointPrecision();
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in >> item.S;
    item.W.clear();
    item.A.clear();
    item.E.clear();
    item.E.reserve(item.S - 1);
    item.l.clear();
    item.l.reserve(item.S);
    for (int k = 0; k < item.S; ++k){
        int temp;
        in >> temp;
        item.l.emplace_back(temp);
    }
    item.W.reserve(item.S-1);
    item.A.reserve(item.S);
    for (int k = 0; k < item.S - 1; ++k){
        item.A.emplace_back(Eigen::RowVectorXd::Zero(item.l[k]));
        item.W.push_back(Eigen::MatrixXd::Zero(item.l[k],item.l[k+1]));
        for(auto &r : item.W[k].reshaped())
            in >> r;
    }
    item.A.emplace_back(Eigen::RowVectorXd::Zero(item.l[item.S-1]));
    item.E.reserve(item.S - 1);
    for (int k = 0; k < item.S - 1; ++k) {
        item.E.emplace_back(item.l[k], item.l[k + 1]);
        for (char & c : item.E[k].reshaped())
            in >> c;
    }
    in.setFloatingPointPrecision(prev);
    return in;
}
