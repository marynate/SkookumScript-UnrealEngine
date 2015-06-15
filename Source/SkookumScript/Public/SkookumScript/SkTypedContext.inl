//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Class Type Scope Context
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================


//=======================================================================================
// SkTypeContext Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Default Constructor
// Author(s):   Conan Reis
A_INLINE SkTypeContext::SkTypeContext() :
  m_obj_scope_p(nullptr),
  m_params_p(nullptr),
  m_current_scope_p(&m_top_scope),
  m_current_vars_p(&m_top_scope.m_vars),
  m_capture_current_p(nullptr)
  {
  m_scope_stack.append(&m_top_scope);
  }

//---------------------------------------------------------------------------------------
// Destructor
// Author(s):   Conan Reis
A_INLINE SkTypeContext::~SkTypeContext()
  {
  m_scope_stack.remove(&m_top_scope);
  m_scope_stack.free_all();
  m_capture_stack.free_all();
  }

//---------------------------------------------------------------------------------------
// Appends a local typed variable name to the context.
// Arg         tname_p - typed variable name to add
// See:        free_all_locals(), empty_locals()
// Author(s):   Conan Reis
A_INLINE void SkTypeContext::append_local(
  const ASymbol &   var_name,
  SkClassDescBase * type_p
  )
  {
  // $Revisit - CReis [Memory] These structures should come from a memory pool
  m_current_vars_p->append(*SK_NEW(SkTypedName)(var_name, type_p));
  }

//---------------------------------------------------------------------------------------
// Frees (deletes) the typed local variables from the current scope and all
//             the old local history.
// Arg         var_names - names of variables to free
// See:        free_all_locals(), empty_locals()
// Author(s):   Conan Reis
A_INLINE void SkTypeContext::free_all_locals()
  {
  m_top_scope.empty();
  m_scope_stack.remove(&m_top_scope);
  m_scope_stack.free_all();
  m_scope_stack.append(&m_top_scope);
  m_params_p = nullptr;
  }

//---------------------------------------------------------------------------------------
// Determines if the current scope has any variables.
//             [Yes, the method name is poor grammar.]
// Author(s):   Conan Reis
A_INLINE bool SkTypeContext::is_locals() const
  {
  return m_current_vars_p->is_filled();
  }

//---------------------------------------------------------------------------------------
// Appends a new scope nest level and makes it the current scope.
// See:        unnest_locals(), accept_nest(), change_variable_types, merge_locals()
// Author(s):   Conan Reis
A_INLINE void SkTypeContext::nest_locals()
  {
  // $Revisit - CReis [Memory] These structures should come from a memory pool
  ScopeVars * vars_p = SK_NEW(ScopeVars);

  m_current_scope_p = vars_p;
  m_current_vars_p = &vars_p->m_vars;
  m_scope_stack.append(vars_p);
  }

//---------------------------------------------------------------------------------------
// Removes a scope nest level and makes the previous nest level the current
//             scope.
// See:        nest_locals(), accept_nest(), change_variable_types, merge_locals()
// Author(s):   Conan Reis
A_INLINE void SkTypeContext::unnest_locals(
  eAHistory history // = AHistory_remember
  )
  {
  ScopeVars * old_vars_p = m_scope_stack.pop_last();
  ScopeVars * vars_p     = m_scope_stack.get_last();

  m_current_scope_p = vars_p;
  m_current_vars_p = &vars_p->m_vars;

  if (history && old_vars_p->m_var_history.is_filled())
    {
    // Remember past variables
    vars_p->m_var_history.xfer_absent_all_free_dupes(&old_vars_p->m_var_history);
    }

  delete old_vars_p;
  }
