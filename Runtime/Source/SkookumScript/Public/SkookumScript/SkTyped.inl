//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Typed name and typed data classes
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkInstance.hpp>


//=======================================================================================
// SkTypedName Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Default constructor
// Returns:    itself
// Arg         name - name of the object (Default ASymbol::ms_null)
// Arg         type_p - class type to use.  The instance stored by this member must be of
//             this class or a subclass.
// Notes:      A SkTypedName may be coerced to or from a ASymbol if only the name is needed.
// Author(s):   Conan Reis
A_INLINE SkTypedName::SkTypedName(
  const ASymbol &         name,  // = ASymbol::ms_null
  const SkClassDescBase * type_p // = nullptr
  ) :
  ANamed(name),
  m_type_p(const_cast<SkClassDescBase *>(type_p))
  {
  }

//---------------------------------------------------------------------------------------
// Copy constructor
// Returns:    itself
// Arg         source - Typed name to copy
// Notes:      May be coerced to or from a ASymbol if only the name is needed.
// Author(s):   Conan Reis
A_INLINE SkTypedName::SkTypedName(const SkTypedName & source) :
  ANamed(source),
  m_type_p(source.m_type_p)
  {
  }

//---------------------------------------------------------------------------------------
// Destructor
// Author(s):   Conan Reis
A_INLINE SkTypedName::~SkTypedName()
  {
  // Defined here to ensure compiler knows about SkClass details
  }


//=======================================================================================
// SkTypedData Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Default constructor
// Returns:    itself
// Arg         name - name of the object (Default ASymbol::ms_null)
// Arg         type_p - class type to use.  The instance stored by this member must be of
//             this class or a subclass.
// Arg         instance_p - pointer to class instance to reference.  [Note that it does
//             not automatically have its reference count incremented.]
// Notes:      A SkTypedData may be coerced to or from a ASymbol if only the name is needed.
// Author(s):   Conan Reis
A_INLINE SkTypedData::SkTypedData(
  const ASymbol &         name,      // = ASymbol::ms_null
  const SkClassDescBase * type_p,    // = nullptr
  SkInstance *            instance_p // = nullptr
  ) :
  SkTypedName(name, type_p),
  m_data_p(instance_p)
  {
  }

//---------------------------------------------------------------------------------------
// Copy constructor
// Returns:    itself
// Arg         source - Typed name to copy
// Author(s):   Conan Reis
A_INLINE SkTypedData::SkTypedData(const SkTypedData & source) :
  SkTypedName(source),
  m_data_p(source.m_data_p)
  {
  }

 //---------------------------------------------------------------------------------------
// Data setter
// Author(s):   Conan Reis
A_INLINE void SkTypedData::set_data(SkInstance * data_p)
  {
  if (data_p) data_p->reference();
  if (m_data_p) m_data_p->dereference();
  m_data_p = data_p;
  }
