//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  Object Reuse Pool class template
// Author(s):    Conan Reis
// Create Date:   2000-05-30
// Notes:          
//=======================================================================================


#ifndef __AOBJREUSEPOOL_HPP // __AOBJREUSEPOOL_HPP defined later in file


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/APArray.hpp>


//=======================================================================================
// Global Macros / Defines
//=======================================================================================

#if defined(A_EXTRA_CHECK) && !defined(AORPOOL_NO_USAGE_COUNT) && !defined(AORPOOL_USAGE_COUNT)
  // If this is defined track the current and maximum number of objects used.
  #define AORPOOL_USAGE_COUNT
#endif


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Notes    This is a sub-structure use by the AObjReusePool to store blocks of memory
//          for reusable objects.
//
// Arg      _ObjectType - the class/type of objects to be stored
// UsesLibs    
// Inlibs   AgogCore/AgogCore.lib
// Examples:    
// Author   Conan Reis
template<class _ObjectType>
class AObjBlock
  {
  public:

  // Common types

    typedef AObjBlock<_ObjectType> tObjBlock;  // Local shorthand for this template


  // Common Methods

    AObjBlock(uint32_t size) :
      m_size(size)
      {
      m_objects_a = AMemory::new_array<_ObjectType>(size, "AObjBlock");
      A_VERIFY_MEMORY(m_objects_a != nullptr, tObjBlock);
      }

    ~AObjBlock()
      {
      AMemory::delete_array(m_objects_a, m_size);
      }

    // Number of objects contained (stored and optionally initialized) by this block
    uint32_t m_size;

    // The object array - with m_size elements
    _ObjectType * m_objects_a;
  };


//---------------------------------------------------------------------------------------
// Notes    The AObjReusePool class template provides a population of contiguous blocks of
//          dynamically objects that call there constructors once when created, then are
//          taken out of the pool and put back on the pool as they are used and finished
//          being used respectively, and then are finally destructed at the end of this
//          objects lifetime.  Additional blocks of objects are allocated as needed.
//
//          pop() is used effectively as a new.
//          append() is used effectively as a delete.
//
//          Any modifications to this template should be compile-tested by adding an
//          explicit instantiation declaration such as:
//            template class AObjReusePool<AStringRef>;
// Arg      _ObjectType - the class/type of elements to be pointed to by the array.
// UsesLibs    
// Inlibs   AgogCore/AgogCore.lib
// Examples:    
// Author   Conan Reis
template<class _ObjectType>
class AObjReusePool
  {
  public:

  // Public Data Members

    #ifdef AORPOOL_USAGE_COUNT

      // Number of objects currently used / outstanding
      uint32_t m_count_now;

      // Maximum number of objects used at once - i.e. peak usage
      uint32_t m_count_max;

      // Optional callback that is called just prior to adding
      void (* m_grow_f)(const AObjReusePool & pool);

    #endif

  // Common types

    // Local shorthand for templates
    typedef AObjReusePool<_ObjectType> tObjReusePool;
    typedef AObjBlock<_ObjectType>     tObjBlock;

  // Common Methods

    AObjReusePool(uint32_t initial_size, uint32_t expand_size);
    ~AObjReusePool();


  // Accessor Methods

    uint32_t get_block_count() const     { return 1u + m_exp_blocks.get_length(); }
    uint32_t get_bytes_allocated() const { return get_count_capacity() * sizeof(_ObjectType); }
    uint32_t get_expand_size() const     { return m_expand_size; }
    uint32_t get_count_initial() const   { return m_block_p->m_size; }
    uint32_t get_count_available() const { return m_pool.get_length() + m_exp_pool.get_length(); }
    uint32_t get_count_expanded() const  { return m_exp_blocks.is_empty() ? 0u : (m_exp_blocks.get_length() * m_expand_size); }
    uint32_t get_count_capacity() const  { return m_block_p->m_size + get_count_expanded(); }
    uint32_t get_count_used() const;
    uint32_t get_count_max() const;
    uint32_t get_count_overflow() const;

    const APArray<_ObjectType> & get_available() const         { return m_pool; }
    const APArray<_ObjectType> & get_available_epanded() const { return m_exp_pool; }


  // Modifying Methods

    _ObjectType * pop();
    void          append(_ObjectType * obj_p);
    void          append_all(_ObjectType ** objs_a, uint length);

    void          append_block(uint32_t size);
    void          empty();
    void          remove_expanded();
    void          repool();


  protected:
  // Data Members

    // Pool of previously constructed objects that are ready for use.
    APArray<_ObjectType> m_pool;

    // Initial object block - i.e. the block of memory where the objects are actually
    // stored.
    tObjBlock * m_block_p;


    // These expansion data structures are separate so that they can be easily cleared.

    // Pool of previously constructed objects that are ready for use.
    APArray<_ObjectType> m_exp_pool;

    // This is the array of expanded object blocks - i.e. if the initial block is not
    // sufficient these additional blocks are added.
    APArray<tObjBlock> m_exp_blocks;

    // If there is need of additional objects, this is the size to use for additional
    // object blocks.
    uint32_t m_expand_size;

  };  // AObjReusePool



