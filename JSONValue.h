/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_JSONValue_h
#define _FTL_JSONValue_h

#include <FTL/OrderedStringMap.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONDec.h>
#include <FTL/JSONEnc.h>

FTL_NAMESPACE_BEGIN

class JSONValue
{
public:

  enum Type
  {
    Type_Null,
    Type_Boolean,
    Type_SInt32,
    Type_Float64,
    Type_String,
    Type_Array,
    Type_Object
  };

  static JSONValue *Create( JSONEnt const &je );

  static JSONValue *Decode( StrRef &jsonStr, uint32_t &jsonLine, uint32_t &jsonColumn );

  Type getType() const
    { return m_type; }

  bool isNull() const
    { return m_type == Type_Null; }
  bool isBoolean() const
    { return m_type == Type_Boolean; }
  bool isSInt32() const
    { return m_type == Type_SInt32; }
  bool isFloat64() const
    { return m_type == Type_Float64; }
  bool isString() const
    { return m_type == Type_String; }
  bool isArray() const
    { return m_type == Type_Array; }
  bool isObject() const
    { return m_type == Type_Object; }

  virtual ~JSONValue() {}

  template<typename JSONValueTy>
  JSONValueTy *maybeCast()
  {
    if ( JSONValueTy::classof( this ) )
      return static_cast<JSONValueTy *>( this );
    else
      return 0;
  }

  template<typename JSONValueTy>
  JSONValueTy *cast()
  {
    JSONValueTy *result = maybeCast<JSONValueTy>();
    if ( !result )
      throw JSONInvalidCastException( JSONValueTy::NotAStr() );
    return result;
  }

  template<typename JSONValueTy>
  JSONValueTy const *maybeCast() const
  {
    if ( JSONValueTy::classof( this ) )
      return static_cast<JSONValueTy const *>( this );
    else
      return 0;
  }

  template<typename JSONValueTy>
  JSONValueTy const *cast() const
  {
    JSONValueTy const *result = maybeCast<JSONValueTy>();
    if ( !result )
      throw JSONInvalidCastException( JSONValueTy::NotAStr() );
    return result;
  }

  virtual void encode( JSONEnc<std::string> &je ) const = 0;

  void encode( std::string &string ) const
  {
    JSONEnc<std::string> je( string );
    encode( je );
  }

protected:

  JSONValue( Type type )
    : m_type( type ) {}

private:

  Type m_type;
};

class JSONNull : public JSONValue
{
public:

  static bool classof( JSONValue const *jsonValue )
    { return jsonValue->getType() == Type_Null; }

  static StrRef NotAStr()
    { return FTL_STR("not a null"); }

  JSONNull()
    : JSONValue( Type_Null ) {}

  virtual void encode( JSONEnc<std::string> &enc ) const
    { JSONNullEnc<std::string> nullEnc( enc ); }
};

class JSONBoolean : public JSONValue
{
public:

  static bool classof( JSONValue const *jsonValue )
    { return jsonValue->getType() == Type_Boolean; }

  static StrRef NotAStr()
    { return FTL_STR("not a boolean"); }

  JSONBoolean( bool value )
    : JSONValue( Type_Boolean )
    , m_value( value ) {}

  JSONBoolean()
    : JSONValue( Type_Boolean ) {}

  bool getValue() const
    { return m_value; }

  void setValue( bool value )
    { m_value = value; }

  virtual void encode( JSONEnc<std::string> &enc ) const
    { JSONBooleanEnc<std::string> booleanEnc( enc, m_value ); }

private:

  bool m_value;
};

class JSONSInt32 : public JSONValue
{
public:

  static bool classof( JSONValue const *jsonValue )
    { return jsonValue->getType() == Type_SInt32; }

  static StrRef NotAStr()
    { return FTL_STR("not an integer"); }

  JSONSInt32( int32_t value )
    : JSONValue( Type_SInt32 )
    , m_value( value ) {}

  JSONSInt32()
    : JSONValue( Type_SInt32 ) {}

  int32_t getValue() const
    { return m_value; }

  void setValue( int32_t value )
    { m_value = value; }

  virtual void encode( JSONEnc<std::string> &enc ) const
    { JSONSInt32Enc<std::string> sint32Enc( enc, m_value ); }

private:

