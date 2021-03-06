/*
 Copyright (c) 2013, Insomniac Games
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
 - Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 \file
 \author Ron Pieket \n<http://www.ItShouldJustWorkTM.com> \n<http://twitter.com/RonPieket>
 */
/* MojoLib is documented at: http://www.itshouldjustworktm.com/mojolib/ */
#pragma once

// -- Mojo
#include "MojoUtil.h"
#include "MojoAbstractSet.h"
#include "MojoMap.h"
#include "MojoMultiMap.h"
#include "MojoKeyValue.h"

/**
 \class MojoRelation
 \ingroup group_container
 Defines a many-to-one relation, such as child to parent. Each child can have only one parent. A parent can have any
 number of children.
 Also implements the MojoAbstractSet interface. As a MojoAbstractSet, the children are considered the elements.
 \see MojoForEachChildOfParent
 \tparam key_T Key type. Must be hashable.
 */
template< typename key_T >
class MojoRelation final : public MojoAbstractSet< key_T >
{
public:
  /**
   Shorthand specialization of MojoKeyValue.
   */
  typedef MojoKeyValue< key_T, key_T > KeyValue;
  
  /**
   Default constructor. You must call Create() before the map is ready for use.
   */
  MojoRelation()
  {
    Init();
  }
  
  /**
   Initializing constructor. No need to call Create().
   \param[in] name The name of the map. Will also be used for internal memory allocation.
   \param[in] not_found_value The value to return if nothing was found.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig for
   details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc for
   details on how to set the global default.
   \param[in] fixed_array You may provide an array that the map will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   */
  MojoRelation( const char* name, const key_T& not_found_value = key_T(), const MojoConfig* config = NULL,
               MojoAlloc* alloc = NULL, KeyValue* fixed_array = NULL, int fixed_array_count = 0 )
  {
    Init();
    Create( name, not_found_value, config, alloc, fixed_array, fixed_array_count );
  }
  
  /**
   Create after default constructor or Destroy().
   \param[in] name The name of the map. Will also be used for internal memory allocation.
   \param[in] not_found_value The value to return if nothing was found.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig for
   details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc for
   details on how to set the global default.
   \param[in] fixed_array You may provide an array that the map will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   \return Status code.
   */
  MojoStatus Create( const char* name, const key_T& not_found_value = key_T(), const MojoConfig* config = NULL,
                   MojoAlloc* alloc = NULL, KeyValue* fixed_array = NULL, int fixed_array_count = 0 );
  
  /**
   Remove all entries and free all allocated buffers.
   */
  void Destroy();
  
  /**
   Remove all entries.
   */
  void Reset();
  
  /**
   Insert relation. If child key already exists in map, its parent will be replaced.
   \param[in] child Child of the parent-child relation to insert.
   \param[in] parent Parent of the parent-child relation to insert.
   \return Status code.
   */
  MojoStatus InsertChildParent( const key_T& child, const key_T& parent );
  
  /**
   Remove all child's relation with parent. There is never more than one.
   \param[in] child Child of the parent-child relation to remove.
   \return Status code.
   */
  MojoStatus RemoveChild( const key_T& child );
  
  /**
   Remove all relations where given key is child.
   \param[in] parent Parent to be removed.
   \return Status code.
   */
  MojoStatus RemoveParent( const key_T& parent );
  
  /**
   Find parent of given child.
   \param[in] child Child to look for.
   \return Parent of the child, or not_found_value.
   */
  key_T FindParent( const key_T& child ) const;
  
  /**
   Test presence of a child. If it is present, it means the child has a parent.
   \param[in] child Child to look for.
   \return true if child has a parent.
   */
  virtual bool Contains( const key_T& child ) const override;
  
  /**
   Test presence of a parent. If it is present, it means the parent has at least one child.
   \param[in] parent Parent to look for.
   \return true if parent has a child.
   */
  bool ContainsParent( const key_T& parent ) const;
  
  /**
   Square bracket operator is an alias for FindParent()
   */
  key_T operator[]( const key_T& child ) const { return FindParent( child ); }
  
  /**
   Update table sizes, if needed. This is only useful if the config specified no dynamic memory allocation. If dynamic
   memory allocation is allowed, tables are resized as needed during Insert() or Remove(), and Update() is unnecessary.
   \return Status code.
   */
  MojoStatus Update();
  
