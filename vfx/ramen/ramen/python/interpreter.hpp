// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PYTHON_INTERPRETER_HPP
#define RAMEN_PYTHON_INTERPRETER_HPP

#include<boost/noncopyable.hpp>
#include<boost/python.hpp>

#include<loki/Singleton.h>

namespace ramen
{
namespace python
{

class interpreter_impl : boost::noncopyable
{
public:

    boost::python::object& main_namespace() { return main_namespace_;}

private:

    friend struct Loki::CreateUsingNew<interpreter_impl>;

    interpreter_impl();

    boost::python::object main_namespace_;
};

typedef Loki::SingletonHolder<interpreter_impl> interpreter_t;

} // namespace
} // namespace

#endif
