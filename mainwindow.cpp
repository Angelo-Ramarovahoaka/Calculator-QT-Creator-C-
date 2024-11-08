#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <stack>
#include <QString>
#include <QRegularExpression>
#include <cmath>

QString expression; // Stocke l'expression complète

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connexions pour les boutons de chiffres
    connect(ui->pushButton_1, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_3, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_4, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_5, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_6, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_7, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_8, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_9, &QPushButton::released, this, &MainWindow::digit_pressed);
    connect(ui->pushButton_0, &QPushButton::released, this, &MainWindow::digit_pressed);

    // Connexions pour les opérations unaires
    connect(ui->pushButton_PlusMinus, &QPushButton::released, this, &MainWindow::unary_operation_pressed);
    connect(ui->pushButton_Percent, &QPushButton::released, this, &MainWindow::unary_operation_pressed);

    // Connexions pour les opérations binaires
    connect(ui->pushButton_Add, &QPushButton::released, this, &MainWindow::binary_operation_pressed);
    connect(ui->pushButton_Minus, &QPushButton::released, this, &MainWindow::binary_operation_pressed);
    connect(ui->pushButton_Divide, &QPushButton::released, this, &MainWindow::binary_operation_pressed);
    connect(ui->pushButton_Multiple, &QPushButton::released, this, &MainWindow::binary_operation_pressed);

    // Configuration des boutons binaires pour qu'ils soient décochés au départ
    ui->pushButton_Add->setCheckable(true);
    ui->pushButton_Minus->setCheckable(true);
    ui->pushButton_Divide->setCheckable(true);
    ui->pushButton_Multiple->setCheckable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::digit_pressed()
{
    QPushButton *button = (QPushButton*) sender();
    // Ajouter le chiffre pressé à l'expression
    expression += button->text();
    ui->label->setText(expression); // Mettre à jour l'affichage
}

void MainWindow::on_pushButton_Decimale_released()
{
    // Ajout du point décimal s'il n'est pas déjà présent dans la dernière partie de l'expression
    if (!expression.endsWith(".") && !expression.contains('.'))
    {
        expression += ".";
        ui->label->setText(expression);
    }
}

void MainWindow::unary_operation_pressed()
{
    QPushButton *button = (QPushButton *) sender();
    double labelNumber = (ui->label->text()).toDouble();
    QString newLabel;

    if (button->text() == "+/-") {
        labelNumber *= -1;
        newLabel = QString::number(labelNumber, 'g', 15);
        expression = newLabel; // Met à jour l'expression avec le signe modifié
        ui->label->setText(expression);
    }

    if (button->text() == "%") {
        labelNumber *= 0.01;
        newLabel = QString::number(labelNumber, 'g', 15);
        expression = newLabel; // Met à jour l'expression avec le pourcentage
        ui->label->setText(expression);
    }
}

void MainWindow::on_pushButton_Clear_released()
{
    // Réinitialiser l'expression et l'affichage
    expression.clear();
    ui->label->setText("0");

    // Décocher tous les boutons binaires
    ui->pushButton_Add->setChecked(false);
    ui->pushButton_Minus->setChecked(false);
    ui->pushButton_Divide->setChecked(false);
    ui->pushButton_Multiple->setChecked(false);
}

void MainWindow::on_pushButton_Equal_released()
{
    double result;
    // Convertit l'expression en un calcul si possible
    try {
        result = evaluateExpression(expression);
    } catch (...) {
        ui->label->setText("Erreur");
        return;
    }
    // Mettre à jour l'affichage avec le résultat
    ui->label->setText(QString::number(result, 'g', 15));
    expression = QString::number(result, 'g', 15); // Mettre à jour l'expression avec le résultat
}

double MainWindow::evaluateExpression(const QString &exp)
{
    std::stack<double> values;
    std::stack<QChar> operators;
    QString num;

    // Fonction pour appliquer un opérateur entre deux valeurs
    auto applyOperator = [](double a, double b, QChar op) -> double {
        switch(op.toLatin1()) {
        case '+': return a + b;
        case '-': return a - b;
        case 'x': return a * b;
        case '/': return (b != 0) ? a / b : throw std::runtime_error("Division par zéro");
        default: throw std::runtime_error("Opérateur inconnu");
        }
    };

    // Priorités des opérateurs
    auto precedence = [](QChar op) -> int {
        if (op == '+' || op == '-') return 1;
        if (op == 'x' || op == '/') return 2;
        return 0;
    };

    for (int i = 0; i < exp.length(); ++i) {
        QChar ch = exp[i];

        // Ignorer les espaces
        if (ch.isSpace())
            continue;

        // Accumuler les chiffres pour former un nombre
        if (ch.isDigit() || ch == '.') {
            num += ch;
        } else {
            // Convertir le nombre accumulé et l'empiler
            if (!num.isEmpty()) {
                values.push(num.toDouble());
                num.clear();
            }

            // Gérer les opérateurs
            if (ch == '+' || ch == '-' || ch == 'x' || ch == '/') {
                while (!operators.empty() && precedence(operators.top()) >= precedence(ch)) {
                    double b = values.top(); values.pop();
                    double a = values.top(); values.pop();
                    QChar op = operators.top(); operators.pop();
                    values.push(applyOperator(a, b, op));
                }
                operators.push(ch);
            }
        }
    }

    // Empiler le dernier nombre
    if (!num.isEmpty()) {
        values.push(num.toDouble());
    }

    // Évaluer l'expression en vidant les piles
    while (!operators.empty()) {
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        QChar op = operators.top(); operators.pop();
        values.push(applyOperator(a, b, op));
    }

    // Résultat final
    return values.top();
}


void MainWindow::binary_operation_pressed()
{
    QPushButton *button = (QPushButton *) sender();

    // Ajouter le symbole d'opération à l'expression
    if (button->text() == "+") expression += "+";
    else if (button->text() == "-") expression += "-";
    else if (button->text() == "/") expression += "/";
    else if (button->text() == "x") expression += "x";

    ui->label->setText(expression); // Mettre à jour l'affichage
}


