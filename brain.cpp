#include "brain.h"

using namespace std;

int brain::ID = 0;

double brain::sigmoid(const double &x){
    return 1/(1+exp(-x));
}

double brain::sigmoid_distance(const double &x){
    return 1/(1+exp(-x/2-15));
}

brain::brain(){
    S = 2 + rand() % 5;//максимум пять внутренних слоёв
    l.reserve(S);
    l.emplace_back(first);
    for (int i = 1; i < S-1; ++i){ //на каждый слой внутренний по количеству вершин
        l.emplace_back(last + 1 + rand() % (first-last)); //внутренний слой размером в диапазоне от last+1 до first-1
    }
    l.emplace_back(last);
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
//    for(auto &w: W)
//        cout << w.norm() << endl;
}


brain::brain(brain &a, brain &b, double dmnc){
    this->S=a.S*dmnc+b.S*(1-dmnc)+0.5;      //+0.5 для правильного окргуления дробных чисел
                                            //тут влияние dmnc
    this->l.clear();
    this->l.reserve(this->S);
    //vector<vector<int>> specPos;

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

    if (!viable()) return;

    inheritWeights(a,b,dmnc);

    mutate();
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
                this->W[lt](i,j)=((a.W[lt](i,j)+b.W[lt](i,j))/2.0)*(95+(rand()%11))/100.0; ////вот тут шум
                //cout << i << "\t" << j << "\n"; //11,6 проверка
            }
        }

        //восстанавливаем потерянные столбцы
        {
            auto &abMax = (a.l[lt+1] >= b.l[lt+1]? a : b); //сравниваем по количеству столбцов
            auto &abMin = (a.l[lt+1] <  b.l[lt+1]? a : b);
            for (int temp = abMin.l[lt+1]; //берем за начальную точку конец меньшей матрицы
                 temp < this->l[lt+1]; ++temp){ //пока точка не вышла за пределы текущей матрицы
                for (int i = 0; i < abMax.l[lt] && i < l[lt]; ++i){
                    this->W[lt](i,temp) = abMax.W[lt](i,temp)*(95+(rand()%11))/100.0; ////вот тут шум
                                                                                    //берем веса большей матрицы
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
                    this->W[lt](temp,i) = abMax.W[lt](temp,i)*(95+(rand()%11))/100.0; ////вот тут шум
                    //берем веса большей матрицы
                }
            }
            for(int temp = abMin.l[lt]; //разница между максимумом
                temp < this->l[lt]; ++temp)
                for (int i = 0; i < abMax.l[lt+1] && i<l[lt+1]; ++i){
                    this->W[lt](temp,i) = abMax.W[lt](temp,i)*(95+(rand()%11))/100.0; ////вот тут шум
                    //берем веса большей матрицы
                }
        }

        ++lt;
    }

    //наследование матриц только от одного родителя без стекл
    auto &abMax = (a.S > b.S? a : b); //ищем большего родителя
    ///bool first_special_layer = true;
    for (; lt < abMax.S-2 && lt != this->S-2; ){ //все еще не берём синее стекло
        this->A.emplace_back(l[lt]);
        this->W.emplace_back(l[lt],l[lt+1]);
        for (int j = 0; j < abMax.l[lt+1] && j < l[lt+1]; ++j){
            for (int i = 0; i < abMax.l[lt] && i < l[lt]; ++i){
                this->W[lt](i,j) = abMax.W[lt](i,j)*(95+(rand()%11))/100.0; ////вот тут шум
                //просто берём
            }
        }

        //в начале можно потерять одну строку ?
        {       //восстанавливаем потерянные строки
//        if (first_special_layer){
//            auto &abMax = (a.l[lt-1] >= b.l[lt-1]? a : b); //сравниваем по количеству строк
//            auto &abMin = (a.l[lt-1] <  b.l[lt-1]? a : b);
//            for (int temp = abMin.l[lt-1]; //берем за начальную точку конец меньшей матрицы
//                 temp < this->l[lt-1]; ++temp){ //пока точка не вышла за пределы текущей матрицы
//                for (int i = 0; i < abMax.l[lt]; ++i){
//                    this->W[lt](temp,i) = abMax.W[lt-1](temp,i); //берем веса большей матрицы
//                }
//            }
//            first_special_layer=false;
//        }
        }
        ++lt;
    }

    ////b.l[2]=6; экстремальная проверка
    //наследование пересекающихся матриц синие стекла
    this->A.emplace_back(l[lt]);
    this->W.emplace_back(l[lt],l[lt+1]);
    this->A.emplace_back(l[lt+1]);

    for (int j = 0; j < a.l[a.S-1] && j < b.l[b.S-1]; ++j){
        for (int i = 0; i < a.l[a.S-2] && i < b.l[b.S-2]; ++i){
            this->W[S-2](i,j)=((a.W[a.S-2](i,j)+b.W[b.S-2](i,j))/2.0)*(95+(rand()%11))/100.0; ////вот тут шум
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
                this->W[S-2](temp,i) = abMax.W[abMax.S-2](temp,i)*(95+(rand()%11))/100.0; ////вот тут шум;
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

void brain::mutate(){
    ofstream fout("mutate" + std::to_string(++ID) + ".log");
    string name;
    name = '_'+std::to_string(S);
    name.push_back('s');
    for (auto &i: l){
        name += std::to_string(i);
        name.push_back('.');
    }

    int mutatedLayers = int(1 == rand()%20)*(rand()%2? 1 : -1); //в одном из 20-ти происходит мутация слоев на один(не больше)
    if (S == 2 && mutatedLayers < 0 ) mutatedLayers = 0;
    fout << name << "\n" << std::to_string(ID) + "\t S -> " + std::to_string(mutatedLayers) + "\n";
    S+=mutatedLayers;

    //мутирует количество слоёв
    l.pop_back(); //убрали последний
    A.pop_back(); //убрали последний
    auto w = W[W.size()-1];//запомнили веса последней матрицы
    if (mutatedLayers>0) {
        W.pop_back();
        l.emplace_back(l[l.size()-1]); //количество нейронов равное предыдущему слою
        A.emplace_back(l[l.size()-1]); //создали новый
        W.push_back(Eigen::MatrixXd::Random(l[l.size()-1],l[l.size()-1]));
        W.push_back(w);
    } else if(mutatedLayers<0) {
        W.pop_back();
        l.pop_back();           //убили предпоследний
        A.pop_back();           //убили предпоследний
        W.pop_back();           //убили предпоследний
        W.push_back(Eigen::MatrixXd::Random(l[l.size()-1],last));
        for(int c = 0; c < last && c < w.cols(); ++c)
            for (int r = 0; r < l[S-2] && r < w.rows(); ++r){
                W[S-2](r,c) = w(r,c);
            }
    }
    l.emplace_back(last);
    A.emplace_back(last);


    //мутирует кол-во нейронов в слоях
    vector<int> vec(S);
    fout << ID << "\t l -> ";
    for (auto &v: vec){
        v = int(1 == rand()%6)*(rand()%2? 1 : 1); //в одном из десяти мутация на один
    }

    vec[0]=0;
    vec[S-1]=0;
    for (auto &v: vec){
        fout << v << "\t";
    }
    fout << "\n\n";
    int i = 0;

    for (auto v = vec.begin(); v+1!=vec.end(); ++v){
        if (*v < 0) {
            int r = W[i-1].rows();
            int c = W[i-1].cols();
            int tempnum = rand() % c;
            for(int tmp = tempnum; tmp < c; ++tmp){ //пузырьком всплывает столбец весов
                W[i-1].col(tmp-1)=W[i-1].col(tmp)+W[i-1].col(tmp-1);
                W[i-1].col(tmp)=W[i-1].col(tmp-1)-W[i-1].col(tmp);
            }
            W[i-1].conservativeResize(r, *v+c); //отрубили последний столбец
            c = W[i].cols();
            r = W[i].rows();
            for(int tmp = tempnum; tmp < r; ++tmp){ //пузырьком всплывает строка весов
                W[i].row(tmp-1)=W[i].row(tmp)+W[i].row(tmp-1);
                W[i].row(tmp)=W[i].row(tmp-1)-W[i].row(tmp);
            }
            W[i].conservativeResize(r+*v, c); //отрубили последнюю строку
        }
        if (*v > 0){
            int r = W[i-1].rows();
            int c = W[i-1].cols();
            W[i-1].conservativeResize(r, *v+c); //добавили столбец в конец
            //for (int n = c; n < W[i-1].cols(); ++n){
            for (int tr = 0; tr < r; ++tr) {
                W[i-1](tr,c)=(-100+rand()%201)/100.0; //заполнили
            }
            r = W[i].rows();
            c = W[i].cols();
            W[i].conservativeResize(r+*v, c);
            for (int tc = 0; tc < c; ++tc) {
                W[i](r,tc)=(-100+rand()%201)/100.0; //заполнили
            }

        }

        l[i]+=*v;
        A[i].resize(A[i].cols()+*v);
        ++i;
    }

}




void brain::think(){
    int i = 0;
    for (int k = 0; k<6; ++k){
        A[0](k)=sigmoid_distance(A[0](k));
    }
    for (auto &w: W) {
        A[i + 1] = A[i] * w;
        for(auto &a: A[i + 1]){
            a = sigmoid(a);
        }
        ++i;
    }
    i = 0;
    for (auto &a: A[S-1]) {
        A[0](first-last+i)=a;
        ++i;
    }
}

bool brain::viable(){
    for (vector<int>::iterator it = l.begin(); it + 1 != l.end();) {
        if(*it < *(it+1)) return false;
        ++it;
    }
    return true;
}








//========================================================================


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