//=======================================================================================
// Methods
//=======================================================================================

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Common Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         initial_size - initial object population for the reuse pool
// Arg         expand_size - additional number of objects to allocate if all the objects
//             in the reuse pool are in use and more objects are required.
// Author(s):   Conan Reis
template<class _ObjectType>
inline AObjReusePool<_ObjectType>::AObjReusePool(
  uint32_t initial_size,
  uint32_t expand_size
  ) :
  #ifdef AORPOOL_USAGE_COUNT
    m_count_now(0u), m_count_max(0u), m_grow_f(nullptr),
  #endif
  m_pool(nullptr, 0u, initial_size),
  m_block_p(nullptr),
  m_expand_size(expand_size)
  {
  append_block(initial_size);
  }

//---------------------------------------------------------------------------------------
// Destructor
// Author(s):   Conan Reis
template<class _ObjectType>
inline AObjReusePool<_ObjectType>::~AObjReusePool()
  {
  m_exp_blocks.free_all();
  delete m_block_p;
  }

//---------------------------------------------------------------------------------------
// Determines number of objects currently used / outstanding.
// Returns:    number of objects currently used
// Author(s):   Conan Reis
template<class _ObjectType>
inline uint32_t AObjReusePool<_ObjectType>::get_count_used() const
  {
  #ifdef AORPOOL_USAGE_COUNT
    return m_count_now;
  #else
    return get_count_capacity() - get_count_available();
  #endif
  }

//---------------------------------------------------------------------------------------
// Determines number of objects currently used / outstanding.
// Returns:    number of objects currently used
// Author(s):   Conan Reis
template<class _ObjectType>
inline uint32_t AObjReusePool<_ObjectType>::get_count_max() const
  {
  #ifdef AORPOOL_USAGE_COUNT
    return m_count_max;
  #else
    // Approximate
    return get_count_capacity();
  #endif
  }

//---------------------------------------------------------------------------------------
// Determines number of objects used above and beyond the initial amount that
//             was allocated.
// Returns:    number of objects that have overflowed initial allocation
// Author(s):   Conan Reis
template<class _ObjectType>
inline uint32_t AObjReusePool<_ObjectType>::get_count_overflow() const
  {
  #ifdef AORPOOL_USAGE_COUNT
    return (m_count_max <= m_block_p->m_size) ? 0u : (m_count_max - m_block_p->m_size);
  #else
    // Approximate
    return get_count_expanded();
  #endif
  }

//---------------------------------------------------------------------------------------
// Retrieves a previously allocated object from the dynamic pool.  This
//             method should be used instead of 'new' because it prevents unnecessary
//             allocations by reusing previously allocated objects.
// Returns:    a dynamic object
// See:        append() 
// Notes:      To 'deallocate' an object that was retrieved with this method, use
//             'append()' rather than 'delete'.
// Author(s):   Conan Reis
template<class _ObjectType>
inline _ObjectType * AObjReusePool<_ObjectType>::pop()
  {
  #ifdef AORPOOL_USAGE_COUNT
    m_count_now++;

    if (m_count_now > m_count_max)
      {
      m_count_max = m_count_now;
      }
  #endif

  if (m_pool.get_length() == 0u)
    {
    if (m_exp_pool.get_length() == 0u)
      {
      // No free objects, so make more
      append_block(m_expand_size);
      }

    return m_exp_pool.pop_last();
    }

  return m_pool.pop_last();
  }

//---------------------------------------------------------------------------------------
// Frees up an Object and returns it into the dynamic pool ready for its next
//             use.  This method should be used instead of 'delete' because it prevents
//             unnecessary deallocations by saving previously allocated objects.
// Arg         obj_p - pointer to object to free up and put into the dynamic pool.
// See:        pop(), append_all()
// Notes:      To 'allocate' an object use 'pop()' rather than 'new'.
// Author(s):   Conan Reis
template<class _ObjectType>
inline void AObjReusePool<_ObjectType>::append(_ObjectType * obj_p)
  {
  #ifdef AORPOOL_USAGE_COUNT
    m_count_now--;
  #endif

  if ((obj_p >= m_block_p->m_objects_a)
    && (obj_p < (m_block_p->m_objects_a + m_block_p->m_size)))
    {
    m_pool.append(*obj_p);
    }
  else
    {
    m_exp_pool.append(*obj_p);
    }
  }

