// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image_input/image_input_param.hpp>

#include<algorithm>
#include<sstream>

#include<boost/filesystem/operations.hpp>

#include<adobe/algorithm/find.hpp>

#include<ramen/Qr/QrLabel.hpp>

#include<QPushButton>
#include<QVBoxLayout>

#include<ramen/nodes/image_input/image_input_node.hpp>
#include<ramen/nodes/image_input/image_input_info.hpp>

#include<ramen/imageio/factory.hpp>
#include<ramen/imageio/multichannel_reader.hpp>

#include<ramen/app/document.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/util/path.hpp>

namespace ramen
{

image_input_param_t::image_input_param_t() : static_param_t()
{
    value().assign( image_input_info_t());
    default_value() = value();
}

image_input_param_t::image_input_param_t( const std::string& name) : static_param_t( name)
{
    value().assign( image_input_info_t());
    default_value() = value();
}

image_input_param_t::image_input_param_t( const std::string& name, const boost::filesystem::path& p, bool sequence) : static_param_t( name)
{
    path_sequence_t seq;

    if( sequence)
	seq = sequence_for_path( p);
    else
	seq = path_sequence_t( p, false);

    boost::filesystem::path img_path( seq( seq.start()));

    try
    {
	std::auto_ptr<imageio::reader_t> reader = imageio::factory_t::Instance().reader_for_image( img_path);

	if( reader.get())
	{
	    imageio::multichannel_reader_t *mr = dynamic_cast<imageio::multichannel_reader_t*>( reader.get());

	    if( mr && mr->has_extra_channels())
	    {
		value().assign( image_input_info_t( seq, mr->channel_list()));
		default_value() = value();
		return;
	    }
	}
    }
    catch( std::exception& e)
    {
    }

    value().assign( image_input_info_t( seq));
    default_value() = value();
}

image_input_param_t::image_input_param_t( const image_input_param_t& other) : static_param_t( other)
{
    input_ = 0;
    info_ = 0;
    slip_ = 0;
    lock_ = 0;
    red_combo_ = green_combo_ = blue_combo_ = alpha_combo_ = 0;
}

boost::filesystem::path image_input_param_t::path_for_frame( float f) const
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    // TODO: handle out of bounds frame numbers here!
    return info( f);
}

boost::tuple<int,int,int,int> image_input_param_t::channels() const
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    return info.channels();
}

std::vector<std::string> image_input_param_t::channel_list() const
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    return info.channel_list();
}

void image_input_param_t::set_channels( const std::string& red, const std::string& green,
			const std::string& blue, const std::string& alpha)
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    int c0, c1, c2, c3;

    std::vector<std::string>::const_iterator it;

    // red
    it = adobe::find( info.channel_list(), red);

    if( it != info.channel_list().end())
	c0 = std::distance( info.channel_list().begin(), it);
    else
	c0 = info.channel_list().size();

    // green
    it = adobe::find( info.channel_list(), green);

    if( it != info.channel_list().end())
	c1 = std::distance( info.channel_list().begin(), it);
    else
	c1 = info.channel_list().size();

    // blue
    it = adobe::find( info.channel_list(), blue);

    if( it != info.channel_list().end())
	c2 = std::distance( info.channel_list().begin(), it);
    else
	c2 = info.channel_list().size();

    // alpha
    it = adobe::find( info.channel_list(), alpha);

    if( it != info.channel_list().end())
	c3 = std::distance( info.channel_list().begin(), it);
    else
	c3 = info.channel_list().size();

    boost::tuple<int,int,int,int> ch( c0, c1, c2, c3);
    info.set_channels( ch);
    value().assign( info);
}

param_t *image_input_param_t::do_clone() const { return new image_input_param_t( *this);}

void image_input_param_t::slip_changed( int s)
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    info.set_slip( s);

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    param_set()->param_changed( this);
    param_set()->end_edit();
}

void image_input_param_t::lock_pushed( int state)
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    info.set_locked( state == Qt::Checked);

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    param_set()->param_changed( this);
    param_set()->end_edit();
}

