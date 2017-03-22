/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/StrRef.h>

#include <exception>
#include <stdint.h>
#include <stdio.h>
#include <string>

FTL_NAMESPACE_BEGIN

class JSONException : public std::exception
{
public:

  virtual ~JSONException() throw() {}

  StrRef getDesc() const
    { return m_desc; }
  char const *getDescCStr() const
    { return m_desc.c_str(); }

protected:

  JSONException() {}

  JSONException( StrRef desc )
    : m_desc( desc ) {}

  void append( StrRef str )
    { m_desc += str; }

  void appendSize( size_t value )
  {
    size_t oldSize = m_desc.size();
    m_desc.resize( oldSize + 24 );
    m_desc.resize( oldSize + snprintf(
      &m_desc[oldSize], 24, "%u", unsigned(value)
      ) );
  }

  virtual char const *what() const throw()
    { return m_desc.c_str(); }

private:

  std::string m_desc;
};

class JSONInvalidCastException : public JSONException
{
public:

  JSONInvalidCastException( StrRef desc )
    : JSONException( desc ) {}
};

class JSONInvalidIndexException : public JSONException
{
public:

  JSONInvalidIndexException( size_t index )
  {
    append( FTL_STR("index ") );
    appendSize( index );
    append( FTL_STR(" invalid") );
  }
};

class JSONKeyNotFoundException : public JSONException
{
public:

  JSONKeyNotFoundException( StrRef key )
  {
    append( FTL_STR("key '") );
    append( key );
    append( FTL_STR("' not found") );
  }
};

class JSONInternalErrorException : public JSONException
{
public:

  JSONInternalErrorException()
    : JSONException( FTL_STR("internal error") ) {}
};

class JSONEncodingErrorException : public JSONException
{
public:

  JSONEncodingErrorException()
    : JSONException( FTL_STR("encoding error") ) {}
};

class JSONMalformedException : public JSONException
{

public:

  JSONMalformedException(
    uint32_t line,
    uint32_t column,
    StrRef desc
    )
    : m_line( line )
    , m_column( column )
  {
    append( FTL_STR("line ") );
    appendSize( line );
    append( FTL_STR(", column ") );
    appendSize( column );
    append( FTL_STR(": ") );
    append( desc );
  }

  uint32_t getLine() const
    { return m_line; }
  uint32_t getColumn() const
    { return m_column; }

private:

  uint32_t m_line;
  uint32_t m_column;
};

FTL_NAMESPACE_END
