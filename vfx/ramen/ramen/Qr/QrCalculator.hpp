// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef QR_CALCULATOR_HPP
#define	QR_CALCULATOR_HPP

#include<loki/Singleton.h>

#include<string>

#include<QObject>

#include<ramen/Qr/parser.h>

class QWidget;
class QDialog;
class QDesktopWidget;
class QPushButton;
class QLineEdit;

class QrCalculatorImpl : public QObject
{
    Q_OBJECT

public:

    double value() const	{ return value_;}
    void setValue( double v)	{ value_ = v;}

    void show( int x, int y);

    bool eventFilter( QObject *watched, QEvent *event);

Q_SIGNALS:

    void valueChanged( double v);

private Q_SLOTS:

    void buttonClicked();
    void increment();
    void decrement();
    void changeSign();
    void clearCalculator();
    void cancelCalculator();

private:

    QrCalculatorImpl();
    ~QrCalculatorImpl();

    int width() const;
    int height() const;

    void close( bool success);

    friend struct Loki::CreateUsingNew<QrCalculatorImpl>;

    QString str_;

    double value_;
    double inc_;

    Parser parser_;

    QDialog *dialog_;
    QDesktopWidget *desktop_;

    QLineEdit *result_line_;
    QPushButton *button_sign_;
    QPushButton *button_minus2_;
    QPushButton *button_inc_;
    QPushButton *button_dec_;
    QPushButton *button_div_;
    QPushButton *button9_;
    QPushButton *button8_;
    QPushButton *button7_;
    QPushButton *button_times_;
    QPushButton *button6_;
    QPushButton *button5_;
    QPushButton *button4_;
    QPushButton *button_minus_;
    QPushButton *button3_;
    QPushButton *button1_;
    QPushButton *button2_;
    QPushButton *button_plus_;
    QPushButton *button_dot_;
    QPushButton *button_clear_;
    QPushButton *button0_;
};

typedef Loki::SingletonHolder<QrCalculatorImpl> QrCalculator;

#endif	/* _CALCULATOR_HPP */
