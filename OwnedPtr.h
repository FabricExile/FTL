/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_OwnedPtr_h
#define _FTL_OwnedPtr_h

#include <FTL/Config.h>

#include <algorithm>

FTL_NAMESPACE_BEGIN

template<typename Ty>
class OwnedPtr
{
  void init( Ty *ptr )
  {
    m_ptr = ptr;
  }

  void steal( Ty *&ptr )
  {
    m_ptr = ptr;
    ptr = 0;
  }

  void cleanup()
  {
    delete m_ptr;
  }      

  OwnedPtr( OwnedPtr<Ty> const & );

  OwnedPtr &operator =( OwnedPtr<Ty> const & );

public:

  OwnedPtr()
    { init( 0 ); }

  OwnedPtr( Ty *ptr )
    { init( ptr ); }

  template<typename OtherTy>
  OwnedPtr( OtherTy *ptr )
    { init( ptr ); }

  void reset( Ty *ptr )
  {
    if ( m_ptr != ptr )
    {
      cleanup();
      init( ptr );
    }
  }

  OwnedPtr &operator =( Ty *ptr )
  {
    reset( ptr );
    return *this;
  }

  template<typename OtherTy>
  void reset( OtherTy *ptr )
  {
    cleanup();
    init( ptr );
  }

  template<typename OtherTy>
  OwnedPtr &operator =( OtherTy *ptr )
  {
    reset( ptr );
    return *this;
  }

  Ty *get()
    { return m_ptr; }

  Ty const *get() const
    { return m_ptr; }

  Ty *operator ->()
    { return get(); }

  Ty const *operator ->() const
    { return get(); }

  Ty &operator *()
    { return *get(); }

  Ty const &operator *() const
    { return *get(); }

  Ty *take()
  {
    Ty *result = m_ptr;
    m_ptr = 0;
    return result;
  }

  ~OwnedPtr()
    { cleanup(); }

  operator bool() const
    { return get() != 0; }

  bool operator !() const
    { return !get(); }

  void swap( OwnedPtr<Ty> &that )
    { std::swap( m_ptr, that.m_ptr ); }

private:

  Ty *m_ptr;
};

FTL_NAMESPACE_END

#endif //_FTL_OwnedPtr_h