  int32_t m_value;
};

class JSONFloat64 : public JSONValue
{
public:

  static bool classof( JSONValue const *jsonValue )
    { return jsonValue->getType() == Type_Float64; }

  static StrRef NotAStr()
    { return FTL_STR("not a scalar"); }

  JSONFloat64()
    : JSONValue( Type_Float64 ) {}

  JSONFloat64( double value )
    : JSONValue( Type_Float64 )
    , m_value( value ) {}

  double getValue() const
    { return m_value; }

  void setValue( double value )
    { m_value = value; }

  virtual void encode( JSONEnc<std::string> &enc ) const
    { JSONFloat64Enc<std::string> float64Enc( enc, m_value ); }

private:

  double m_value;
};

class JSONString : public JSONValue
{
public:

  static bool classof( JSONValue const *jsonValue )
    { return jsonValue->getType() == Type_String; }

  static StrRef NotAStr()
    { return FTL_STR("not a string"); }

  JSONString()
    : JSONValue( Type_String ) {}

  JSONString( StrRef value )
    : JSONValue( Type_String )
    , m_value( value ) {}

  static JSONString *CreateWithSwap( std::string &value )
  {
    JSONString *result = new JSONString;
    result->m_value.swap( value );
    return result;
  }

  StrRef getValue() const
    { return m_value; }

  void setValue( StrRef value )
    { m_value = value; }

  virtual void encode( JSONEnc<std::string> &enc ) const
    { JSONStringEnc<std::string> stringEnc( enc, StrRef( m_value ) ); }

private:

  std::string m_value;
};

class JSONArray : public JSONValue
{
  typedef std::vector<JSONValue *> Vec;

public:

  static bool classof( JSONValue const *jsonValue )
    { return jsonValue->getType() == Type_Array; }

  static StrRef NotAStr()
    { return FTL_STR("not an array"); }

  JSONArray()
    : JSONValue( Type_Array ) {}

  ~JSONArray()
    { clear(); }

  virtual void encode( JSONEnc<std::string> &enc ) const
  {
    JSONArrayEnc<std::string> arrayEnc( enc );
    for ( Vec::const_iterator it = m_vec.begin(); it != m_vec.end(); ++it )
    {
      JSONValue const *value = *it;
      JSONEnc<std::string> elementEnc( arrayEnc );
      value->encode( elementEnc );
    }
  }

  bool empty() const
    { return m_vec.empty(); }

  size_t size() const
    { return m_vec.size(); }

  JSONValue *operator[]( size_t index ) const
  {
    if ( index < m_vec.size() )
      return m_vec[index];
    else
      throw JSONInvalidIndexException( index );
  }

  typedef Vec::const_iterator const_iterator;

  const_iterator begin() const
    { return m_vec.begin(); }

  const_iterator end() const
    { return m_vec.end(); }

  void clear()
  {
    for ( Vec::const_iterator it = m_vec.begin(); it != m_vec.end(); ++it )
      delete *it;
    m_vec.clear();
  }

  void reserve( size_t size )
    { m_vec.reserve( size ); }

  void push_back( JSONValue *jsonValue )
    { m_vec.push_back( jsonValue ); }

private:

  Vec m_vec;
};

class JSONObject : public JSONValue
{
  typedef OrderedStringMap<JSONValue *> Map;

public:

  static bool classof( JSONValue const *jsonValue )
    { return jsonValue->getType() == Type_Object; }

  static StrRef NotAStr()
    { return FTL_STR("not an object"); }

  static JSONObject *Decode(
    StrRef &jsonStr, uint32_t &jsonLine, uint32_t &jsonColumn
    )
  {
    JSONValue *jsonValue = JSONValue::Decode( jsonStr, jsonLine, jsonColumn );
    return jsonValue->cast<JSONObject>();
  }

  JSONObject()
    : JSONValue( Type_Object ) {}

  ~JSONObject()
    { clear(); }

  virtual void encode( JSONEnc<std::string> &enc ) const
  {
    JSONObjectEnc<std::string> objectEnc( enc );
    for ( Map::const_iterator it = m_map.begin(); it != m_map.end(); ++it )
    {
      StrRef key = it->first;
      JSONValue const *value = it->second;
      JSONEnc<std::string> memberEnc( objectEnc, key );
      value->encode( memberEnc );
    }
  }

