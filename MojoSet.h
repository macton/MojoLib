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

// -- Standard Libs
#include <stdint.h>
#include <new>

// -- Mojo
#include "MojoStatus.h"
#include "MojoAlloc.h"
#include "MojoConfig.h"
#include "MojoUtil.h"
#include "MojoArray.h"
#include "MojoAbstractSet.h"
#include "MojoCollector.h"

/**
 \class MojoSet
 \ingroup group_container
 A key-only hash table.
 Also implements the MojoAbstractSet interface.
 \see MojoForEachKey
 \tparam key_T Key type. Must be hashable.
 */
template< typename key_T >
class MojoSet final : public MojoAbstractSet< key_T >
{
public:
  
  /**
   Default constructor. You must call Create() before the set is ready for use.
   */
  MojoSet()
  {
    Init();
  }

  /**
   Initializing constructor. No need to call Create().
   \param[in] name The name of the set. Will also be used for internal memory allocation.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig for
   details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc for
   details on how to set the global default.
   \param[in] fixed_array You may provide an array that the set will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   */
  MojoSet( const char* name, const MojoConfig* config = NULL, MojoAlloc* alloc = NULL,
          key_T* fixed_array = NULL, int fixed_array_count = 0 )
  {
    Init();
    Create( name, config, alloc, fixed_array, fixed_array_count );
  }
  
  /**
   Create after default constructor or Destroy().
   \param[in] name The name of the set. Will also be used for internal memory allocation.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig for
   details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc for
   details on how to set the global default.
   \param[in] fixed_array You may provide an array that the set will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   \return Status code.
   */
  MojoStatus Create( const char* name, const MojoConfig* config = NULL, MojoAlloc* alloc = NULL,
                   key_T* fixed_array = NULL, int fixed_array_count = 0 );

  /**
   Remove all keys and free all allocated buffers.
   */
  void Destroy();
  
  /**
   Remove all the leys.
   */
  void Reset();
  
  /**
   Insert key into set. If key already exists in set, does nothing.
   \param[in] key Key to insert.
   \return Status code.
   */
  MojoStatus Insert( const key_T& key );

  /**
   Remove key from the set.
   */
  MojoStatus Remove( const key_T& key );

  /**
   Test presence of a key.
   */
  virtual bool Contains( const key_T& key ) const override;

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
   Get number of keys in the set.
   */
  int GetCount() const;

