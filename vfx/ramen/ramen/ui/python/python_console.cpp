// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#include<ramen/ui/python/python_console.hpp>

#include<boost/python.hpp>

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QSplitter>
#include<QTextEdit>
#include<QPushButton>

#include<ramen/Qr/QrUtilityWindow.hpp>

#include<ramen/python/interpreter.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/undo/undo_stack.hpp>

namespace ramen
{
namespace ui
{

namespace
{

class python_output_redirector
{
public:

    python_output_redirector() {}
    void write( const std::string& str) { python_console_t::Instance().write( str);}
};

python_output_redirector out_to_console;

} // unnamed

python_console_impl::python_console_impl() : window_(0)
{
    window_ = new QWidget();
    window_->setWindowTitle( QObject::tr( "Python Console"));

    QVBoxLayout *layout = new QVBoxLayout();

    QWidget *toolbar_ = new QWidget();
	QHBoxLayout *hlayout = new QHBoxLayout();

	QPushButton *button = new QPushButton( "Run");
	connect( button, SIGNAL( clicked()), this, SLOT( run()));
	hlayout->addWidget( button);

    toolbar_->setLayout( hlayout);

    layout->addWidget( toolbar_);

    QSplitter *splitter = new QSplitter( Qt::Vertical);

    edit_ = new QTextEdit( splitter);
    edit_->setTabStopWidth( 10);
    splitter->addWidget( edit_);

    output_ = new QTextEdit( splitter);
    output_->setReadOnly( true);
    output_->setTabStopWidth( 10);
    output_->insertPlainText( "Output");
    splitter->addWidget( output_);

    layout->addWidget( splitter);
    window_->setLayout( layout);

    out_to_console = python_output_redirector();

    python::interpreter_t::Instance().main_namespace()["python_output_redirector"] =
	    boost::python::class_<python_output_redirector>( "python_output_redirector", boost::python::init<>())
		.def( "write", &python_output_redirector::write);

    boost::python::import( "sys").attr( "stderr") = out_to_console;
    boost::python::import( "sys").attr( "stdout") = out_to_console;
}

python_console_impl::~python_console_impl() { window_->deleteLater();}

void python_console_impl::run()
{
    undo::undo_stack_t::Instance().clear();

    output_area()->clear();
    QString script = edit_->toPlainText();

    try
    {
	boost::python::handle<> ignored(( PyRun_String( script.toStdString().c_str(),
  							    Py_file_input,
  							    python::interpreter_t::Instance().main_namespace().ptr(),
  							    python::interpreter_t::Instance().main_namespace().ptr())));

	ui::user_interface_t::Instance().update();
    }
    catch( boost::python::error_already_set )
    {
	PyErr_Print();
	PyErr_Clear();
    }
}

void python_console_impl::write( const std::string& str)
{
    output_area()->insertPlainText( QString::fromStdString( str));
    output_area()->update();
}

} // namespace
} // namespace
