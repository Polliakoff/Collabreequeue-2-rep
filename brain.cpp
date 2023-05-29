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

    auto lt = 0;
    //наследование пересекающихся матриц слева-направо зеленые + красные стекла
    for (; lt < a.S-2 && lt < b.S-2 && lt != this->S-2;){ //не берем последний слой (синее стекло)
        this->A.emplace_back(l[lt]);
        this->W.emplace_back(l[lt],l[lt+1]);
        for (int j = 0; j < a.l[lt+1] && j < b.l[lt+1] && j < l[lt+1]; ++j){
            for (int i = 0; i < a.l[lt] && i < b.l[lt] && i < l[lt]; ++i){
                this->W[lt](i,j) = (a.W[lt](i,j)+b.W[lt](i,j))/2.0;
            }
        }

        //восстанавливаем потерянные столбцы
        {
            auto &abMax = (a.l[lt+1] >= b.l[lt+1]? a : b); //сравниваем по количеству столбцов
            auto &abMin = (a.l[lt+1] <  b.l[lt+1]? a : b);
            for (int temp = abMin.l[lt+1]; //берем за начальную точку конец меньшей матрицы
                 temp < this->l[lt+1]; ++temp){ //пока точка не вышла за пределы текущей матрицы
                for (int i = 0; i < abMax.l[lt] && i < l[lt]; ++i){
                    this->W[lt](i,temp) = abMax.W[lt](i,temp);//берем веса большей матрицы
                }
            }
        }

        //восстанавливаем потерянные строки
        {
            auto &abMax = (a.l[lt] >= b.l[lt]? a : b); //сравниваем по количеству строк
            auto &abMin = (a.l[lt] <  b.l[lt]? a : b);
            for (int temp = abMin.l[lt]; //берем за начальную точку конец меньшей матрицы
                 temp < this->l[lt]; ++temp){ //пока точка не вышла за пределы текущей матрицы
                for (int i = 0; i < abMax.l[lt+1] && i<l[lt+1]; ++i){
                    this->W[lt](temp,i) = abMax.W[lt](temp,i);
                    //берем веса большей матрицы
                }
            }
            for(int temp = abMin.l[lt]; //разница между максимумом
                temp < this->l[lt]; ++temp)
                for (int i = 0; i < abMax.l[lt+1] && i<l[lt+1]; ++i){
                    this->W[lt](temp,i) = abMax.W[lt](temp,i);
                    //берем веса большей матрицы
                }
        }
        ++lt;
    }

    //наследование матриц только от одного родителя без стекл
    auto &abMax = (a.S > b.S? a : b); //ищем большего родителя
    for (; lt < abMax.S-2 && lt != this->S-2; ){ //все еще не берём синее стекло
        this->A.emplace_back(l[lt]);
        this->W.emplace_back(l[lt],l[lt+1]);
        for (int j = 0; j < abMax.l[lt+1] && j < l[lt+1]; ++j){
            for (int i = 0; i < abMax.l[lt] && i < l[lt]; ++i){
                this->W[lt](i,j) = abMax.W[lt](i,j);
                //просто берём
            }
        }
        ++lt;
    }

    //наследование пересекающихся матриц синие стекла
    this->A.emplace_back(l[lt]);
    this->W.emplace_back(l[lt],l[lt+1]);
    this->A.emplace_back(l[lt+1]);

    for (int j = 0; j < a.l[a.S-1] && j < b.l[b.S-1]; ++j){
        for (int i = 0; i < a.l[a.S-2] && i < b.l[b.S-2]; ++i){
            this->W[S-2](i,j) = (a.W[a.S-2](i,j)+b.W[b.S-2](i,j))/2.0;
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
                //берем веса большей матрицы
            }
        }
        //ебнутый вариант
        for (int temp = abMax.l[abMax.S-2]; //берем за начальную точку конец БОЛЬШЕЙ матрицы
             temp < this->l[S-2]; ++temp){ //пока точка не вышла за пределы текущей матрицы
            for (int i = 0; i < abMax.l[abMax.S-1]; ++i){
                this->W[S-2](temp,i) = (-100+rand()%201)/100.0;
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
    for (auto &w: W){
        for (auto &i: w.reshaped()){
            i += (rand()%201 - 100)/1000.0;
            if(i>1){
                i=1;
            }
            if(i<-1){
                i=-1;
            }

        }
    }
}

void brain::think(){
    int i = 0;
    for (int k = 0; k<6; ++k){
        A[0](k) /= 10.0;
    }

    for (auto &w: W) {
        A[i + 1] = A[i] * w;
        for(auto &a: A[i + 1]){
            a = sigmoid(a);
        }
        ++i;
    }
    i = 0;
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
    out.setFloatingPointPrecision(prev);
    return out;
}

QDataStream &operator>>(QDataStream &in, brain &item){
    QDataStream::FloatingPointPrecision prev = in.floatingPointPrecision();
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in >> item.S;
    item.W.clear();
    item.A.clear();
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
    in.setFloatingPointPrecision(prev);
    return in;
}
