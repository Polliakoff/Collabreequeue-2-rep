#include "brain.h"

using namespace std;

int brain::ID = 0;

double brain::sigmoid(const double &x){
    return 1/(1+exp(-x));
}

brain::brain()
{
    S = 5;

    l.reserve(S);
    l.emplace_back(first);

    l.emplace_back(6);
    l.emplace_back(5);
    l.emplace_back(4);

    l.emplace_back(last);
    sort(l.begin(), l.end(), greater<int>()); //количество вершин не растет

    A.reserve(S);
    W.reserve(S-1);
    for (int i = 0; i < S-1; ++i){ //создаем веса
        A.emplace_back(l[i]);
        W.push_back(Eigen::MatrixXd::Random(l[i],l[i+1])); //на первое время и отрицательные веса тоже
        E.emplace_back(W.back().rows(), W.back().cols());
        E.back().setOnes();            // все связи активны изначально
    }
    A.emplace_back(l[S-1]);
}

brain::brain(brain &a, brain &b, double dmnc)
{
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
        this->A.emplace_back(l[lt]);
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
        this->A.emplace_back(l[lt]);
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
    this->A.emplace_back(l[lt]);
    this->W.emplace_back(l[lt],l[lt+1]);
    // 1) Заводим новую маску точно такого же размера:
    this->E.emplace_back(l[lt], l[lt+1]);
    // 2) Для начала обнуляем (будем включать только те связи, что были в одном из родителей)
    this->E.back().setZero();
    this->A.emplace_back(l[lt+1]);

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
                ref += (rand() % 201 - 100) / 1000.0;
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
    std::size_t layer = rand() % (S - 1);          // только соседние слои
    std::size_t src   = rand() % l[layer];
    std::size_t dst   = rand() % l[layer + 1];

    if (!E[layer](src, dst)) {                     // было отключено
        E[layer](src, dst) = 1;
        W[layer](src, dst) = (rand() % 2001 - 1000) / 1000.0; // [-1;1]
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

    /* 2. добавляем строку в W[layer+1] (+маску) */
    if (layer + 1 < W.size()) {
        W[layer + 1].conservativeResize(l[layer + 1], Eigen::NoChange);
        E[layer + 1].conservativeResize(l[layer + 1], Eigen::NoChange);
        W[layer + 1].row(newNeuron).setZero();
        E[layer + 1].row(newNeuron).setZero();
        W[layer + 1](newNeuron, dst) = oldW;
        E[layer + 1](newNeuron, dst) = 1;
    }

    /* 3. активируем буфер A – увеличиваем размер слоя (для корректности) */
    A[layer + 1].conservativeResize(l[layer + 1]);
    A[layer + 1](newNeuron) = 0.0;
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
        item.A.emplace_back(item.l[k]);
        item.W.push_back(Eigen::MatrixXd::Zero(item.l[k],item.l[k+1]));
        for(auto &r : item.W[k].reshaped())
            in >> r;
    }
    item.A.emplace_back(item.l[item.S-1]);
    item.E.reserve(item.S - 1);
    for (int k = 0; k < item.S - 1; ++k) {
        item.E.emplace_back(item.l[k], item.l[k + 1]);
        for (char & c : item.E[k].reshaped())
            in >> c;
    }
    in.setFloatingPointPrecision(prev);
    return in;
}
