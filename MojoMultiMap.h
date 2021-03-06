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
#include "MojoKeyValue.h"

/**
 \class MojoMultiMap
 \ingroup group_container
 A one-to-many hash table. Every key has one or more values. If a key is not present in the table, its value is
 not_found_value. The not_found_value is specified when the table is initialized.
 Also implements the MojoAbstractSet interface. As a MojoAbstractSet, the map work more like a set. That is, only the
 presence of keys is used.
 \see MojoForEachMultiValue
 \tparam key_T Key type. Must be hashable.
 \tparam value_T Value type.
 */
template< typename key_T, typename value_T >
class MojoMultiMap final : public MojoAbstractSet< key_T >
{
public:
  /**
   Shorthand specialization of MojoKeyValue.
   */
  typedef MojoKeyValue< key_T, value_T > KeyValue;
  
  /**
   Default constructor. You must call Create() before the map is ready for use.
   */
  MojoMultiMap()
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
  MojoMultiMap( const char* name, const value_T& not_found_value = value_T(), const MojoConfig* config = NULL,
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
  MojoStatus Create( const char* name, const value_T& not_found_value = value_T(), const MojoConfig* config = NULL,
                   MojoAlloc* alloc = NULL, KeyValue* fixed_array = NULL, int fixed_array_count = 0 );
  
  /**
   Remove all entries and free all allocated buffers.
   */
  void Destroy();
  
  /**
   Remove all key-value pairs.
   */
  void Reset();
  
  /**
   Insert key-value pair into the map. If key already exists in map, the value is overwritten.
   \param[in] key Key of the key-value pair to insert.
   \param[in] value Value of the key-value pair to insert.
   \return Status code.
   */
  MojoStatus Insert( const key_T& key, const value_T& value );
  
  /**
   Remove all key-value pairs with given key from the map.
   \param[in] key Key to remove.
   \return Status code.
   */
  MojoStatus Remove( const key_T& key );

  /**
   Remove key-value pair from the map.
   \param[in] key Key of the key-value pair to remove.
   \param[in] value Value of the key-value pair to remove.
   \return Status code.
   */
  MojoStatus Remove( const key_T& key, const value_T& value );
  
  /**
   Find value that is associated with the key.
   */
  value_T Find( const key_T& key ) const;
  
  /**
   Test presence of a key.
   \param[in] key Key to seach for.
   \return true if key is in the map.
   */
  virtual bool Contains( const key_T& key ) const override;

  /**
   Test presence of a key-value pair.
   \param[in] key Key of the key-value pair to seach for.
   \param[in] value Value of the key-value pair to seach for.
   \return true if key-value pair is in the map.
   */
  bool Contains( const key_T& key, const value_T& value ) const;
  
  /**
   Square bracket operator is an alias for Find()
   */
  value_T operator[]( const key_T& key ) const { return Find( key ); }
  
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
   Get number of key-value pairs in the map.
   \return Number of pairs.
   */
  int GetCount() const;
  
  /**
   Return name of the map.
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
   Get index of first slot in table with a particular key. This is used for the ForEach... macros. It must be declared
   public to work with the macros, but should be considered private.
   \private
   */
  int _GetFirstIndexOf( const key_T& key ) const;

  /**
   Get index of next occupied slot in table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  int _GetNextIndex( int index ) const;

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
  bool _IsIndexValid( int index ) const;
  
  /**
   Verify that table index is in range. This is used for the ForEach... macros. It must be declared public to work with
   the macros, but should be considered private.
   \private
   */
  bool _IsIndexValidOf( const key_T& key, int index ) const { return _IsIndexValid( index ); }

  /**
   Get key at a specific index in the table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  key_T _GetKeyAt( int index ) const;
  
  /**
   Get value at a specific index in the table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  value_T _GetValueAt( int index ) const;
  
  /**
   Get key-value pair at a specific index in the table. This is used for the ForEach... macros. It must be declared
   public to work with the macros, but should be considered private.
   \private
   */
  KeyValue _GetKeyValueAt( int index ) const;
  
  virtual void Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;
  
  virtual ~MojoMultiMap();

private:
  
  MojoAlloc*          m_Alloc;
  const char*         m_Name;
  KeyValue*           m_KeyValues;
  value_T             m_NotFoundValue;
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
  int FindEmptyOrMatching( const key_T& key, const value_T& value ) const;
  int FindEmpty( const key_T& key ) const;
  void Reinsert( int index );
  void FixUp( int index, int count );
  bool RemoveAll( const key_T& key );
  bool RemoveOne( const key_T& key, const value_T& value );
  
  void Destruct( KeyValue* table, int count );
  void Construct( KeyValue* table, int count );
  bool IsFirstInRun( int index ) const;
};

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Init()
{
  m_Alloc = NULL;
  m_Name = NULL;
  m_KeyValues = NULL;
  m_TableCount = 0;
  m_AllocCount = 0;
  m_ActiveCount = 0;
  m_ChangeCount = 0;
  m_Status = kMojoStatus_NotInitialized;
}

// ---------------------------------------------------------------------------------------------------------------------
// Inline implementations

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::GetStatus() const
{
  return m_Status;
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Create( const char* name, const value_T& not_found_value,
                                                 const MojoConfig* config, MojoAlloc* alloc, KeyValue* fixed_array,
                                                 int fixed_array_count )
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
    
    m_Alloc             = fixed_array ? NULL : alloc;
    m_Name              = name;
    m_NotFoundValue     = not_found_value;
    m_KeyValues         = fixed_array;
    m_AllocCount        = fixed_array_count;
    m_ActiveCount       = 0;
    
    m_AllocCountMin     = config->m_AllocCountMin;
    m_TableCountMin     = config->m_TableCountMin;
    m_GrowThreshold     = config->m_GrowThreshold;
    m_ShrinkThreshold   = config->m_ShrinkThreshold;
    m_AutoGrow          = config->m_AutoGrow;
    m_AutoShrink        = config->m_AutoShrink;
    m_DynamicAlloc      = config->m_DynamicAlloc && m_Alloc;
    
    if( !m_KeyValues )
    {
      Resize( m_TableCountMin, MojoMax( m_AllocCountMin, m_TableCountMin ) );
    }
    
    if( !m_KeyValues )
    {
      m_Status = kMojoStatus_CouldNotAlloc;
    }
  }
  return m_Status;
}

