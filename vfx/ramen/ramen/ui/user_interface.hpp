// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_HPP
#define RAMEN_UI_HPP

#include<boost/signals.hpp>
#include<boost/filesystem/path.hpp>

#include<loki/Singleton.h>

#include<QObject>
#include<QString>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{
namespace ui
{

class main_window_t;

class user_interface_impl : public QObject
{
    Q_OBJECT

public:
	
    void init();
    void show();
    int run();
    void quit();

    void create_new_document();
    void open_document( const boost::filesystem::path& p, bool no_gui = true);

    void node_released( node_t *n);

    void update();

    node_t *active_node() const { return active_;}
    void set_active_node( node_t *n);

    node_t *context_node() const { return context_;}
    void set_context_node( node_t *n);

    main_window_t *main_window() { return window_;}

    const QString& image_types_string() const { return image_types_str_;}

    bool quitting() const { return quitting_;}

    // time
    int start_frame() const;
    int end_frame() const;
    int frame() const;

    // error reporting
    void fatal_error( const std::string& msg);
    void error( const std::string& msg);
    void inform( const std::string& msg);
    bool question( const std::string& what, bool default_answer = true);

    // sequence file selector
    bool image_sequence_file_selector( boost::filesystem::path& p, bool& sequence, bool& relative) const;
    bool image_sequence_file_selector( const std::string& title, const std::string& types, boost::filesystem::path& p,
					bool& sequence, bool& relative) const;

public Q_SLOTS:
    
    void set_start_frame( int t);
    void set_end_frame( int t);
    void set_frame( int t);

private:

    user_interface_impl();
    ~user_interface_impl();
	
    friend struct Loki::CreateUsingNew<user_interface_impl>;

    main_window_t *window_;
    node_t *active_, *context_;
    bool quitting_;

    QString image_types_str_;
};

typedef Loki::SingletonHolder<user_interface_impl> user_interface_t;

} // namespace
} // namespace

#endif
