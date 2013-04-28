// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/panel.hpp>

#include<boost/foreach.hpp>

#include<QScrollArea>
#include<QPushButton>
#include<QToolButton>
#include<QFrame>
#include<QLineEdit>
#include<QVBoxLayout>
#include<QMessageBox>

#include<ramen/Qr/QrLabel.hpp>

#include<ramen/ui/inspector.hpp>

#include<ramen/app/document.hpp>
#include<ramen/nodes/node.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/undo/undo_stack.hpp>

namespace ramen
{
namespace ui
{

panel_t::panel_t( node_t *n) : n_(n), panel_(0)
{
    panel_ = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);
    layout->setSizeConstraint( QLayout::SetFixedSize);
    //layout->setSpacing( 5);

    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    edit_ = new QLineEdit( top);

    int hpos = inspector_t::Instance().left_margin() / 2;
    int vsize = edit_->sizeHint().height();
    
    label->setText( "Name");
    label->move( 0, 5);
    label->resize( hpos - 5, vsize);
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);

    edit_->move( hpos, 5);
    edit_->resize( inspector_t::Instance().width() - hpos - 15, vsize);
    edit_->setText( n->name().c_str());
    connect( edit_, SIGNAL( editingFinished()), this, SLOT( change_name()));

    hpos = inspector_t::Instance().left_margin() / 2;
    QToolButton *autokey_button = new QToolButton( top);
    autokey_button->setText( "Autokey");
    autokey_button->move( hpos, vsize + 10);
    QSize s = autokey_button->sizeHint();
    autokey_button->resize( s.width(), s.height());
    autokey_button->setCheckable( true);
    autokey_button->setChecked( n->autokey());
    connect( autokey_button, SIGNAL( toggled( bool)), this, SLOT( set_autokey( bool)));
    hpos += s.width() + 5;

    QToolButton *reset_button = new QToolButton( top);
    reset_button->setText( "Reset");
    reset_button->setEnabled( false);
    reset_button->move( hpos, vsize + 10);
    QSize s2 = reset_button->sizeHint();
    reset_button->resize( s2.width(), s.height());
    connect( reset_button, SIGNAL( pressed()), this, SLOT( reset()));
    hpos += s2.width() + 5;

    QToolButton *help_button = new QToolButton( top);
    help_button->setText( "Help");
    help_button->move( hpos, vsize + 10);
    s2 = help_button->sizeHint();
    help_button->resize( s2.width(), s.height());

    if( !n->help_string())
	help_button->setEnabled( false);

    connect( help_button, SIGNAL( pressed()), this, SLOT( show_help()));

    hpos += s.width() + 5;
    vsize += s.height();
    
    top->setMinimumSize( inspector_t::Instance().width(), vsize + 10);
    top->setMaximumSize( inspector_t::Instance().width(), vsize + 10);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

    layout->addWidget( top);

    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::HLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    // add any other widgets here
    BOOST_FOREACH( param_t& p, n->param_set())
    {
	QWidget *w = p.create_widgets();

	if( w)
	    layout->addWidget( w);
    }

    layout->addStretch();

    panel_->setLayout( layout);
}

panel_t::~panel_t() { panel_->deleteLater();}

void panel_t::update()
{
    BOOST_FOREACH( param_t& p, n_->param_set())
    {
	p.update_widgets();
    }
}

void panel_t::change_name()
{
    document_t::Instance().composition().rename_node( n_, edit_->text().toStdString());
    edit_->blockSignals( true);
    edit_->setText( n_->name().c_str());
    edit_->blockSignals( false);
    user_interface_t::Instance().update();
}

void panel_t::set_autokey( bool b) { n_->set_autokey( b);}

void panel_t::show_help()
{
    const char *str = n_->help_string();
    QMessageBox::information( 0, "Node Help", str);
}

void panel_t::reset()
{
    // TODO: implement this
}

} // namespace
} // namespace
