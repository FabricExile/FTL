/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

template<class ShareableTy>
class SharedPtr;

class Shareable
{
  template<class ShareableTy>
  friend class SharedPtr;

  Shareable( Shareable const &that );
  Shareable &operator=( Shareable const &that );

protected:

  Shareable()
    : m_refCount( 0 ) {}
  
  virtual ~Shareable()
    {}

  void retain() const
    { ++m_refCount; }

  void release() const
    { if ( --m_refCount == 0 ) delete this; }

private:

  mutable size_t m_refCount;
};

template<class ShareableTy>
class SharedPtr
{
  template<class OtherShareableTy>
  friend class SharedPtr;

  void init( ShareableTy *shareable )
  {
    m_shareable = shareable;
    if ( m_shareable )
      m_shareable->retain();
  }

  void steal( ShareableTy *&shareable )
  {
    m_shareable = shareable;
    shareable = 0;
  }

  template<class OtherShareableTy>
  void steal( OtherShareableTy *&shareable )
  {
    m_shareable = shareable;
    shareable = 0;
  }

  void cleanup()
  {
    if ( m_shareable )
    {
      m_shareable->release();
#if defined(FTL_BUILD_DEBUG)
      m_shareable = (ShareableTy *)0xDEADBEEF;
#endif
    }
  }

  void assign( ShareableTy *shareable )
  {
    if ( m_shareable != shareable )
    {
      cleanup();
      init( shareable );
    }
  }

public:

  SharedPtr()
  {
    init( 0 );
  }

  SharedPtr( ShareableTy *shareable )
  {
    init( shareable );
  }

  template<class OtherShareableTy>
  SharedPtr( OtherShareableTy *shareable )
  {
    init( shareable );
  }

  SharedPtr( SharedPtr<ShareableTy> const &that )
  {
    init( that.m_shareable );
  }

  template<class OtherShareableTy>
  SharedPtr( SharedPtr<OtherShareableTy> const &that )
  {
    init( that.m_shareable );
  }

  ~SharedPtr()
  {
    cleanup();
  }

  SharedPtr<ShareableTy> &operator =( SharedPtr<ShareableTy> const &that )
  {
    assign( that.m_shareable );
    return *this;
  }

  template<class OtherShareableTy>
  SharedPtr<ShareableTy> &operator =( SharedPtr<OtherShareableTy> const &that )
  {
    assign( that.m_shareable );
    return *this;
  }

  bool isNull() const
  {
    return m_shareable == 0;
  }

  operator bool() const
  {
    return !isNull();
  }

  bool operator!() const
  {
    return isNull();
  }

  ShareableTy *get() const
    { return m_shareable; }

  ShareableTy *operator ->() const
    { return m_shareable; }

  ShareableTy &operator *() const
    { return *m_shareable; }

  bool operator ==( SharedPtr<ShareableTy> const &that ) const
  {
    return m_shareable == that.m_shareable;
  }

  bool operator !=( SharedPtr<ShareableTy> const &that ) const
  {
    return m_shareable != that.m_shareable;
  }

  bool operator <( SharedPtr<ShareableTy> const &that ) const
  {
    return m_shareable < that.m_shareable;
  }

  template<class OtherShareableTy>
  static SharedPtr<ShareableTy> StaticCast( SharedPtr<OtherShareableTy> const &that )
  {
    ShareableTy *shareable = static_cast<ShareableTy *>( that.m_shareable );
    return SharedPtr<ShareableTy>( shareable );
  }

private:

  ShareableTy *m_shareable;
};

FTL_NAMESPACE_END
