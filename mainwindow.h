#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <cstdlib>
#include "evolution.h"
#include <QWheelEvent>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void painter();

    void gauges();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void genNameSet(std::string name);

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_checkBox_2_stateChanged(int arg1);

    void zoomIn();          // слот «приблизить»

    void zoomOut();         // слот «отдалить»

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

private:
    QString filename;
    Ui::MainWindow *ui;
    void qdraw_polygon(const polygon& pol, QGraphicsScene* scene);
    std::unique_ptr<QGraphicsScene> scene;
    std::unique_ptr<evolution> ship_evolution;
    ///тестовый
    std::unique_ptr<ship_physics> test_ship;
    QMetaObject::Connection test_update_connection;
    QMetaObject::Connection test_think_n_do_connection;
    ///тестовый
    bool eventFilter(QObject *obj, QEvent *event) override;
    std::shared_ptr<pathway> map;
    std::shared_ptr<QTimer> update_timer;
    std::shared_ptr<QTimer> painter_timer;

    /* --- список карт, текущий индекс --- */
    QStringList geo_maps;
    int         geo_idx = 0;

    /*  общая «актуальная» карта берётся либо из ship_evolution,
        либо, если эволюции ещё нет, – из поля map  */
    std::shared_ptr<pathway> current_map() const {
        return ship_evolution ? ship_evolution->map : map;
    }

    double neuron1 = 0, neuron2 = 0, neuron3 = 0, neuron4 = 0;
    bool first_boot = true,
        first_map = true,
        only_test = false,
        pre_init = false,
        tmblr_1 = false,
        tmblr_2 = false,
        tmblr_3 = false,
        tmblr_4 = false,
        tmblr_time = false,
        tmblr_slow_time = false,
        tmblr_generator = false,
        tmblr_eyes = false,
        exam_run = false;

};
#endif // MAINWINDOW_H