void image_input_param_t::select_pushed()
{
    boost::filesystem::path p;
    bool sequence, relative;

    if( ui::user_interface_t::Instance().image_sequence_file_selector( p, sequence, relative))
    {
        image_input_info_t info( get_value<image_input_info_t>( *this));
	path_sequence_t seq;

	if( sequence)
	    seq = sequence_for_path( p);
	else
	    seq = path_sequence_t( p, false);

	info.set_path_sequence( seq);

	boost::filesystem::path img_path( seq( seq.start()));
	std::auto_ptr<imageio::reader_t> reader = imageio::factory_t::Instance().reader_for_image( img_path);

	if( reader.get())
	{
	    imageio::multichannel_reader_t *mr = dynamic_cast<imageio::multichannel_reader_t*>( reader.get());

	    if( mr && mr->has_extra_channels())
	    {
		if( mr->channel_list() != channel_list())
		    info.reset_channels();

		info.set_channel_list( mr->channel_list());

		param_set()->begin_edit();
		param_set()->add_command( this);
		value().assign( info);
		param_set()->param_changed( this);
		param_set()->end_edit();
		return;
	    }
	}

	info.reset_channels();
	info.clear_channel_list();

	param_set()->begin_edit();
	param_set()->add_command( this);
	value().assign( info);
	param_set()->param_changed( this);
	param_set()->end_edit();
    }
}

void image_input_param_t::channel_picked( int index)
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    boost::tuple<int,int,int,int> ch( info.channels());

    if( QObject::sender() == red_combo_)
	boost::get<0>( ch) = index;

    if( QObject::sender() == green_combo_)
	boost::get<1>( ch) = index;

    if( QObject::sender() == blue_combo_)
	boost::get<2>( ch) = index;

    if( QObject::sender() == alpha_combo_)
	boost::get<3>( ch) = index;

    info.set_channels( ch);

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    param_set()->param_changed( this);
    param_set()->end_edit();
}

