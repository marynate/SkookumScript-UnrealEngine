//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2014 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript Unreal Engine Helper Utilities
// 
// # Author(s):  Conan Reis
//=======================================================================================


#ifndef __SKUEUTILS_HPP
#define __SKUEUTILS_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include "Engine/World.h"
#include "SharedPointer.h"


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Helper class to hold a pointer to a component 
// and make sure it gets destroyed when the pointer goes away
template <class _ComponentClass>
class TWeakComponentPtr : public TWeakObjectPtr<_ComponentClass>
  {
  public:

    TWeakComponentPtr(const _ComponentClass * component_p) : TWeakObjectPtr<_ComponentClass>(component_p) {}
    ~TWeakComponentPtr()
      {
      _ComponentClass * component_p = Get();

      if (component_p)
        {
        component_p->ConditionalBeginDestroy();
        }
      }
  };

//---------------------------------------------------------------------------------------
// Storage specialization - TWeakComponentPtr<UParticleSystemComponent> stored indirectly as pointer in SkUserData rather than whole structure
template<> inline TWeakComponentPtr<UParticleSystemComponent> * SkUserDataBase::as<TWeakComponentPtr<UParticleSystemComponent>>() const { return as_stored<TWeakComponentPtr<UParticleSystemComponent>>(); }

//---------------------------------------------------------------------------------------
// Helper class for delegate callbacks
// $Revisit - Should look at this again in the future
class SkUERawICoroutineResume : public SkInvokedCoroutine
  {
  public:

    void on_audio_finished(UAudioComponent *)  { resume(); }
    
  };


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

//---------------------------------------------------------------------------------------
// Gets player pawn from first player controller.
//APawn * get_player_pawn();




#endif  // __SKUEUTILS_HPP

