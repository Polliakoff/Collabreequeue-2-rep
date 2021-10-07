#include "brain.h"

using namespace std;

double brain::sigmoid(const double &x){
    return 1/(1+exp(-x));
}

brain::brain(){
    S = 2 + rand() % 6;//максимум пять внутренних слоёв
    l.reserve(S + 2); //плюс один входной и выходной
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

void brain::think(){
    int i = 0;
    cout << A[0] * W[0] << endl;
    for (auto &w: W) {
        A[i + 1] = A[i] * w;
        for(auto &a: A[i + 1]){
            a = sigmoid(a);
        }
        ++i;
    }
    cout << "\nendl\n";
}


std::ofstream &operator<<(std::ofstream& fout, brain& b){
    //fout << endl << b.S << endl;
    for (auto &w: b.W){
        fout << w << endl;
    }
    return fout;
}

std::ifstream &operator>>(std::ifstream& fin, Eigen::MatrixXd& b){
    //fout << endl << b.S << endl;
    //for (auto &w: b.W){
        //fin >> b.W[0];
    //}
    return fin;
}

std::ifstream &operator>>(std::ifstream& fin, brain& b){
    //fout << endl << b.S << endl;
    //for (auto &w: b.W){
        fin >> b.W[0];
    //}
    return fin;
}

QDataStream &operator<<(QDataStream &out, const brain &item){
    QDataStream::FloatingPointPrecision prev = out.floatingPointPrecision();
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);
    out << item.S;
    out.setFloatingPointPrecision(prev);
    return out;
}

QDataStream &operator>>(QDataStream &in, brain &item){
    QDataStream::FloatingPointPrecision prev = in.floatingPointPrecision();
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in >> item.S;
    in.setFloatingPointPrecision(prev);
    return in;
}