QWidget *image_input_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    input_ = new QLineEdit( top);
    QPushButton *button_ = new QPushButton( top);

    QSize s = input_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    button_->move( ui::inspector_t::Instance().width() - s.height() - 10, 0);
    button_->resize( s.height(), s.height());
    button_->setText( "...");
    connect( button_, SIGNAL( clicked()), this, SLOT( select_pushed()));

    input_->move( ui::inspector_t::Instance().left_margin(), 0);
    input_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - button_->width() - 10, s.height());
    input_->setReadOnly( true);

    int current_height = s.height() + 5;

    label = new QrLabel( top);
    label->move( 0, current_height);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Info");

    info_ = new QLineEdit( top);
    info_->move( ui::inspector_t::Instance().left_margin(), current_height);
    info_->setReadOnly( true);
    info_->resize( input_->width(), s.height());
    update_info();
    
    current_height += s.height() + 5;

    label = new QrLabel( top);
    label->move( 0, current_height);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Slip");

    slip_ = new QrSpinBox( top);
    QSize s2 = slip_->sizeHint();
    slip_->move( ui::inspector_t::Instance().left_margin(), current_height);
    slip_->resize( s2.width(), s2.height());
    slip_->setRange( -65535, 65536);
    connect( slip_, SIGNAL( valueChanged( int)), this, SLOT( slip_changed( int)));

    lock_ = new QCheckBox( top);
    lock_->setText( "Lock");
    lock_->move( ui::inspector_t::Instance().left_margin() + s2.width() + 5, current_height);
    lock_->resize( s2.width() + 30, s2.height());
    connect( lock_, SIGNAL( stateChanged( int)), this, SLOT( lock_pushed( int)));

    current_height += s2.height() + 5;

    // popups
    {
	QrLabel *label = new QrLabel( top);
	red_combo_ = new QComboBox( top);
	red_combo_->setFocusPolicy( Qt::NoFocus);

	QSize s = red_combo_->sizeHint();

	label->move( 0, current_height);
	label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
	label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
	label->setText( "Red");

	red_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
	red_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
	connect( red_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
	current_height += s.height() + 5;
    }

    {
	QrLabel *label = new QrLabel( top);
	green_combo_ = new QComboBox( top);
	green_combo_->setFocusPolicy( Qt::NoFocus);

	QSize s = green_combo_->sizeHint();

	label->move( 0, current_height);
	label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
	label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
	label->setText( "Green");

	green_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
	green_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
	connect( green_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
	current_height += s.height() + 5;
    }

    {
	QrLabel *label = new QrLabel( top);
	blue_combo_ = new QComboBox( top);
	blue_combo_->setFocusPolicy( Qt::NoFocus);

	QSize s = blue_combo_->sizeHint();

	label->move( 0, current_height);
	label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
	label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
	label->setText( "Blue");

	blue_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
	blue_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
	connect( blue_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
	current_height += s.height() + 5;
    }

    {
	QrLabel *label = new QrLabel( top);
	alpha_combo_ = new QComboBox( top);
	alpha_combo_->setFocusPolicy( Qt::NoFocus);

	QSize s = alpha_combo_->sizeHint();

	label->move( 0, current_height);
	label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
	label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
	label->setText( "Alpha");

	alpha_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
	alpha_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
	connect( alpha_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
	current_height += s.height() + 5;
    }

    top->setMinimumSize( ui::inspector_t::Instance().width(), current_height);
    top->setMaximumSize( ui::inspector_t::Instance().width(), current_height);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    do_update_widgets();
    return top;
}

void image_input_param_t::do_update_widgets()
{
    if( input_)
    {
        slip_->blockSignals( true);
	lock_->blockSignals( true);

	image_input_info_t info( get_value<image_input_info_t>( *this));

	input_->setText( QString::fromStdString( info.format_string()));
	slip_->setValue( info.slip());

	if( info.locked())
	    lock_->setCheckState( Qt::Checked);
	else
	    lock_->setCheckState( Qt::Unchecked);

	update_info();

        slip_->blockSignals( false);
	lock_->blockSignals( false);

	std::vector<std::string> ch_list( channel_list());

	update_popup( red_combo_, ch_list, boost::get<0>( info.channels()));
	update_popup( green_combo_, ch_list, boost::get<1>( info.channels()));
	update_popup( blue_combo_, ch_list, boost::get<2>( info.channels()));
	update_popup( alpha_combo_, ch_list, boost::get<3>( info.channels()));
    }
}

void image_input_param_t::update_popup( QComboBox *popup, const std::vector<std::string>& ch_list, int value)
{
    popup->blockSignals( true);
    popup->clear();

    if( !ch_list.empty())
    {
	for( int i = 0; i < ch_list.size(); ++i)
	    popup->addItem( ch_list[i].c_str());

	popup->addItem( "None");
	popup->setEnabled( true);
    }
    else
    {
	popup->addItem( "Red");
	popup->addItem( "Green");
	popup->addItem( "Blue");
	popup->addItem( "Alpha");
	popup->setEnabled( false);
    }

    popup->setCurrentIndex( value);
    popup->blockSignals( false);
}

void image_input_param_t::update_info()
{
    if( info_)
    {
	image_input_info_t seq_info( get_value<image_input_info_t>( *this));

	if( seq_info.valid())
	{
	    std::stringstream ss;
	    ss << "frames " << seq_info.sequence_start() << "-" << seq_info.sequence_end();

	    image_input_node_t *n = dynamic_cast<image_input_node_t*>( parameterised());
	    imageio::reader_t *r = n->reader();

	    if( r)
	    {
		Imath::Box2i domain( r->domain());
		ss << " size [" << domain.size().x + 1 << " x " << domain.size().y + 1 << "]";
	    }

	    info_->setText( ss.str().c_str());
	}
	else
	    info_->setText( "No Sequence");
    }
}

path_sequence_t image_input_param_t::sequence_for_path( const boost::filesystem::path& p) const
{
    boost::filesystem::path dir;

    path_sequence_t seq( p);

    if( is_relative( p))
	dir = document_t::Instance().composition().relative_to_absolute( p).branch_path();
    else
	dir = p.branch_path();

    for( boost::filesystem::directory_iterator itr(dir); itr != boost::filesystem::directory_iterator(); ++itr)
    {
	if( !boost::filesystem::is_directory( itr->path()))
        {
            if( itr->path().filename()[0] != '.')
                seq.add_path( itr->path());
        }
    }

    return seq;
}

void image_input_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

void image_input_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<image_input_info_t>();

} // namespace

BOOST_CLASS_EXPORT( ramen::image_input_param_t)
