// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/file_param.hpp>

#include<ramen/serialization/path.hpp>

#include<ramen/util/path.hpp>

#include<ramen/params/param_set.hpp>
#include<ramen/app/document.hpp>

#include<ramen/util/path.hpp>

#include<QLineEdit>
#include<QPushButton>
#include<QFileDialog>
#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>

#include<ramen/Qr/QrLabel.hpp>

namespace ramen
{

file_param_t::file_param_t() : static_param_t()
{
    is_input_ = true;
    set_default_value( boost::filesystem::path());
}

file_param_t::file_param_t( const std::string& name) : static_param_t( name)
{
    is_input_ = true;
    set_default_value( boost::filesystem::path()); 
}

file_param_t::file_param_t( const file_param_t& other) : static_param_t( other)
{
    input_ = 0;
    button_ = 0;
    is_input_ = other.is_input_;
}

void file_param_t::set_default_value( const boost::filesystem::path& p)
{
    value().assign( p);
    default_value().assign( p);
}

void file_param_t::set_value( const boost::filesystem::path& p)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( p);
    param_set()->param_changed( this);
}

bool file_param_t::empty() const
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));
    return p.empty();
}

std::string file_param_t::extension() const
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));
    return p.extension();
}

void file_param_t::set_extension( const std::string& ext)
{
    assert( !is_input_);
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));
    p.replace_extension( ext);
    value().assign( p);
    update_input_text();
}

std::string file_param_t::do_str() const
{
    return hash_string( get_value<boost::filesystem::path>( *this));
}

QWidget *file_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    input_ = new QLineEdit( top);
    button_ = new QPushButton( top);

    QSize s = input_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    button_->move( ui::inspector_t::Instance().width() - s.height() - 10, 0);
    button_->resize( s.height(), s.height());
    button_->setText( "...");
    button_->setEnabled( enabled());
    connect( button_, SIGNAL( clicked()), this, SLOT( select_pushed()));

    input_->move( ui::inspector_t::Instance().left_margin(), 0);
    input_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin()
                    - button_->width() - 10, s.height());

    if( is_input_)
        input_->setReadOnly( true);

    input_->setEnabled( enabled());

    update_input_text();

    connect( input_, SIGNAL( editingFinished()), this, SLOT( text_changed()));

    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void file_param_t::do_update_widgets()
{
    if( input_)
        update_input_text();
}

void file_param_t::do_enable_widgets( bool e)
{
    if( input_)
    {
        input_->setEnabled( e);
        button_->setEnabled( e);
    }
}

void file_param_t::select_pushed()
{
    QString fname;
    static bool was_relative = false;
    bool relative  = false;

    if( is_input_)
    {
        QFileDialog dialog( 0, tr( "Select file"), QString::null, "File (*.*)");

        #ifdef __APPLE__
            dialog.setOption( QFileDialog::DontUseNativeDialog);
        #endif

        dialog.setFileMode( QFileDialog::ExistingFile);

        QCheckBox *relative_check = new QCheckBox( QObject::tr( "Relative"));

        if( document_t::Instance().has_file())
            relative_check->setChecked( was_relative);
        else
        {
            was_relative = false;
            relative_check->setChecked( false);
            relative_check->setEnabled( false);
        }

        QGridLayout *glayout = (QGridLayout *) dialog.layout();
        glayout->addWidget( relative_check, glayout->rowCount(), 0, 1, glayout->columnCount(), Qt::AlignLeft);

        dialog.show();

        if( dialog.exec())
        {
            QStringList filenames = dialog.selectedFiles();
            fname = filenames[0];
            relative = relative_check->isChecked();
            was_relative = relative;
        }
    }
    else
        fname = QFileDialog::getSaveFileName( 0, tr( "Select file"), QString::null, "File (*.*)");

    if( !fname.isEmpty())
    {
        boost::filesystem::path p( fname.toStdString());

        if( is_input_ && relative)
        {
            assert( composition());
            p = composition()->absolute_to_relative( p);
        }

        update_input_text( p);

        param_set()->begin_edit();
        set_value( p);
        param_set()->end_edit();
    }
}

void file_param_t::text_changed()
{
    std::string fname( input_->text().toStdString());
    boost::filesystem::path p( fname);
    param_set()->begin_edit();
    set_value( p);
    param_set()->end_edit();
}

void file_param_t::update_input_text( const boost::filesystem::path& p)
{
    input_->blockSignals( true);
    input_->setText( p.external_file_string().c_str());
    input_->blockSignals( false);
}

void file_param_t::update_input_text()
{
    update_input_text( get_value<boost::filesystem::path>( *this));
}

void file_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
    ar & boost::serialization::make_nvp( "is_input", is_input_);
}

void file_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
    ar & boost::serialization::make_nvp( "is_input", is_input_);
}

boost::filesystem::path get_absolute_path( const param_t& p)
{
    boost::filesystem::path path( get_value<boost::filesystem::path>( p));

    if( !path.empty() && is_relative( path))
    {
	assert( p.composition());
	path = p.composition()->relative_to_absolute( path);
    }

    return path;
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<boost::filesystem::path>();

} // namespace

BOOST_CLASS_EXPORT( ramen::file_param_t)
