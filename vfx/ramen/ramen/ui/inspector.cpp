// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/inspector.hpp>

#include<boost/bind.hpp>

#include<QApplication>
#include<QFontMetrics>

#include<QVBoxLayout>

#include<ramen/Qr/QrDoubleSpinBox.hpp>
#include<ramen/Qr/QrLabel.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/ui/panel.hpp>

namespace ramen
{
namespace ui
{

inspector_impl::inspector_impl() : window_(0), current_( panels_.end()), left_margin_( 0), width_( 0)
{
    window_ = new QrUtilityWindow();
    window_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred);
    window_->setMinimumSize( width()+16, 0);
    window_->setMaximumSize( width()+16, QWIDGETSIZE_MAX);
    window_->setWindowTitle( QObject::tr( "Inspector"));
    window_->setContentsMargins( 0, 0, 0, 0);

    scroll_ = new QScrollArea();
    scroll_->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded);
    scroll_->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    scroll_->setWidgetResizable( true);
    scroll_->setContentsMargins( 0, 0, 0, 0);

    view_ = new QrContainerWidget();
    scroll_->setWidget( view_);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);
    layout->addWidget( scroll_);
    window_->setLayout( layout);
}

inspector_impl::~inspector_impl() {}

int inspector_impl::left_margin() const
{
    if( !left_margin_)
    {
        QrLabel *tmp = new QrLabel();
        tmp->setText( "M");
        QSize s = tmp->sizeHint();
        delete tmp;

        const int max_label_lenght = 14;
        left_margin_ = s.width() * max_label_lenght;
    }

    return left_margin_;
}

int inspector_impl::width() const
{
    if( !width_)
    {
        QrDoubleSpinBox *tmp = new QrDoubleSpinBox();
        QSize s = tmp->sizeHint();
        delete tmp;

        // adjust for the rgba param case
        s.setWidth( s.width() - 20);
        width_ = left_margin() + 5 + (2 * s.height()) + ( s.width() * 4) + 25;
    }

    return width_;
}

void inspector_impl::show()	{ window_->show();}
void inspector_impl::hide()	{ window_->hide();}
void inspector_impl::toggle()	{ window_->toggle();}

void inspector_impl::edit_node( node_t *n)
{
    connection_.disconnect();

    if( current_ != panels_.end())
    {
	view_->clearContents();
	current_ = panels_.end();
    }

    if( n != 0)
    {
	std::map<node_t*,panel_t*>::const_iterator it = panels_.find( n);

	if( it == panels_.end())
	    create_panel( n);

	current_ = panels_.find( n);
	view_->setContents( current_->second->widget());
	connection_ = n->params_changed.connect( boost::bind( &inspector_impl::recreate_panel, this, _1));
    }
}

void inspector_impl::delete_panel( node_t *n)
{
    std::map<node_t*, panel_t*>::iterator it( panels_.find( n));

    if( it != panels_.end())
    {
	delete it->second;
	panels_.erase( n);
    }
}

void inspector_impl::update()
{
    if( current_ != panels_.end())
	current_->second->update();
}

void inspector_impl::create_panel( node_t *n)
{
    panel_t *p = new panel_t( n);
    panels_[n] = p;
}

void inspector_impl::recreate_panel( node_t *n)
{
    edit_node( 0);
    delete_panel( n);
    create_panel( n);
    edit_node( n);
}

} // namespace
} // namespace
