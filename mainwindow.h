#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void digit_pressed();

    void unary_operation_pressed();

    void on_pushButton_Decimale_released();

    void on_pushButton_Clear_released();

    void on_pushButton_Equal_released();

    void binary_operation_pressed();

    double evaluateExpression(const QString &exp);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