//---------------------------------------------------------------------------------------
// Frees up 'length' Objects and returns them into the dynamic pool ready for
//             their next use.  This method should be used instead of 'delete' because it
//             prevents unnecessary deallocations by saving previously allocated objects.
// Arg         objs_a - pointer to the array of objects to free up and put into the
//             dynamic pool.
// See:        append(), pop()
// Notes:      To 'allocate' an object use 'pop()' rather than 'new'.
// Author(s):   Conan Reis
template<class _ObjectType>
void AObjReusePool<_ObjectType>::append_all(
  _ObjectType ** objs_a,
  uint            length
  )
  {
  #ifdef AORPOOL_USAGE_COUNT
    m_count_now -= length;
  #endif

  _ObjectType *  obj_p;
  _ObjectType ** objs_end_a = objs_a + length;

  while (objs_a < objs_end_a)
    {
    obj_p = *objs_a;

    if ((obj_p >= m_block_p->m_objects_a)
      && (obj_p < (m_block_p->m_objects_a + m_block_p->m_size)))
      {
      m_pool.append(*obj_p);
      }
    else
      {
      m_exp_pool.append(*obj_p);
      }

    objs_a++;
    }
  }

//---------------------------------------------------------------------------------------
// Creates and appends a block of objects to the object pool
// Arg         size - number of objects to allocate in the block
// Author(s):   Conan Reis
template<class _ObjectType>
void AObjReusePool<_ObjectType>::append_block(uint32_t size)
  {
  #ifdef AORPOOL_USAGE_COUNT
    // Notify that the pool has grown
    if (m_block_p && m_grow_f)
      {
      (m_grow_f)(*this);
      }
  #endif

  tObjBlock * obj_block_p = new ("tObjBlock") tObjBlock(size);

  A_VERIFY_MEMORY(obj_block_p != nullptr, tObjReusePool);

  if (m_block_p)
    {
    m_exp_blocks.append(*obj_block_p);
    m_exp_pool.append_all(obj_block_p->m_objects_a, size);
    }
  else
    {
    m_block_p = obj_block_p;
    m_pool.append_all(obj_block_p->m_objects_a, size);
    }
  }

//---------------------------------------------------------------------------------------
// Clears out pools
// See:        remove_expanded()
// Author(s):   Conan Reis
template<class _ObjectType>
void AObjReusePool<_ObjectType>::empty()
  {
  #ifdef AORPOOL_USAGE_COUNT
    m_count_now = 0u;
  #endif

  // $Revisit - CReis Might be a good idea to put an assert here to ensure that all objects
  // have been returned to the pool for reuse before they are deleted.

  m_pool.empty();
  m_pool.compact();
  delete m_block_p;
  m_block_p = nullptr;

  m_exp_pool.empty();
  m_exp_pool.compact();
  m_exp_blocks.free_all();
  m_exp_blocks.compact();
  }

//---------------------------------------------------------------------------------------
// Removes / frees memory of all expanded / grown objects above and beyond
//             the initial object block.
// See:        empty()
// Author(s):   Conan Reis
template<class _ObjectType>
void AObjReusePool<_ObjectType>::remove_expanded()
  {
  #ifdef AORPOOL_USAGE_COUNT
    m_count_now = m_block_p->m_size - m_pool.get_length();
  #endif

  // Ensure that all objects have been returned to the pool for reuse before they are
  // deleted.
  A_ASSERT(
    m_exp_pool.get_length() == m_exp_blocks.get_length() * m_expand_size,
    "AObjReusePool<>::remove_expanded() - Not all of the expanded objects have been returned,\nso they probably should not all be deleted!",
    AErrId_generic,
    tObjReusePool);

  m_exp_pool.empty();
  m_exp_pool.compact();
  m_exp_blocks.free_all();
  m_exp_blocks.compact();
  }

//---------------------------------------------------------------------------------------
// Removes / frees memory of all expanded / grown objects above and beyond
//             the initial object block and ensures that all of the objects in the
//             initial pool are available whether they were returned for use or not.
// See:        empty()
// Author(s):   Conan Reis
template<class _ObjectType>
void AObjReusePool<_ObjectType>::repool()
  {
  remove_expanded();

  #ifdef AORPOOL_USAGE_COUNT
    m_count_now = 0u;
  #endif

  m_pool.empty();
  m_pool.append_all(m_block_p->m_objects_a, m_block_p->m_size);
  }


#define __AOBJREUSEPOOL_HPP
  
#endif  // __AOBJREUSEPOOL_HPP


