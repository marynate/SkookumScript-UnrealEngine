//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Unreal Engine Helper Utilities
// 
// Author:  Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

//=======================================================================================
// Global Functions
//=======================================================================================

//---------------------------------------------------------------------------------------
inline AString FStringToAString(const FString & str)
  {
  // $Revisit - CReis Look into StringCast<>  Engine\Source\Runtime\Core\Public\Containers\StringConv.h
  return AString(*str, str.Len());
  }

//---------------------------------------------------------------------------------------
inline ASymbol FStringToASymbol(const FString & str)
  {
  // $Revisit - CReis Look into StringCast<>
  return ASymbol::create(AString(*str, str.Len()));
  }

//---------------------------------------------------------------------------------------
inline FString AStringToFString(const AString & str)
  {
  // $Revisit - CReis Look into StringCast<>
  return FString(str.as_cstr());
  }

//---------------------------------------------------------------------------------------
// Converts `AString` to `FName` (similar to `ASymbol`). If this is the first time this
// string is encountered in the `FName` system it stores in a string table for later
// conversion.
// 
// Somewhat dangerous conversion since `FName` does case insensitive comparison and stores
// first string it encounters. So `length` will match an originally stored `Length`.
//
// # Author(s): Conan Reis
inline FName AStringToFName(const AString & str)
  {
  // $Revisit - CReis Look into StringCast<>
  return FName(str.as_cstr());
  }

//---------------------------------------------------------------------------------------
// Converts `AString` to `FName` (similar to `ASymbol`) if it already exist. If the string
// does not already exist as an `FName` then return `FNAME_Find`.
// 
// Somewhat dangerous conversion since `FName` does case insensitive comparison and stores
// first string it encounters. So `length` will match an originally stored `Length`.
//
// # Author(s): Conan Reis
inline FName AStringToExistingFName(const AString & str)  
  {
  // $Revisit - CReis Look into StringCast<>
  return FName(str.as_cstr(), FNAME_Find);
  }