template< typename key_T, typename value_T >
MojoMultiMap< key_T, value_T >::~MojoMultiMap()
{
  Destroy();
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Destroy()
{
  Resize( 0, 0 );
  Init();
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Reset()
{
  for( int i = 0; i < m_TableCount; ++i )
  {
    m_KeyValues[ i ] = KeyValue();
  }
  m_ActiveCount = 0;
  m_ChangeCount += 1;
  Resize( m_TableCountMin, MojoMax( m_AllocCountMin, m_TableCountMin ) );
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Insert( const key_T& key, const value_T& value )
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
        int index = FindEmptyOrMatching( key, value );
        if( m_KeyValues[ index ].key.IsHashNull() )
        {
          m_KeyValues[ index ].key = key;
          m_KeyValues[ index ].value = value;
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

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Remove( const key_T& key )
{
  if( m_Status )
  {
    return m_Status;
  }
  else if( !key.IsHashNull() )
  {
    if( RemoveAll( key ) )
    {
      m_ChangeCount += 1;
      AutoShrink();
      return kMojoStatus_Ok;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Remove( const key_T& key, const value_T& value )
{
  if( m_Status )
  {
    return m_Status;
  }
  else if( !key.IsHashNull() )
  {
    if( RemoveOne( key, value ) )
    {
      m_ChangeCount += 1;
      AutoShrink();
      return kMojoStatus_Ok;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Update()
{
  MojoStatus status = m_Status;
  if( !m_Status )
  {
    Grow();
    Shrink();
  }
  return status;
}

template< typename key_T, typename value_T >
value_T MojoMultiMap< key_T, value_T >::Find( const key_T& key ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key );
    if( !m_KeyValues[ index ].key.IsHashNull() )
    {
      return m_KeyValues[ index ].value;
    }
  }
  return m_NotFoundValue;
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetFirstIndexOf( const key_T& key ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key );
    if( !m_KeyValues[ index ].key.IsHashNull() )
    {
      return index;
    }
  }
  return m_TableCount;
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetNextIndexOf( const key_T& key, int index ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    for( int i = index + 1; i < m_TableCount; ++i )
    {
      if( m_KeyValues[ i ].key.IsHashNull() )
      {
        return m_TableCount;
      }
      if( m_KeyValues[ i ].key == key )
      {
        return i;
      }
    }
    
    for( int i = 0; i < index; ++i )
    {
      if( m_KeyValues[ i ].key.IsHashNull() )
      {
        return m_TableCount;
      }
      if( m_KeyValues[ i ].key == key )
      {
        return i;
      }
    }
  }
  return m_TableCount;
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::Contains( const key_T& key ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key );
    return !m_KeyValues[ index ].key.IsHashNull();
  }
  return false;
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::Contains( const key_T& key, const value_T& value ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key, value );
    return !m_KeyValues[ index ].key.IsHashNull();
  }
  return false;
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::GetCount() const
{
  return m_ActiveCount;
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetFirstIndex() const
{
  return _GetNextIndex( -1 );
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::IsFirstInRun( int index ) const
{
  const key_T& key = m_KeyValues[ index ].key;
  
  for( int i = index - 1; i >= 0; --i )
  {
    if( m_KeyValues[ i ].key.IsHashNull() )
    {
      return true;
    }
    if( m_KeyValues[ i ].key == key )
    {
      return false;
    }
  }
  
  for( int i = m_TableCount - 1; i > index; --i )
  {
    if( m_KeyValues[ i ].key.IsHashNull() )
    {
      return true;
    }
    if( m_KeyValues[ i ].key == key )
    {
      return false;
    }
  }
  
  return true;
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetNextIndex( int index ) const
{
  for( int i = index + 1; i < m_TableCount; ++i )
  {
    if( !m_KeyValues[ i ].key.IsHashNull() && IsFirstInRun( i ) )
    {
      return i;
    }
  }
  return m_TableCount;
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::_IsIndexValid( int index ) const
{
  return !m_Status && index < m_TableCount;
}

template< typename key_T, typename value_T >
key_T MojoMultiMap< key_T, value_T >::_GetKeyAt( int index ) const
{
  return m_KeyValues[ index ].key;
}

template< typename key_T, typename value_T >
value_T MojoMultiMap< key_T, value_T >::_GetValueAt( int index ) const
{
  return m_KeyValues[ index ].value;
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Destruct( KeyValue* table, int count )
{
  for( int i = 0; i < count; ++i )
  {
    table[ i ].~KeyValue();
  }
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Construct( KeyValue* table, int count )
{
  for( int i = 0; i < count; ++i )
  {
    new( table + i ) KeyValue();
  }
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Resize( int new_table_count, int new_capacity )
{
  if( m_Alloc && m_AllocCount != new_capacity )
  {
    KeyValue* old_key_values = m_KeyValues;
    int old_alloc_count = m_AllocCount;
    int old_table_count = m_TableCount;
    
    // Allocate some new memory
    m_AllocCount = new_capacity;
    m_TableCount = new_table_count;
    m_ActiveCount = 0;
    if( m_AllocCount )
    {
      m_KeyValues = ( KeyValue* )m_Alloc->Allocate( m_AllocCount * sizeof( KeyValue ), m_Name );
      Construct( m_KeyValues, m_AllocCount );
    }
    else
    {
      m_KeyValues = NULL;
    }
    
    if( old_key_values && m_KeyValues )
    {
      for( int i = 0; i < old_table_count; ++i )
      {
        if( !old_key_values[ i ].key.IsHashNull() )
        {
          Insert( old_key_values[ i ].key, old_key_values[ i ].value );
        }
      }
    }
    
    if( old_key_values )
    {
      Destruct( old_key_values, old_alloc_count );
      m_Alloc->Free( old_key_values );
    }
  }
  else if( new_table_count < m_TableCount )
  {
    // Shrink table in place
    int old_table_count = m_TableCount;
    m_TableCount = new_table_count;
    
    for( int i = 0; i < old_table_count; ++i )
    {
      if( !m_KeyValues[ i ].key.IsHashNull() )
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
      if( !m_KeyValues[ i ].key.IsHashNull() )
      {
        Reinsert( i );
      }
    }
    
    // Need to fix up the beginning of the new part of the table. It may contain keys that were bumped due to a
    // collision during the Grow operation
    for( int i = old_table_count; i < new_table_count; ++i )
    {
      if( m_KeyValues[ i ].key.IsHashNull() )
      {
        // We can stop if we reach an empty slot.
        break;
      }
      Reinsert( i );
    }
  }
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::FindEmptyOrMatching( const key_T& key ) const
{
  int start_index = key.GetHash() % m_TableCount;
  
  // Look forward to the end of the key array
  for( int i = start_index; i < m_TableCount; ++i )
  {
    if( m_KeyValues[ i ].key.IsHashNull() || m_KeyValues[ i ].key == key )
    {
      return i;
    }
  }
  
  // If not found, wrap around to the start
  for( int i = 0; i < start_index; ++i )
  {
    if( m_KeyValues[ i ].key.IsHashNull() || m_KeyValues[ i ].key == key )
    {
      return i;
    }
  }
  
  return 0;
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::FindEmptyOrMatching( const key_T& key, const value_T& value ) const
{
  int start_index = key.GetHash() % m_TableCount;
  
  // Look forward to the end of the key array
  for( int i = start_index; i < m_TableCount; ++i )
  {
    if( m_KeyValues[ i ].key.IsHashNull() || ( m_KeyValues[ i ].key == key && m_KeyValues[ i ].value == value ) )
    {
      return i;
    }
  }
  
  // If not found, wrap around to the start
  for( int i = 0; i < start_index; ++i )
  {
    if( m_KeyValues[ i ].key.IsHashNull() || ( m_KeyValues[ i ].key == key && m_KeyValues[ i ].value == value ) )
    {
      return i;
    }
  }
  
  return 0;
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Reinsert( int index )
{
  // Only move the entry if it is in the wrong place (due to collision)
  int new_index = FindEmptyOrMatching( m_KeyValues[ index ].key, m_KeyValues[ index ].value );
  if( new_index != index )
  {
    // Occupy new location
    m_KeyValues[ new_index ] = m_KeyValues[ index ];
    
    // Vacate old location
    m_KeyValues[ index ] = KeyValue();
  }
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::FixUp( int index, int count )
{
  for( int i = index + 1; i < m_TableCount; ++i )
  {
    if( !count-- )
    {
      return;
    }
    if( !m_KeyValues[ i ].key.IsHashNull() )
    {
      Reinsert( i );
    }
  }
  for( int i = 0; i < index; ++i )
  {
    if( !count-- )
    {
      return;
    }
    if( !m_KeyValues[ i ].key.IsHashNull() )
    {
      Reinsert( i );
    }
  }
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::RemoveAll( const key_T& key )
{
  int before_count = m_ActiveCount;
  if( !key.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key );
    if( !m_KeyValues[ index ].key.IsHashNull() )
    {
      int count = 0;
      int i = index;
      while( !m_KeyValues[ i ].key.IsHashNull() )
      {
        if( m_KeyValues[ i ].key == key )
        {
          m_KeyValues[ i ] = KeyValue();
          m_ActiveCount--;
        }
        count += 1;
        i = ( i + 1 ) % m_TableCount;
      }
      FixUp( index, count );
    }
  }
  return m_ActiveCount < before_count;
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::RemoveOne( const key_T& key, const value_T& value )
{
  int before_count = m_ActiveCount;
  if( !key.IsHashNull() && !value.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key );
    if( !m_KeyValues[ index ].key.IsHashNull() )
    {
      int count = 0;
      int i = index;
      while( !m_KeyValues[ i ].key.IsHashNull() )
      {
        if( m_KeyValues[ i ].key == key && m_KeyValues[ i ].value == value )
        {
          m_KeyValues[ i ] = KeyValue();
          m_ActiveCount--;
        }
        count += 1;
        i = ( i + 1 ) % m_TableCount;
      }
      FixUp( index, count );
    }
  }
  return m_ActiveCount < before_count;
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Grow()
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

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Shrink()
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

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::AutoGrow()
{
  if( m_AutoGrow )
  {
    Grow();
  }
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::AutoShrink()
{
  if( m_AutoShrink )
  {
    Shrink();
  }
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Enumerate( const MojoCollector< key_T >& collector,
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

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetEnumerationCost() const
{
  return GetCount();
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetChangeCount() const
{
  return m_ChangeCount;
}

/**
 \ingroup group_container
 A macro to help you iterate over all values of a specified key in a MojoMultiMap
 This macro will expand into a <tt>for(;;)</tt> statement that contains all the magic to visit all values of the
 key in a MojoMultiMap. This macro works with MojoMultiMap only
 \param[in] container MojoMultiMap by reference
 \param[in] key The key of which we want to visit all values
 \param[out] value_variable Existing variable that will receive each value for the key in turn
 */
#define MojoForEachMultiValue( container, key, value_variable ) \
for( int _i = ( container )._GetFirstIndexOf( key ); \
    ( container )._IsIndexValidOf( key, _i ) ? ( value_variable = ( container )._GetValueAt( _i ), true ) : false; \
    _i = ( container )._GetNextIndexOf( key, _i ) )