  /**
   Return table status state. This is the only way to find out if something went wrong in the default constructor.
   If Create() was used, the returned status code will be the same.
   \return Status code.
   */
  MojoStatus GetStatus() const;
  
  /**
   Get number of relations in the container.
   \return Number of relations.
   */
  int GetCount() const;
  
  /**
   Return name of the container.
   \return Given name.
   */
  const char* GetName() const { return m_Name; }
  
  /**
   Get index of first occupied slot in table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  int _GetFirstIndex() const;

  /**
   Get index of next occupied slot in table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  int _GetNextIndex( int index ) const;

  /**
   Verify that table index is in range. This is used for the ForEach... macros. It must be declared public to work with
   the macros, but should be considered private.
   \private
   */
  bool _IsIndexValid( int index ) const;
  
  /**
   Get key at a specific index in the table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  key_T _GetKeyAt( int index ) const;

  /**
   Get index of next slot in table with matching key. This is used for the ForEach... macros. It must be declared public
   to work with the macros, but should be considered private.
   \private
   */
  int _GetFirstIndexOf( const key_T& key ) const;

  /**
   Get index of next slot in table with matching key. This is used for the ForEach... macros. It must be declared public
   to work with the macros, but should be considered private.
   \private
   */
  int _GetNextIndexOf( const key_T& key, int index ) const;

  /**
   Verify that table index is in range. This is used for the ForEach... macros. It must be declared public to work with
   the macros, but should be considered private.
   \private
   */
  bool _IsIndexValidOf( const key_T& key, int index ) const;

