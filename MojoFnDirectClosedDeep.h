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

#include "MojoConstants.h"
#include "MojoAbstractSet.h"
#include "MojoCollector.h"
#include "MojoUtil.h"

/** \cond HIDE_FORWARD_REFERENCE */
template< typename key_T > class MojoRelation;
/** \endcond */

/**
 \class MojoFnDirectClosedDeep
 \ingroup group_function
 Contains all parents of the keys in the input set, and recursively all parents of keys in the *output* set.
 \image html Func-Direct-Closed-Deep.png
 \note Enumeration may result in duplicate entries. Enumeration into a MojoSetCollector is recommended. This behavior is
 due to the reluctance of the author to use memory allocation for an intermediate result.
 */
template< typename key_T >
class MojoFnDirectClosedDeep final : public MojoAbstractSet< key_T >
{
public:

private:
  class Collector final : public MojoCollector< key_T >
  {
  public:
    Collector( const MojoCollector< key_T >& collector, const MojoRelation< key_T >* relation,
              const MojoAbstractSet< key_T >* limit )
    : m_Collector( collector )
    , m_Relation( relation )
    , m_Limit( limit )
    {}
    
    virtual void Push( const key_T& key ) const override
    {
      key_T parent = m_Relation->FindParent( key );
      if( !parent.IsHashNull() )
      {
        while( !parent.IsHashNull() )
        {
          if( !m_Limit || m_Limit->Contains( parent ) )
          {
            m_Collector.Push( parent );
          }
          parent = m_Relation->FindParent( parent );
        }
      }
      else if( !m_Limit || m_Limit->Contains( key ) )
      {
        m_Collector.Push( key );
      }
    }
  private:
    const MojoCollector< key_T >& m_Collector;
    const MojoRelation< key_T >*  m_Relation;
    const MojoAbstractSet< key_T >*     m_Limit;
  };
  
public:
  /**
   Construct from a MojoRelation and a MojoAbstractSet object.
   \param[in] relation The relation that defines the function.
   \param[in] set The set to be transformed by the function.
   */
  MojoFnDirectClosedDeep( const MojoRelation< key_T>* relation, const MojoAbstractSet< key_T >* set )
  : m_Relation( relation )
  , m_Set( set )
  {}
  
  virtual bool Contains( const key_T& key ) const override
  {
    // Key is a parent in the relation.
    key_T child;
    MojoForEachChildOfParent( *m_Relation, key, child )
    {
      if( m_Set->Contains( child ) )
      {
        return true;
      }
      if( Contains( child ) )
      {
        return true;
      }
    }
    return !m_Relation->Contains( key ) && m_Set->Contains( key );
  }
  
  virtual void Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override
  {
    m_Set->Enumerate( Collector( collector, m_Relation, limit ) );
  }
  
  /** \private */
  virtual int _GetEnumerationCost() const override
  {
    return m_Set->_GetEnumerationCost();
  }
  
  /** \private */
  virtual int _GetChangeCount() const override
  {
    return m_Set->_GetChangeCount() + m_Relation->_GetChangeCount();
  }
  
private:
  
  const MojoRelation< key_T >*  m_Relation;
  const MojoAbstractSet< key_T >*     m_Set;
};

// ---------------------------------------------------------------------------------------------------------------------
