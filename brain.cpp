#include "brain.h"

using namespace std;

double brain::sigmoid(const double &x){
    return 1/(1+exp(-x));
}

brain::brain(){
    S = 2 + rand() % 6;//максимум пять внутренних слоёв
    l.reserve(S);
    l.emplace_back(12);
    for (int i = 1; i < S-1; ++i){ //на каждый слой внутренний по количеству вершин
        l.emplace_back(7 + rand() % 5); //внутренний слой размером в диапазоне от 11 до 7
    }
    l.emplace_back(6);
    sort(l.begin(), l.end(), greater<int>()); //количество вершин не растет

    A.reserve(S);
    W.reserve(S-1);
    for (int i = 0; i < S-1; ++i){ //создаем веса
        A.emplace_back(l[i]);
        //A.push_back(Eigen::RowVectorXd::Zero());
        //W.emplace_back(l[i],l[i+1]);
        W.push_back(Eigen::MatrixXd::Random(l[i],l[i+1])); //на первое время и отрицательные веса тоже
    }
    A.emplace_back(l[S-1]);
}


brain::brain(const brain &a, const brain &b, double dmnc){
    this->S=a.S*dmnc+b.S*(1-dmnc)+0.5;    //+0.5 для правильного окргуления дробных чисел
                                            //тут влияние dmnc

    l.clear();
    l.reserve(S);
    for (int i = 0; i < a.S-1 && i < b.S-1; ++i){ //используем существующие размерности в обоих сетях-родителях
        this->l.emplace_back(a.l[i]*dmnc+b.l[i]*(1-dmnc)+0.5); //тут влияние dmnc
    }
    int missingLayersCnt = this->S - (a.S<b.S ? a.S : b.S); //количество недостающих слоёв в новой сети

    int temp = (a.S > b.S ? a.S : b.S); //нужен для отсчета от последнего слоя большей сети

    for (int i = missingLayersCnt+1; i > 0 && temp-i+1 < (a.S>b.S ? a.S : b.S); --i){ // добавляем недостающие слои копируя у бо'льшего родителя
        this->l.emplace_back((a.S>b.S ? a.l : b.l)[temp-i]);
        --missingLayersCnt; //уменьшаем кол-во недостающих слоев
    }


    //вариант, где случайный слой появляется на случайном месте
    {
//        int lRandDestination; //хранит позицию мутировавшего - нового слоя
//        while (--missingLayersCnt>=0){ //this ранее использовались для наглядности, тут, очевидно, будет использоватья уже созданый l
//            int temp = l.size();
//            lRandDestination = 1 + rand() % (temp - 1); //новый слой может появиться в промежутках
//            l.emplace_back(0); //временная пустышка
//            for (int i = 0; i < temp-1; ++i) {
//                if (lRandDestination>i)
//                    l[i+1]=l[i]; //смещаем после места случайного
//            }
//            l[lRandDestination] = l[lRandDestination-1]+l[lRandDestination+1]/2; //среднее значение от боковых слоёв
//        }
    }


    //вариант, где мы добавляем новое значение в конец перед выходным слоем
    while (--missingLayersCnt>=0){
        l.emplace_back(*l.end()); //равное предыдущему
    }


    l.emplace_back(6); //выходной всегда 6
    //итого у нас информация о количестве нейронов в каждом слое


}

void brain::think(){
    int i = 0;
    for (auto &w: W) {
        A[i + 1] = A[i] * w;
        for(auto &a: A[i + 1]){
            a = sigmoid(a);
        }
        ++i;
    }
    cout << "\nendl\n";
}

QDataStream &operator<<(QDataStream &out, const brain &item){
    QDataStream::FloatingPointPrecision prev = out.floatingPointPrecision();
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);
    out << item.S;
    //https://eigen.tuxfamily.org/dox/group__TutorialSTL.html
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
