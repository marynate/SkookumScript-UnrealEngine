//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Actor Component
// 
// Author: Conan Reis, Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include "Components/ActorComponent.h"
#include "SkookumScriptComponent.generated.h"


//=======================================================================================
// Global Defines / Macros
//=======================================================================================


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Adds SkookumScript text-based scripting capabilities to an actor.
UCLASS(classGroup=Scripting, editinlinenew, BlueprintType, meta=(BlueprintSpawnableComponent), hideCategories=(Object, ActorComponent), EarlyAccessPreview)
class SKOOKUMSCRIPTRUNTIME_API USkookumScriptComponent : public UActorComponent
  {

    GENERATED_UCLASS_BODY()

  public:

  // Public Data Members

    // SkookumScript class type - used to create appropriate Skookum object instance
    // Uses most derived parent class if left blank.
    UPROPERTY(Category = Script, EditAnywhere, BlueprintReadOnly)
    FString ScriptClassName;


  // Methods

    // Gets our SkookumScript instance
    SkInstance * get_instance() const { return m_instance_p; }

    // Calls a script coroutine on this actor (no arguments)
    // Coroutines are durational and used to do time-based / concurrent tasks that can take more than one frame to complete.
    // @param name name of the coroutine 
    UFUNCTION(BlueprintCallable, Category = "Script|Functions")
    virtual void invoke_coroutine(FString name);

    // Calls a script method on this actor (no arguments)
    // Methods complete immediately.
    // @param name name of the method to call
    UFUNCTION(BlueprintCallable, Category = "Script|Functions")
    virtual void invoke_method(FString method_name);

    // Calls a script query/predicate method on this actor (no arguments) and returns true or false
    // @param name name of the query method to call
    UFUNCTION(BlueprintCallable, Category = "Script|Functions")
    virtual bool invoke_query(FString query_name);

    // Begin UActorComponent interface

      virtual void OnRegister() override;
      virtual void InitializeComponent() override;
      virtual void UninitializeComponent() override;
      virtual void OnUnregister() override;

  protected:

  // Internal Data Members

    // Keep the SkookumScript instance belonging to this actor around
    AIdPtr<SkInstance> m_instance_p;

  };  // USkookumScriptComponent


//=======================================================================================
// Inline Functions
//=======================================================================================

