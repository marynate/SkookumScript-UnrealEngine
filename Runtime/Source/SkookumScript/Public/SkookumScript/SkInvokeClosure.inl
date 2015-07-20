//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Invoked closure classes: SkInvokeClosureBase, SkInvokeClosureMethod and
//             SkInvokeClosureCoroutine
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================


//=======================================================================================
// SkInvokeClosureBase Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
//
// #Author(s) Conan Reis
A_INLINE SkInvokeClosureBase::SkInvokeClosureBase(
  // expression that is the 'target' of this invocation.  If nullptr is given then 'this'
  // - i.e. the topmost scope - is inferred.
  SkExpressionBase * receiver_p // = nullptr
  ) :
  m_receiver_p(receiver_p)
  {
  }

//---------------------------------------------------------------------------------------
// Transfer contents constructor.
//
// #Author(s) Conan Reis
A_INLINE SkInvokeClosureBase::SkInvokeClosureBase(
  SkExpressionBase * receiver_p,
  APCompactArray<SkExpressionBase> * send_args_p,
  APCompactArray<SkIdentifier> * return_args_p
  ) :
  m_receiver_p(receiver_p),
  m_arguments(send_args_p),
  m_return_args(return_args_p)
  {
  }

//=======================================================================================
// SkInvokeClosureCoroutine Inline Method Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Determines if expression completes immediately (true) - i.e. completes in 1 frame - or
// that its completion may be durational (false) - i.e. may take 1 or more frames.
//
// #Notes
//   Coroutines are durational, some expressions such as a code block may be immediate or
//   durational depending on its sub-expressions and some expressions such as identifiers
//   are always immediate.
//
// #Modifiers virtual
// #Author(s) Conan Reis
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  A_INLINE
  // true if immediate and false if durational
  bool
SkInvokeClosureCoroutine::is_immediate(
  // Address to store the character index position of the first durational expression if
  // this expression is durational - either this expression or a sub-expression that it
  // contains.
  uint32_t * durational_idx_p // = nullptr
  ) const
  {
  #if (SKOOKUM & SK_DEBUG)
    if (durational_idx_p)
      {
      *durational_idx_p = m_source_idx;
      }
  #endif

  // Concurrent expressions may take more than one frame to execute and return.
  return false;
  }
