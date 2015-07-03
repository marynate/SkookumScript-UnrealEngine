//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Precompiled header file for all plugin CPP files
// 
// Author: Conan Reis
//=======================================================================================

#pragma once

#include "ISkookumScriptRuntime.h"
#include "Engine.h"
#include "CoreUObject.h"
#include "ModuleManager.h"

#include <AgogCore/AgogCore.hpp>
#include <AgogCore/AString.hpp>
#include <AgogCore/ASymbol.hpp>
#include <AgogCore/ADatum.hpp>
#include <AgogCore/ADeferFunc.hpp>
#include <AgogCore/ABinaryParse.hpp>
#include <AgogCore/AIdPtr.hpp>
#include <AgogCore/AObjReusePool.hpp>
#include <AgogCore/AMath.hpp>
#include <SkookumScript/SkClass.hpp>
#include <SkookumScript/SkBrain.hpp>
#include <SkookumScript/SkBoolean.hpp>
#include <SkookumScript/SkInteger.hpp>
#include <SkookumScript/SkReal.hpp>
#include <SkookumScript/SkString.hpp>
#include <SkookumScript/SkList.hpp>
#include <SkookumScript/SkSymbol.hpp>
#include <SkookumScript/SkUserData.hpp>
#include <SkookumScript/SkInstance.hpp>
#include <SkookumScript/SkInvokedMethod.hpp>
#include <SkookumScript/SkInvokedCoroutine.hpp>
#include <SkookumScript/SkMethod.hpp>
#include <SkookumScript/SkParameterBase.hpp>
#include <SkookumScript/SkClosure.hpp>
#include <SkookumScript/SkMind.hpp>
#include <SkookumScript/SkParser.hpp>
#include <SkookumScript/SkObject.hpp>
#include <SkookumScript/SkDebug.hpp>

#include "Bindings/SkUESymbol.hpp"
#include "Bindings/SkUEUtils.hpp"

#if WITH_EDITOR
  #include "UnrealEd.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogSkookum, Log, All);