  bool empty() const
    { return m_map.empty(); }

  size_t size() const
    { return m_map.size(); }

  size_t has( StrRef key ) const
    { return m_map.has( key ); }

  typedef Map::const_iterator const_iterator;

  const_iterator find( StrRef key ) const
    { return m_map.find( key ); }

  const_iterator begin() const
    { return m_map.begin(); }

  const_iterator end() const
    { return m_map.end(); }

  void clear()
  {
    for ( Map::const_iterator it = m_map.begin(); it != m_map.end(); ++it )
      delete it->second;
    m_map.clear();
  }

  bool insertTakingKey( std::string &key, JSONValue *value )
  {
    return m_map.insertTakingKey( key, value );
  }

  bool insert( StrRef key, JSONValue *value )
  {
    std::string keyString = key;
    return insertTakingKey( keyString, value );
  }

  JSONValue const *maybeGet( StrRef key ) const
  {
    Map::const_iterator it = find( key );
    return it != end()? it->second: 0;
  }

  JSONValue const *get( StrRef key ) const
  {
    JSONValue const *result = maybeGet( key );
    if ( !result )
      throw JSONKeyNotFoundException( key );
    return result;
  }
    
  bool maybeGetString( StrRef key, StrRef &value ) const
  {
    JSONValue const *jsonValue = maybeGet( key );
    if ( jsonValue )
      value = jsonValue->cast<JSONString>()->getValue();
    return !!jsonValue;
  }

  StrRef getString( StrRef key ) const
  {
    JSONValue const *jsonValue = get( key );
    StrRef result = jsonValue->cast<JSONString>()->getValue();
    return result;
  }

private:

  Map m_map;
};

inline JSONValue *JSONValue::Create( JSONEnt const &je )
{
  switch ( je.getType() )
  {
    case JSONEnt::Type_Null:
      return new JSONNull();

    case JSONEnt::Type_Boolean:
      return new JSONBoolean( je.booleanValue() );

    case JSONEnt::Type_Int32:
      return new JSONSInt32( je.int32Value() );

    case JSONEnt::Type_Float64:
      return new JSONFloat64( je.float64Value() );

    case JSONEnt::Type_String:
    {
      std::string string;
      je.stringAppendTo( string ); 
      return JSONString::CreateWithSwap( string );
    }

    case JSONEnt::Type_Object:
    {
      OwnedPtr<JSONObject> object( new JSONObject() );

      StrRef str = je.getRawStr();
      uint32_t line = je.getLine();
      uint32_t column = je.getColumn();
      JSONObjectDec jod( str, line, column );
      JSONEnt keyJE, valueJE;
      while ( jod.getNext( keyJE, valueJE ) )
      {
        if ( !keyJE.isString() )
          throw JSONInternalErrorException();
        std::string key;
        keyJE.stringAppendTo( key );
        JSONValue *value = Create( valueJE );
        if ( !object->insertTakingKey( key, value ) )
        {
          delete value;
          throw JSONMalformedException(
            keyJE.getLine(),
            keyJE.getColumn(),
            FTL_STR("duplicate key")
            );
        }
      }

      return object.take();
    }

    case JSONEnt::Type_Array:
    {
      OwnedPtr<JSONArray> array( new JSONArray() );
      array->reserve( je.arraySize() );

      StrRef str = je.getRawStr();
      uint32_t line = je.getLine();
      uint32_t column = je.getColumn();
      JSONArrayDec jad( str, line, column );
      JSONEnt elementJE;
      while ( jad.getNext( elementJE ) )
        array->push_back( Create( elementJE ) );

      return array.take();
    }

    default:
      throw JSONInternalErrorException();
      break;
  }
}

inline JSONValue *JSONValue::Decode(
  StrRef &jsonStr, uint32_t &jsonLine, uint32_t &jsonColumn
  )
{
  JSONDec jd( jsonStr, jsonLine, jsonColumn );
  JSONEnt je;
  OwnedPtr<JSONValue> result;
  if ( jd.getNext( je ) )
    result = Create( je );
  return result.take();
}

FTL_NAMESPACE_END

#endif //_FTL_JSONValue_h
