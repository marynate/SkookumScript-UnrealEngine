//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript Base Object inline file
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AObjReusePool.hpp>
#include <SkookumScript/SkBrain.hpp>


//=======================================================================================
// Inline Methods
//=======================================================================================

// The methods for simple data type retrieval are inlined in the SkookumScript/SkInstance.inl file.

//---------------------------------------------------------------------------------------
// Retrieves a SkData object from the dynamic pool and initializes it with the
//             nil object for use.  This method should be used instead of 'new' because it
//             prevents unnecessary allocations by reusing previously allocated objects.
// Returns:    a dynamic SkData
// See:        pool_delete() 
// Notes:      To 'deallocate' an object that was retrieved with this method, use
//             'pool_delete()' rather than 'delete'.
// Modifiers:   static
// Author(s):   Conan Reis
A_INLINE SkData * SkData::pool_new(
  const ASymbol & name
  )
  {
  SkData * data_p  = get_pool().pop();
  data_p->m_name   = name;
  data_p->m_data_p = SkBrain::ms_nil_p;

  return data_p;
  }

//---------------------------------------------------------------------------------------
// Retrieves a SkData object from the dynamic pool and initializes it with the
//             supplied object for use.  This method should be used instead of 'new' 
//             because it prevents unnecessary allocations by reusing previously allocated
//             objects.
// Returns:    a dynamic SkData
// See:        pool_delete() 
// Notes:      To 'deallocate' an object that was retrieved with this method, use
//             'pool_delete()' rather than 'delete'.
// Modifiers:   static
// Author(s):   Conan Reis
A_INLINE SkData * SkData::pool_new(
  const ASymbol & name,
  SkInstance *    instance_p
  )
  {
  SkData * data_p  = get_pool().pop();
  data_p->m_name   = name;
  data_p->m_data_p = instance_p;

  return data_p;
  }

//---------------------------------------------------------------------------------------
// Puts a SkData object back on the dynamic pool for future reuse.
//             This method should be used instead of 'delete' because it prevents
//             unnecessary deallocations by reusing previously allocated objects.
// See:        pool_new() 
// Modifiers:   static
// Author(s):   Conan Reis
A_INLINE void SkData::pool_delete(SkData * data_p)
  {
  get_pool().append(data_p);
  }

