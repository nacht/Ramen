// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_editor_toolbar.hpp>

#include<QHBoxLayout>
#include<QPushButton>
#include<QSpacerItem>
#include<QButtonGroup>
#include<QToolButton>
#include<QFrame>

#include<ramen/Qr/QrSmallToolButton.hpp>

#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/anim_editor_select_tool.hpp>
#include <loki/Singleton.h>

namespace ramen
{
namespace ui
{

anim_editor_toolbar_t::anim_editor_toolbar_t(QWidget* parent) : QWidget( parent)
{
    setMinimumSize( 0, 35);
    setMaximumSize( QWIDGETSIZE_MAX, 35);
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 5, 5, 5, 5);

    QrSmallToolButton *select_ = new QrSmallToolButton( this);
    select_->setIcon( QIcon( ":icons/arrow.png"));
    select_->setFocusPolicy( Qt::NoFocus);
    select_->setCheckable(true);
    select_->setChecked(true);
    select_->setToolTip( "Select tool");
    //connect( select_, SIGNAL( toggled( bool)), this, SLOT( pick_select_tool( bool)));
    layout->addWidget( select_);

    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    // tangents
    QrSmallToolButton *smooth_tangents_ = new QrSmallToolButton( this);
    smooth_tangents_->setIcon( QIcon( ":icons/smooth_tan.png"));
    smooth_tangents_->setFocusPolicy( Qt::NoFocus);
    smooth_tangents_->setToolTip( "Smooth tangents");
    connect( smooth_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_smooth()));
    layout->addWidget( smooth_tangents_);

    QrSmallToolButton *flat_tangents_ = new QrSmallToolButton( this);
    flat_tangents_->setIcon( QIcon( ":icons/flat_tan.png"));
    flat_tangents_->setFocusPolicy( Qt::NoFocus);
    flat_tangents_->setToolTip( "Flat tangents");
    connect( flat_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_flat()));
    layout->addWidget( flat_tangents_);
    
    QrSmallToolButton *linear_tangents_ = new QrSmallToolButton( this);
    linear_tangents_->setIcon( QIcon( ":icons/linear_tan.png"));
    linear_tangents_->setFocusPolicy( Qt::NoFocus);
    linear_tangents_->setToolTip( "Linear tangents");
    connect( linear_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_linear()));
    layout->addWidget( linear_tangents_);

    QrSmallToolButton *step_tangents_ = new QrSmallToolButton( this);
    step_tangents_->setIcon( QIcon( ":icons/step_tan.png"));
    step_tangents_->setFocusPolicy( Qt::NoFocus);
    step_tangents_->setToolTip( "Step interpolation");
    connect( step_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_step()));
    layout->addWidget( step_tangents_);

    separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    QrSmallToolButton *constant_extrap_ = new QrSmallToolButton( this);
    constant_extrap_->setText( "C");
    constant_extrap_->setFocusPolicy( Qt::NoFocus);
    constant_extrap_->setToolTip( "Constant extrapolation");
    connect( constant_extrap_, SIGNAL( pressed()), this, SLOT( set_extrapolation_constant()));
    layout->addWidget( constant_extrap_);

    QrSmallToolButton *linear_extrap_ = new QrSmallToolButton( this);
    linear_extrap_->setText( "L");
    linear_extrap_->setFocusPolicy( Qt::NoFocus);
    linear_extrap_->setToolTip( "Linear extrapolation");
    connect( linear_extrap_, SIGNAL( pressed()), this, SLOT( set_extrapolation_linear()));
    layout->addWidget( linear_extrap_);

    QrSmallToolButton *repeat_extrap_ = new QrSmallToolButton( this);
    repeat_extrap_->setText( "R");
    repeat_extrap_->setFocusPolicy( Qt::NoFocus);
    repeat_extrap_->setToolTip( "Repeat extrapolation");
    connect( repeat_extrap_, SIGNAL( pressed()), this, SLOT( set_extrapolation_repeat()));
    layout->addWidget( repeat_extrap_);

    separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    QrSmallToolButton *frame_all_ = new QrSmallToolButton( this);
    frame_all_->setText( "A");
    frame_all_->setFocusPolicy( Qt::NoFocus);
    frame_all_->setToolTip( "Frame all");
    connect( frame_all_, SIGNAL( pressed()), this, SLOT( frame_all()));
    layout->addWidget( frame_all_);

    QrSmallToolButton *frame_selection_ = new QrSmallToolButton( this);
    frame_selection_->setText( "S");
    frame_selection_->setFocusPolicy( Qt::NoFocus);
    frame_selection_->setToolTip( "Frame selection");
    connect( frame_selection_, SIGNAL( pressed()), this, SLOT( frame_selection()));
    layout->addWidget( frame_selection_);

    QSpacerItem *s = new QSpacerItem( 30, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem( s);

    setLayout( layout);

    select_tool_ = new anim_editor_select_tool_t();
}

anim_editor_toolbar_t::~anim_editor_toolbar_t() { delete select_tool_;}

anim_editor_tool_t *anim_editor_toolbar_t::tool() { return select_tool_;}

void anim_editor_toolbar_t::set_tangents_smooth()   { anim_editor_t::Instance().set_tangents_smooth();}
void anim_editor_toolbar_t::set_tangents_flat()	    { anim_editor_t::Instance().set_tangents_flat();}
void anim_editor_toolbar_t::set_tangents_linear()   { anim_editor_t::Instance().set_tangents_linear();}
void anim_editor_toolbar_t::set_tangents_step()	    { anim_editor_t::Instance().set_tangents_step();}

void anim_editor_toolbar_t::set_extrapolation_constant()
{
    anim_editor_t::Instance().set_extrapolation_constant();
}

void anim_editor_toolbar_t::set_extrapolation_linear()
{
    anim_editor_t::Instance().set_extrapolation_linear();
}

void anim_editor_toolbar_t::set_extrapolation_repeat()
{
    anim_editor_t::Instance().set_extrapolation_repeat();
}

void anim_editor_toolbar_t::frame_selection()	{ anim_editor_t::Instance().frame_selection();}
void anim_editor_toolbar_t::frame_all()		{ anim_editor_t::Instance().frame_all();}

} // namespace
} // namespace