  /**
   Get value at a specific index in the table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  key_T _GetValueAt( int index ) const;

  virtual void Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;

  virtual ~MojoRelation();

private:
  
  const char*                   m_Name;
  MojoMap< key_T, key_T >       m_ChildToParent;  // A child may have only one parent
  MojoMultiMap< key_T, key_T >  m_ParentToChild;  // A parent may have multiple children

  void Init();
};

// ---------------------------------------------------------------------------------------------------------------------
// Inline implementations

template< typename key_T >
void MojoRelation< key_T >::Init()
{
  m_Name = NULL;
}

template< typename key_T >
MojoStatus MojoRelation< key_T >::GetStatus() const
{
  MojoStatus status = m_ParentToChild.GetStatus();
  if( !status )
  {
    status = m_ChildToParent.GetStatus();
  }

  return status;
}

template< typename key_T >
MojoStatus MojoRelation< key_T >::Create( const char* name, const key_T& not_found_value, const MojoConfig* config,
                                        MojoAlloc* alloc, KeyValue* fixed_array, int fixed_array_count)
{
  m_Name = name;
  int count = fixed_array_count / 2;
  m_ParentToChild.Create( name, not_found_value, config, alloc, fixed_array, count );
  if( count )
  {
    fixed_array += count;
    count = fixed_array_count - count;
  }
  m_ChildToParent.Create( name, not_found_value, config, alloc, fixed_array, count );

  return GetStatus();
}

template< typename key_T >
MojoRelation< key_T >::~MojoRelation()
{
  Destroy();
}

template< typename key_T >
void MojoRelation< key_T >::Destroy()
{
  m_ParentToChild.Destroy();
  m_ChildToParent.Destroy();
}

template< typename key_T >
void MojoRelation< key_T >::Reset()
{
  m_ParentToChild.Reset();
  m_ChildToParent.Reset();
}

template< typename key_T >
MojoStatus MojoRelation< key_T >::InsertChildParent( const key_T& child, const key_T& parent )
{
  MojoStatus status;
  if( parent.IsHashNull() )
  {
    status = RemoveChild( child );
  }
  else if( child.IsHashNull() )
  {
    status = kMojoStatus_InvalidArguments;
  }
  else
  {
    RemoveChild( child );
    status = m_ChildToParent.Insert( child, parent );
    if( !status )
    {
      status = m_ParentToChild.Insert( parent, child );
    }
  }
  return status;
}

template< typename key_T >
MojoStatus MojoRelation< key_T >::RemoveChild( const key_T& child )
{
  if( !child.IsHashNull() )
  {
    key_T old_parent = m_ChildToParent.Remove( child );
    if( !old_parent.IsHashNull() )
    {
      return m_ParentToChild.Remove( old_parent, child );
    }
  }
  return kMojoStatus_NotFound;
}

template< typename key_T >
MojoStatus MojoRelation< key_T >::RemoveParent( const key_T& parent )
{
  if( !parent.IsHashNull() )
  {
    key_T child;
    MojoForEachMultiValue( m_ParentToChild, parent, child )
    {
      m_ChildToParent.Remove( child );
    }
    return m_ParentToChild.Remove( parent );
  }
  return kMojoStatus_NotFound;
}

template< typename key_T >
MojoStatus MojoRelation< key_T >::Update()
{
  MojoStatus status = m_ChildToParent.Update();
  if( !status )
  {
    status = m_ParentToChild.Update();
  }
  return status;
}

template< typename key_T >
bool MojoRelation< key_T >::ContainsParent( const key_T& parent ) const
{
  return m_ParentToChild.Contains( parent );
}

template< typename key_T >
bool MojoRelation< key_T >::Contains( const key_T& child ) const
{
  return m_ChildToParent.Contains( child );
}

template< typename key_T >
int MojoRelation< key_T >::GetCount() const
{
  return m_ChildToParent.GetCount();
}

template< typename key_T >
int MojoRelation< key_T >::_GetFirstIndex() const
{
  return m_ChildToParent._GetFirstIndex();
}

template< typename key_T >
int MojoRelation< key_T >::_GetNextIndex( int index ) const
{
  return m_ChildToParent._GetNextIndex( index );
}

template< typename key_T >
bool MojoRelation< key_T >::_IsIndexValid( int index ) const
{
  return m_ChildToParent._IsIndexValid( index );
}

template< typename key_T >
key_T MojoRelation< key_T >::_GetKeyAt( int index ) const
{
  return m_ChildToParent._GetKeyAt( index );
}

template< typename key_T >
void MojoRelation< key_T >::Enumerate( const MojoCollector< key_T >& collector,
                                      const MojoAbstractSet< key_T >* limit ) const
{
  if( limit )
  {
    for( int i = _GetFirstIndex(); _IsIndexValid( i ); i = _GetNextIndex( i ) )
    {
      key_T key = _GetKeyAt( i );
      if( limit->Contains( key ) )
      {
        collector.Push( key );
      }
    }
  }
  else
  {
    for( int i = _GetFirstIndex(); _IsIndexValid( i ); i = _GetNextIndex( i ) )
    {
      collector.Push( _GetKeyAt( i ) );
    }
  }
}

template< typename key_T >
int MojoRelation< key_T >::_GetEnumerationCost() const
{
  return GetCount();
}

template< typename key_T >
int MojoRelation< key_T >::_GetChangeCount() const
{
  return m_ChildToParent._GetChangeCount();
}

template< typename key_T >
key_T MojoRelation< key_T >::FindParent( const key_T& child ) const
{
  return m_ChildToParent.Find( child );
}


template< typename key_T >
int MojoRelation< key_T >::_GetFirstIndexOf( const key_T& key ) const
{
  return m_ParentToChild._GetFirstIndexOf( key );
}

template< typename key_T >
int MojoRelation< key_T >::_GetNextIndexOf( const key_T& key, int index ) const
{
  return m_ParentToChild._GetNextIndexOf( key, index );
}

template< typename key_T >
bool MojoRelation< key_T >::_IsIndexValidOf( const key_T& key, int index ) const
{
  return m_ParentToChild._IsIndexValidOf( key, index );
}

template< typename key_T >
key_T MojoRelation< key_T >::_GetValueAt( int index ) const
{
  return m_ParentToChild._GetValueAt( index );
}

/**
 \ingroup group_container
 A macro to help you iterate over all children of a given parent in a MojoRelation
 This macro will expand into a <tt>for(;;)</tt> statement that contains all the magic to visit all children of the
 parent in the relation.
 \param[in] container MojoRelation by reference
 \param[in] parent The parent whose children we want to visit
 \param[out] child_variable Existing variable that will receive each child in turn
 */
#define MojoForEachChildOfParent( container, parent, child_variable ) \
for( int _i = ( container )._GetFirstIndexOf( parent ); \
    ( container )._IsIndexValidOf( parent, _i ) ? ( child_variable = ( container )._GetValueAt( _i ), true ) : false; \
    _i = ( container )._GetNextIndexOf( parent, _i ) )
