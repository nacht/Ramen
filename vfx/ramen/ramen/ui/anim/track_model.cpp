// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/track_model.hpp>

#include<ramen/anim/track.hpp>

namespace ramen
{
namespace ui
{

track_model_t::track_model_t() : QAbstractItemModel( 0)
{
    tracks_ = boost::shared_ptr<anim::track_t>( new anim::track_t( "Channels"));
}

QModelIndex track_model_t::index( int row, int column, const QModelIndex &parent) const
{
    if( !hasIndex( row, column, parent))
         return QModelIndex();

     anim::track_t *parentItem;

     if( !parent.isValid())
         parentItem = tracks_.get();
     else
         parentItem = static_cast<anim::track_t*>( parent.internalPointer());

     if( row >= parentItem->num_children())
	 return QModelIndex();

     anim::track_t *childItem = parentItem->child( row);
     return createIndex(row, column, childItem);
}

QModelIndex track_model_t::parent( const QModelIndex &index) const
{
    if( !index.isValid())
	return QModelIndex();

     anim::track_t *childItem = static_cast<anim::track_t*>( index.internalPointer());
     anim::track_t *parentItem = childItem->parent();

     if (parentItem == tracks_.get())
         return QModelIndex();

     return createIndex( parentItem->row(), 0, parentItem);
}

int track_model_t::rowCount(const QModelIndex &parent) const
{
    anim::track_t *parentItem;

    if( parent.column() > 0)
         return 0;

     if( !parent.isValid())
         parentItem = tracks_.get();
     else
         parentItem = static_cast<anim::track_t*>( parent.internalPointer());

     return parentItem->num_children();
 }

int track_model_t::columnCount(const QModelIndex &parent) const { return 1;}

QVariant track_model_t::data(const QModelIndex &index, int role) const
{
    if( !index.isValid())
	return QVariant();

     if( role != Qt::DisplayRole)
         return QVariant();

     anim::track_t *item = static_cast<anim::track_t*>( index.internalPointer());
     return QString( item->name().c_str());
}

Qt::ItemFlags track_model_t::flags(const QModelIndex &index) const
{
    if( !index.isValid())
	return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant track_model_t::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return QString( tracks_->name().c_str());

    return QVariant();
}

} // namespace
} // namespace