  /**
   Return name of the set.
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

  virtual void Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;

  virtual ~MojoSet();

private:

  MojoAlloc*          m_Alloc;
  const char*         m_Name;
  key_T*              m_Keys;
  int                 m_ActiveCount;    // Number of key/values in play
  int                 m_AllocCount;     // Entries allocated
  int                 m_TableCount;     // Portion of the array currently used for hash table
  int                 m_ChangeCount;
  MojoStatus           m_Status;
  
  int                 m_AllocCountMin;
  int                 m_TableCountMin;
  int                 m_GrowThreshold;
  int                 m_ShrinkThreshold;
  bool                m_AutoGrow;
  bool                m_AutoShrink;
  bool                m_DynamicAlloc;
  
  void Init();
  void Grow();
  void Shrink();
  void AutoGrow();
  void AutoShrink();
  void Resize( int new_table_count, int new_capacity );
  int FindEmptyOrMatching( const key_T& key ) const;
  int FindEmpty( const key_T& key ) const;
  void Reinsert( int index );
  bool RemoveOne( const key_T& key );
  
  void Destruct( key_T* table, int count );
  void Construct( key_T* table, int count );
};
// ---------------------------------------------------------------------------------------------------------------------
// Inline implementations

/**
 \class MojoSetCollector
 Specialization of MojoCollector, with MojoSet as receiver.
 */
template< typename value_T >
class MojoSetCollector final : public MojoCollector< value_T >
{
public:
  /**
   Construct from a MojoSet pointer.
   \param[in] set The set to receive data.
   */
  MojoSetCollector( MojoSet< value_T >* set )
  : m_Set( set )
  {}
  virtual void Push( const value_T& value ) const override
  {
    m_Set->Insert( value );
  }
private:
  MojoSet< value_T >* m_Set;
};

template< typename key_T >
void MojoSet< key_T >::Init()
{
  m_Alloc = NULL;
  m_Name = NULL;
  m_Keys = NULL;
  m_TableCount = 0;
  m_AllocCount = 0;
  m_ActiveCount = 0;
  m_ChangeCount = 0;
  m_Status = kMojoStatus_NotInitialized;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::GetStatus() const
{
  return m_Status;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Create( const char* name, const MojoConfig* config, MojoAlloc* alloc,
                                   key_T* fixed_array, int fixed_array_count )
{
  if( !config )
  {
    config = MojoConfig::GetDefault();
  }
  if( !alloc )
  {
    alloc = MojoAlloc::GetDefault();
  }
  if( m_Status != kMojoStatus_NotInitialized )
  {
    m_Status = kMojoStatus_DoubleInitialized;
  }
  else if( config->m_AllocCountMin <= 1
          || config->m_TableCountMin <= 1
          || config->m_GrowThreshold <= config->m_ShrinkThreshold * 2 )
  {
    m_Status = kMojoStatus_InvalidArguments;
  }
  else
  {
    m_Status = kMojoStatus_Ok;
    
    m_Alloc           = fixed_array ? NULL : alloc;
    m_Name            = name;
    m_Keys            = fixed_array;
    m_AllocCount      = fixed_array_count;
    m_ActiveCount     = 0;
    
    m_AllocCountMin   = config->m_AllocCountMin;
    m_TableCountMin   = config->m_TableCountMin;
    m_GrowThreshold   = config->m_GrowThreshold;
    m_ShrinkThreshold = config->m_ShrinkThreshold;
    m_AutoGrow        = config->m_AutoGrow;
    m_AutoShrink      = config->m_AutoShrink;
    m_DynamicAlloc    = config->m_DynamicAlloc && m_Alloc;
    
    if( !m_Keys )
    {
      Resize( m_TableCountMin, MojoMax( m_AllocCountMin, m_TableCountMin ) );
    }
    
    if( !m_Keys )
    {
      m_Status = kMojoStatus_CouldNotAlloc;
    }
  }
  return m_Status;
}

template< typename key_T >
MojoSet< key_T >::~MojoSet()
{
  Destroy();
}

template< typename key_T >
void MojoSet< key_T >::Destroy()
{
  Resize( 0, 0 );
  Init();
}

template< typename key_T >
void MojoSet< key_T >::Reset()
{
  for( int i = 0; i < m_TableCount; ++i )
  {
    m_Keys[ i ] = key_T();
  }
  m_ActiveCount = 0;
  m_ChangeCount += 1;
  Resize( m_TableCountMin, MojoMax( m_AllocCountMin, m_TableCountMin ) );
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Insert( const key_T& key )
{
  MojoStatus status = m_Status;
  if( !status )
  {
    if( key.IsHashNull() )
    {
      status = kMojoStatus_InvalidArguments;
    }
    else
    {
      AutoGrow();
      
      if( m_ActiveCount < m_TableCount )
      {
        int index = FindEmptyOrMatching( key );
        if( m_Keys[ index ].IsHashNull() )
        {
          m_Keys[ index ] = key;
          m_ActiveCount += 1;
          m_ChangeCount += 1;
        }
      }
      else
      {
        status = kMojoStatus_CouldNotAlloc;
      }
    }
  }
  return status;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Remove( const key_T& key )
{
  if( m_Status )
  {
    return m_Status;
  }
  else if( !key.IsHashNull() )
  {
    if( RemoveOne( key ) )
    {
      m_ChangeCount += 1;
      AutoShrink();
      return kMojoStatus_Ok;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Update()
{
  MojoStatus status = m_Status;
  if( !m_Status )
  {
    Grow();
    Shrink();
  }
  return status;
}

template< typename key_T >
bool MojoSet< key_T >::Contains( const key_T& key ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key );
    return !m_Keys[ index ].IsHashNull();
  }
  return false;
}

template< typename key_T >
int MojoSet< key_T >::GetCount() const
{
  return m_ActiveCount;
}

template< typename key_T >
int MojoSet< key_T >::_GetFirstIndex() const
{
  return _GetNextIndex( -1 );
}

template< typename key_T >
int MojoSet< key_T >::_GetNextIndex( int index ) const
{
  for( int i = index + 1; i < m_TableCount; ++i )
  {
    if( !m_Keys[ i ].IsHashNull() )
    {
      return i;
    }
  }
  return m_TableCount;
}

template< typename key_T >
bool MojoSet< key_T >::_IsIndexValid( int index ) const
{
  return !m_Status && index < m_TableCount;
}

template< typename key_T >
key_T MojoSet< key_T >::_GetKeyAt( int index ) const
{
  return m_Keys[ index ];
}

template< typename key_T >
void MojoSet< key_T >::Destruct( key_T* table, int count )
{
  for( int i = 0; i < count; ++i )
  {
    table[ i ].~key_T();
  }
}

template< typename key_T >
void MojoSet< key_T >::Construct( key_T* table, int count )
{
  for( int i = 0; i < count; ++i )
  {
    new( table + i ) key_T();
  }
}

template< typename key_T >
void MojoSet< key_T >::Resize( int new_table_count, int new_capacity )
{
  if( m_Alloc && m_AllocCount != new_capacity )
  {
    key_T* old_keys = m_Keys;
    int old_alloc_count = m_AllocCount;
    int old_table_count = m_TableCount;
    
    // Allocate some new memory
    m_AllocCount = new_capacity;
    m_TableCount = new_table_count;
    m_ActiveCount = 0;
    if( m_AllocCount )
    {
      m_Keys = ( key_T* )m_Alloc->Allocate( m_AllocCount * sizeof( key_T ), m_Name );
      Construct( m_Keys, m_AllocCount );
    }
    else
    {
      m_Keys = NULL;
    }
    
    if( old_keys && m_Keys )
    {
      for( int i = 0; i < old_table_count; ++i )
      {
        if( !old_keys[ i ].IsHashNull() )
        {
          Insert( old_keys[ i ] );
        }
      }
    }
    
    if( old_keys )
    {
      Destruct( old_keys, old_alloc_count );
      m_Alloc->Free( old_keys );
    }
  }
  else if( new_table_count < m_TableCount )
  {
    // Shrink table in place
    int old_table_count = m_TableCount;
    m_TableCount = new_table_count;
    
    for( int i = 0; i < old_table_count; ++i )
    {
      if( !m_Keys[ i ].IsHashNull() )
      {
        Reinsert( i );
      }
    }
  }
  else if( new_table_count > m_TableCount )
  {
    // Grow table in place
    int old_table_count = m_TableCount;
    m_TableCount = new_table_count;
    
    for( int i = 0; i < old_table_count; ++i )
    {
      if( !m_Keys[ i ].IsHashNull() )
      {
        Reinsert( i );
      }
    }
    
    // Need to fix up the beginning of the new part of the table. It may contain keys that were bumped due to a
    // collision during the Grow operation
    for( int i = old_table_count; i < new_table_count; ++i )
    {
      if( m_Keys[ i ].IsHashNull() )
      {
        // We can stop if we reach an empty slot.
        break;
      }
      Reinsert( i );
    }
  }
}

template< typename key_T >
int MojoSet< key_T >::FindEmptyOrMatching( const key_T& key ) const
{
  int start_index = key.GetHash() % m_TableCount;
  
  // Look forward to the end of the key array
  for( int i = start_index; i < m_TableCount; ++i )
  {
    if( m_Keys[ i ].IsHashNull() || m_Keys[ i ] == key )
    {
      return i;
    }
  }
  
  // If not found, wrap around to the start
  for( int i = 0; i < start_index; ++i )
  {
    if( m_Keys[ i ].IsHashNull() || m_Keys[ i ] == key )
    {
      return i;
    }
  }
  
  return 0;
}

template< typename key_T >
int MojoSet< key_T >::FindEmpty( const key_T& key ) const
{
  int start_index = key.GetHash() % m_TableCount;
  
  // Look forward to the end of the key array
  for( int i = start_index; i < m_TableCount; ++i )
  {
    if( m_Keys[ i ].IsHashNull() )
    {
      return i;
    }
  }
  
  // If not found, wrap around to the start
  for( int i = 0; i < start_index; ++i )
  {
    if( m_Keys[ i ].IsHashNull() )
    {
      return i;
    }
  }
  
  return 0;
}

template< typename key_T >
void MojoSet< key_T >::Reinsert( int index )
{
  // Only move the entry if it is in the wrong place (due to collision)
  int new_index = FindEmptyOrMatching( m_Keys[ index ] );
  if( new_index != index )
  {
    // Occupy new location
    m_Keys[ new_index ] = m_Keys[ index ];
    
    // Vacate old location
    m_Keys[ index ] = key_T();
  }
}

template< typename key_T >
bool MojoSet< key_T >::RemoveOne( const key_T& key )
{
  if( key.IsHashNull() )
  {
    return false;
  }
  
  int index = FindEmptyOrMatching( key );
  if( m_Keys[ index ].IsHashNull() )
  {
    return false;
  }
  else
  {
    // Clear the slot
    m_Keys[ index ] = key_T();
    m_ActiveCount--;
    
    // Now fix up entries after this, that may have landed there after a hash collision.
    for( int i = index + 1; i < m_TableCount; ++i )
    {
      if( m_Keys[ i ].IsHashNull() )
      {
        return true;
      }
      Reinsert( i );
    }
    for( int i = 0; i < index; ++i )
    {
      if( m_Keys[ i ].IsHashNull() )
      {
        return true;
      }
      Reinsert( i );
    }
    return true;
  }
}

template< typename key_T >
void MojoSet< key_T >::Grow()
{
  // Make more room if table is getting crowded
  if( m_ActiveCount * 100 >= m_TableCount * m_GrowThreshold )
  {
    int new_table_count = m_TableCount * 2;
    int new_capacity = MojoMax( m_AllocCount, new_table_count );
    if( !m_DynamicAlloc )
    {
      new_capacity = m_AllocCount;
      new_table_count = MojoMin( new_table_count, new_capacity );
    }
    Resize( new_table_count, new_capacity );
  }
}

template< typename key_T >
void MojoSet< key_T >::Shrink()
{
  // Shrink if it's getting too empty
  if( m_TableCount > m_TableCountMin && m_ActiveCount * 100 < m_TableCount * m_ShrinkThreshold )
  {
    int new_table_count = MojoMax( m_TableCount / 2, m_TableCountMin );
    int new_capacity = MojoMax( new_table_count, m_AllocCountMin );
    if( !m_DynamicAlloc )
    {
      new_capacity = m_AllocCount;
    }
    Resize( new_table_count, new_capacity );
  }
}

template< typename key_T >
void MojoSet< key_T >::AutoGrow()
{
  if( m_AutoGrow )
  {
    Grow();
  }
}

template< typename key_T >
void MojoSet< key_T >::AutoShrink()
{
  if( m_AutoShrink )
  {
    Shrink();
  }
}

template< typename key_T >
void MojoSet< key_T >::Enumerate( const MojoCollector< key_T >& collector, const MojoAbstractSet< key_T >* limit ) const
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
int MojoSet< key_T >::_GetEnumerationCost() const
{
  return GetCount();
}

template< typename key_T >
int MojoSet< key_T >::_GetChangeCount() const
{
  return m_ChangeCount;
}

/**
 \ingroup group_container
 A macro to help you iterate over the keys in a hash table
 This macro will expand into a <tt>for(;;)</tt> statement that contains all the magic to visit all keys in the
 hash table. This macro works with MojoSet and MojoMap
 \param[in] container MojoSet or MojoMap by reference
 \param[out] key_variable Existing variable that will receive each key in the set or map in turn
 */
#define MojoForEachKey( container, key_variable ) \
for( int _i = ( container )._GetFirstIndex(); \
    ( container )._IsIndexValid( _i ) ? ( key_variable = ( container )._GetKeyAt( _i ), true ) : false; \
    _i = ( container )._GetNextIndex( _i ) )
