//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Coroutine parameters & body classes
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKCOROUTINE_HPP
#define __SKCOROUTINE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkInvokableBase.hpp>
#include <SkookumScript/SkInstance.hpp>         // Used by tSkCoroutineMthd to determine size of method pointer


//=======================================================================================
// Global Structures
//=======================================================================================

class SkInvokedCoroutine;  // Pre-declaration


//---------------------------------------------------------------------------------------
// Notes      Method parameters & body abstract class
// Subclasses SkCoroutine, SkCoroutineMthd, SkCoroutineFunc
// Author(s)  Conan Reis
class SkCoroutineBase : public SkInvokableBase
  {
  public:

  // Common Methods

    SkCoroutineBase(const ASymbol & name, SkClass * scope_p) : SkInvokableBase(name, scope_p) {}
    SkCoroutineBase(const ASymbol & name, SkClass * scope_p, SkParameters * params_p) : SkInvokableBase(name, scope_p, params_p) {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkCoroutineBase(const ASymbol & name, SkClass * scope_p, const void ** binary_pp);
      virtual void assign_binary(const void ** binary_pp);
    #endif

    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif

    #if defined(SK_AS_STRINGS)
      virtual AString as_code_params() const;
    #endif

  // Methods

    virtual bool is_class_member() const  { return false; }
    virtual bool on_update(SkInvokedCoroutine * data_p) const = 0;
    virtual void track_memory(AMemoryStats * mem_stats_p) const = 0;


  // Data Members

    // Future: Specification and description of data used by this coroutine
    // Future: Description of coroutine for tools/debugging
    // Future: Bit field/list of incompatible/conflicting coroutines that this coroutine cannot run concurrently with (i.e. to movement related coroutines would likely conflict)

  };  // SkCoroutineBase


//---------------------------------------------------------------------------------------
// Notes      Custom/Compound Coroutine Class
// Author(s)  Conan Reis
class SkCoroutine : public SkCoroutineBase
  {
  friend class SkParser;

  public:
	  SK_NEW_OPERATORS(SkCoroutine);
  // Common Methods

    SkCoroutine(const ASymbol & name, SkClass * scope_p) : SkCoroutineBase(name, scope_p), m_expr_p(nullptr) {}
    SkCoroutine(const ASymbol & name, SkClass * scope_p, SkParameters * params_p, SkExpressionBase * body_p = nullptr) : SkCoroutineBase(name, scope_p, params_p), m_expr_p(body_p) {}
    virtual ~SkCoroutine();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkCoroutine(const ASymbol & name, SkClass * scope_p, const void ** binary_pp);
      virtual void assign_binary(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * get_custom_expr() const;
    #endif


  // Methods

    SkCoroutine & assign_take(SkCoroutine * coroutine_p);
    virtual bool  on_update(SkInvokedCoroutine * data_p) const;
    void          set_expression(SkExpressionBase * expr_p);

    // Overridden from SkMethodBase

      virtual void    track_memory(AMemoryStats * mem_stats_p) const;

    // Overridden from SkInvokableBase

      virtual eSkInvokable get_invoke_type() const;
      virtual bool         is_bound() const;

  protected:

  // Data Members

    // Optimized code block containing the statements to execute when this coroutine is
    // invoked.  It is either a SkCode, SkLiteral(nil), or some other expression.
    SkExpressionBase * m_expr_p;

  };  // SkCoroutine

//---------------------------------------------------------------------------------------
// Typedef for pointer to C++ coroutine method - which must be a member of a class
// derived from SkookumScript/SkInstance.
// It is associated with a Skookum coroutine and called every `on_update()` when in an
// Actor's list of invoked coroutines.
// 
// Returns:  
//   Boolean indicating whether the coroutine has completed.  `false` if the invoked
//   coroutine should be called again in the next update, `true` if the coroutine has
//   completed and it should be removed from the Actor's invoked coroutines list.
//   
// Params:  
//   scope_p:
//     Calling scope and working data for the invoked coroutine - this is the access point
//     for run-time information, passed arguments, data members, the call stack, etc.
//     - see `SkInvokedCoroutine` for a description of its methods and data members.
//     
// See:   Coroutine::on_update(), tSkCoroutineFunc, tSkMethodFunc, tSkMethodMthd
typedef bool (SkInstance::* tSkCoroutineMthd)(SkInvokedCoroutine * scope_p);

// Macro for casting to tSkCoroutineMthd
#define SKCOROUTINE_MTHD(_mthd) static_cast<tSkCoroutineMthd>(_mthd)
//#define SKCOROUTINE_MTHD(_mthd) reinterpret_cast<tSkCoroutineMthd>(_mthd)


//---------------------------------------------------------------------------------------
// Notes      Atomic Coroutine (C++) Method Class
// Author(s)  Conan Reis
class SkCoroutineMthd : public SkCoroutineBase
  {
  public:
	  SK_NEW_OPERATORS(SkCoroutineMthd);

  // Public Data Members

    //uint32_t _align32;  // $Note - CReis [Memory] Alignment of pointer to member is finicky if the owner class is not yet declared.

    // AMethod to call when this coroutine is running/being updated
    tSkCoroutineMthd m_update_m;


  // Common Methods

    SkCoroutineMthd(const ASymbol & name, SkClass * scope_p, SkParameters * params_p, tSkCoroutineMthd update_m = nullptr);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkCoroutineMthd(const ASymbol & name, SkClass * scope_p, const void ** binary_pp);
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkInvokable get_invoke_type() const;
    virtual bool         is_bound() const;
    virtual bool         on_update(SkInvokedCoroutine * data_p) const;
    void                 set_update(tSkCoroutineMthd update_m);
    virtual void         track_memory(AMemoryStats * mem_stats_p) const;

  };  // SkCoroutineMthd


//---------------------------------------------------------------------------------------
// Typedef for pointer to C++ coroutine function - which must be a static or global
// function.
// It is associated with a Skookum coroutine and called every `on_update()` when in an
// Actor's list of invoked coroutines.
// 
// Returns:  
//   Boolean indicating whether the coroutine has completed.  `false` if the invoked
//   coroutine should be called again in the next update, `true` if the coroutine has
//   completed and it should be removed from the Actor's invoked coroutines list.
//   
// Params:  
//   scope_p:
//     Calling scope and working data for the invoked coroutine - this is the access point
//     for run-time information, passed arguments, data members, the call stack, etc.
//     - see `SkInvokedCoroutine` for a description of its methods and data members.
//     
// See:   Coroutine::on_update(), tSkCoroutineMthd, tSkMethodFunc, tSkMethodMthd
typedef bool (* tSkCoroutineFunc)(SkInvokedCoroutine * scope_p);


//---------------------------------------------------------------------------------------
// Notes      Atomic Coroutine Function Class
// Author(s)  Conan Reis
class SkCoroutineFunc : public SkCoroutineBase
  {
  public:
	  SK_NEW_OPERATORS(SkCoroutineFunc);

  // Public Data Members

    // AMethod to call when this coroutine is running/being updated
    tSkCoroutineFunc m_update_f;


  // Common Methods

    SkCoroutineFunc(const ASymbol & name, SkClass * scope_p, SkParameters * params_p, tSkCoroutineFunc update_f = nullptr);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkCoroutineFunc(const ASymbol & name, SkClass * scope_p, const void ** binary_pp);
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkInvokable get_invoke_type() const;
    virtual bool         is_bound() const;
    virtual bool         is_placeholder();
    virtual bool         on_update(SkInvokedCoroutine * data_p) const;
    void                 set_update(tSkCoroutineFunc update_f);
    virtual void         track_memory(AMemoryStats * mem_stats_p) const;

  // Atomic Methods

    static bool coro_placeholder(SkInvokedCoroutine * scope_p);

  };  // SkCoroutineFunc


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkCoroutine.inl>
#endif


#endif  // __SKCOROUTINE_HPP

