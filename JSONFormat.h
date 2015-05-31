/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_JSONFormat_h
#define _FTL_JSONFormat_h

FTL_NAMESPACE_BEGIN

struct JSONFormat
{
  StrRef indentStr;
  StrRef memberSepStr;
  StrRef elementSepStr;
  StrRef objectBeginStr;
  StrRef objectEndStr;
  StrRef arrayBeginStr;
  StrRef arrayEndStr;
  StrRef newlineStr;

  static JSONFormat const &Pretty()
  {
    static JSONFormat format(
      FTL_STR("  "), // indentStr
      FTL_STR(" : "), // memberSepStr
      FTL_STR(","), // elementSepStr
      FTL_STR("{"), // objectBeginStr
      FTL_STR("}"), // objectEndStr
      FTL_STR("["), // arrayBeginStr
      FTL_STR("]"), // arrayEndStr
      FTL_STR("\n") // newlineStr
      );
    return format;
  }

  static JSONFormat const &Packed()
  {
    static JSONFormat format(
      StrRef(), // indentStr
      FTL_STR(":"), // memberSepStr
      FTL_STR(","), // elementSepStr
      FTL_STR("{"), // objectBeginStr
      FTL_STR("}"), // objectEndStr
      FTL_STR("["), // arrayBeginStr
      FTL_STR("]"), // arrayEndStr
      StrRef() // newlineStr
      );
    return format;
  }

protected:

  JSONFormat(
    StrRef theIndentStr,
    StrRef theMemberSepStr,
    StrRef theElementSepStr,
    StrRef theObjectBeginStr,
    StrRef theObjectEndStr,
    StrRef theArrayBeginStr,
    StrRef theArrayEndStr,
    StrRef theNewlineStr
    )
    : indentStr( theIndentStr )
    , memberSepStr( theMemberSepStr )
    , elementSepStr( theElementSepStr )
    , objectBeginStr( theObjectBeginStr )
    , objectEndStr( theObjectEndStr )
    , arrayBeginStr( theArrayBeginStr )
    , arrayEndStr( theArrayEndStr )
    , newlineStr( theNewlineStr )
    {}
};

FTL_NAMESPACE_END

#endif //_FTL_JSONFormat_h
