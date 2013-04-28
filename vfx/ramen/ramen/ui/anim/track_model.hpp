// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_TRACK_MODEL_HPP
#define	RAMEN_UI_ANIM_TRACK_MODEL_HPP

#include<QAbstractItemModel>

#include<boost/shared_ptr.hpp>

#include<ramen/anim/track_fwd.hpp>

namespace ramen
{

namespace ui
{

class track_model_t : public QAbstractItemModel
{
    Q_OBJECT

public:

    track_model_t();

    boost::shared_ptr<anim::track_t> tracks() { return tracks_;}
    
    anim::track_t *root() { return tracks_.get();}

    QModelIndex index( int row, int column, const QModelIndex &parent) const;
    QModelIndex parent( const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:

    boost::shared_ptr<anim::track_t> tracks_;
};

} // namespace
} // namespace

#endif
