// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/Qr/QrCalculator.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<QDialog>
#include<QApplication>
#include<QDesktopWidget>
#include<QLineEdit>
#include<QPushButton>
#include<QMouseEvent>
#include<QKeyEvent>

QrCalculatorImpl::QrCalculatorImpl()
{
    desktop_ = new QDesktopWidget();

    dialog_ = new QDialog( 0, Qt::FramelessWindowHint);
    dialog_->setMinimumSize( width(), height());
    dialog_->setMaximumSize( width(), height());
    dialog_->installEventFilter( this);

    result_line_ = new QLineEdit( dialog_);
    result_line_->setGeometry(QRect(0, 0, 121, 26));
    result_line_->setAlignment( Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    result_line_->setText( "");
    result_line_->setReadOnly( true);
    result_line_->installEventFilter( this);

    button_sign_ = new QPushButton( dialog_);
    button_sign_->setGeometry(QRect(0, 20, 31, 31));
    button_sign_->setText( "x");
    button_sign_->setAutoDefault( false);
    connect( button_sign_, SIGNAL( clicked()), this, SLOT( cancelCalculator()));

    button_minus2_ = new QPushButton( dialog_);
    button_minus2_->setGeometry(QRect(30, 20, 31, 31));
    button_minus2_->setText( "+/-");
    button_minus2_->setAutoDefault( false);
    connect( button_minus2_, SIGNAL( clicked()), this, SLOT( changeSign()));

    button_inc_ = new QPushButton( dialog_);
    button_inc_->setGeometry(QRect(60, 20, 31, 31));
    button_inc_->setText( "Add");
    button_inc_->setAutoDefault( false);
    connect( button_inc_, SIGNAL( clicked()), this, SLOT( increment()));

    button_dec_ = new QPushButton( dialog_);
    button_dec_->setGeometry(QRect(90, 20, 31, 31));
    button_dec_->setText( "Sub");
    button_dec_->setAutoDefault( false);
    connect( button_dec_, SIGNAL( clicked()), this, SLOT( decrement()));

    button_div_ = new QPushButton( dialog_);
    button_div_->setGeometry(QRect(90, 50, 31, 31));
    button_div_->setText( "/");
    button_div_->setShortcut( QKeySequence( "/"));
    button_div_->setAutoDefault( false);
    connect( button_div_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button9_ = new QPushButton( dialog_);
    button9_->setGeometry(QRect(60, 50, 31, 31));
    button9_->setText( "9");
    button9_->setShortcut( QKeySequence( "9"));
    button9_->setAutoDefault( false);
    connect( button9_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button8_ = new QPushButton( dialog_);
    button8_->setGeometry(QRect(30, 50, 31, 31));
    button8_->setText( "8");
    button8_->setShortcut( QKeySequence( "8"));
    button8_->setAutoDefault( false);
    connect( button8_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button7_ = new QPushButton( dialog_);
    button7_->setGeometry(QRect(0, 50, 31, 31));
    button7_->setText( "7");
    button7_->setShortcut( QKeySequence( "7"));
    button7_->setAutoDefault( false);
    connect( button7_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button_times_ = new QPushButton( dialog_);
    button_times_->setGeometry(QRect(90, 80, 31, 31));
    button_times_->setText( "*");
    button_times_->setShortcut( QKeySequence( "*"));
    button_times_->setAutoDefault( false);
    connect( button_times_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button6_ = new QPushButton( dialog_);
    button6_->setGeometry(QRect(60, 80, 31, 31));
    button6_->setText( "6");
    button6_->setShortcut( QKeySequence( "6"));
    button6_->setAutoDefault( false);
    connect( button6_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button5_ = new QPushButton( dialog_);
    button5_->setGeometry(QRect(30, 80, 31, 31));
    button5_->setText( "5");
    button5_->setShortcut( QKeySequence( "5"));
    button5_->setAutoDefault( false);
    connect( button5_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button4_ = new QPushButton( dialog_);
    button4_->setGeometry(QRect(0, 80, 31, 31));
    button4_->setText( "4");
    button4_->setShortcut( QKeySequence( "4"));
    button4_->setAutoDefault( false);
    connect( button4_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button_minus_ = new QPushButton( dialog_);
    button_minus_->setGeometry(QRect(90, 110, 31, 31));
    button_minus_->setText( "-");
    button_minus_->setShortcut( QKeySequence( "-"));
    button_minus_->setAutoDefault( false);
    connect( button_minus_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button3_ = new QPushButton( dialog_);
    button3_->setGeometry(QRect(60, 110, 31, 31));
    button3_->setText( "3");
    button3_->setShortcut( QKeySequence( "3"));
    button3_->setAutoDefault( false);
    connect( button3_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button1_ = new QPushButton( dialog_);
    button1_->setGeometry(QRect(0, 110, 31, 31));
    button1_->setText( "1");
    button1_->setShortcut( QKeySequence( "1"));
    button1_->setAutoDefault( false);
    connect( button1_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button2_ = new QPushButton( dialog_);
    button2_->setGeometry(QRect(30, 110, 31, 31));
    button2_->setText( "2");
    button2_->setShortcut( QKeySequence( "2"));
    button2_->setAutoDefault( false);
    connect( button2_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button_plus_ = new QPushButton( dialog_);
    button_plus_->setGeometry(QRect(90, 140, 31, 31));
    button_plus_->setText( "+");
    button_plus_->setShortcut( QKeySequence( "+"));
    button_plus_->setAutoDefault( false);
    connect( button_plus_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button_dot_ = new QPushButton( dialog_);
    button_dot_->setGeometry(QRect(30, 140, 31, 31));
    button_dot_->setText( ".");
    button_dot_->setShortcut( QKeySequence( "."));
    button_dot_->setAutoDefault( false);
    connect( button_dot_, SIGNAL( clicked()), this, SLOT( buttonClicked()));

    button_clear_ = new QPushButton( dialog_);
    button_clear_->setGeometry(QRect(60, 140, 31, 31));
    button_clear_->setText( "C");
    button_clear_->setShortcut( QKeySequence( "C"));
    button_clear_->setAutoDefault( false);
    connect( button_clear_, SIGNAL( clicked()), this, SLOT( clearCalculator()));

    button0_ = new QPushButton( dialog_);
    button0_->setGeometry(QRect(0, 140, 31, 31));
    button0_->setText( "0");
    button0_->setShortcut( QKeySequence( "0"));
    button0_->setAutoDefault( false);
    connect( button0_, SIGNAL( clicked()), this, SLOT( buttonClicked()));
}

QrCalculatorImpl::~QrCalculatorImpl()
{
    delete dialog_;
    delete desktop_;
}

int QrCalculatorImpl::width() const { return 124;}
int QrCalculatorImpl::height() const { return 171;}

void QrCalculatorImpl::show( int x, int y)
{
    qApp->installEventFilter( this);
    str_ = QString();
    result_line_->setText( "");
    inc_ = 1.0;

    int xpos = x - ( width() / 2);
    int ypos = y - ( height() / 4);

    QRect bounds = desktop_->screenGeometry( QPoint( x, y));
    xpos = adobe::clamp( xpos, bounds.left() + 10, bounds.right() - 10 - width());
    ypos = adobe::clamp( ypos, bounds.top() + 10, bounds.bottom() - 10 - height());

    dialog_->setFocus();
    dialog_->move( xpos, ypos);
    dialog_->setModal( true);
    dialog_->show();
}

void QrCalculatorImpl::close( bool success)
{
    if( success)
    {
	if( !str_.isEmpty())
	{
	    try
	    {
		value_ = parser_.Evaluate( str_.toStdString());
		valueChanged( value_);
	    }
	    catch( ...) {}
	}
    }

    dialog_->hide();
    qApp->removeEventFilter( this);
    disconnect( this, SIGNAL( valueChanged( double)), 0, 0);
}

bool QrCalculatorImpl::eventFilter( QObject *watched, QEvent *event)
{
    if( event->type() == QEvent::KeyPress)
    {
	QKeyEvent *key_event = dynamic_cast<QKeyEvent*>( event);

	if( key_event->key() == Qt::Key_Backspace)
	{
	    str_.remove( str_.size()-1, 1);
	    result_line_->setText( str_);
	    return true;
	}
    }

    if( watched == result_line_)
    {
	if( event->type() == QEvent::MouseButtonPress)
	{
	    close( true);
	    event->accept();
	    return true;
	}
    }

    if( watched == dialog_)
    {
	if( event->type() == QEvent::KeyPress)
	{
	    QKeyEvent *key_event = dynamic_cast<QKeyEvent*>( event);

	    if( ( key_event->key() == Qt::Key_Enter) || ( key_event->key() == Qt::Key_Return))
	    {
		close( true);
		return true;
	    }

	    if( key_event->key() == Qt::Key_Escape)
	    {
		close( false);
		return true;
	    }
	}
    }

    if( watched == qApp)
    {
	if( event->type() == QEvent::MouseButtonPress)
	{
	    QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>( event);

	    if( QApplication::widgetAt( mouse_event->globalPos()) != (QWidget *) dialog_)
	    {
		close( false);
		return true;
	    }
	}
    }

    return QObject::eventFilter( watched, event);
}

void QrCalculatorImpl::buttonClicked()
{
    QPushButton *but = dynamic_cast<QPushButton*>( sender());
    str_.append( but->text());
    result_line_->setText( str_);
}

void QrCalculatorImpl::increment()
{
    if( str_.isEmpty())
    {
	value_ += inc_;
	valueChanged( value_);
    }
    else
    {
	try
	{
	    inc_ = parser_.Evaluate( str_.toStdString());
	    value_ += inc_;
	    valueChanged( value_);
	}
	catch( ...) {}
    }

    result_line_->setText( "");
}

void QrCalculatorImpl::decrement()
{
    if( str_.isEmpty())
    {
	value_ -= inc_;
	valueChanged( value_);
    }
    else
    {
	try
	{
	    inc_ = parser_.Evaluate( str_.toStdString());
	    value_ -= inc_;
	    valueChanged( value_);
	}
	catch( ...) {}
    }

    result_line_->setText( "");
}

void QrCalculatorImpl::changeSign()
{
    if( str_.isEmpty())
    {
	str_ = "-";
	result_line_->setText( str_);
    }
    else
    {
	try
	{
	    double x = parser_.Evaluate( str_.toStdString());
	    x = -x;
	    str_.setNum( x);
	    result_line_->setText( str_);
	}
	catch( ...)
	{
	    str_ = QString();
	    result_line_->setText( str_);
	}
    }
}

void QrCalculatorImpl::clearCalculator()
{
    str_ = QString();
    result_line_->setText( "");
}

void QrCalculatorImpl::cancelCalculator() { close( false);}
