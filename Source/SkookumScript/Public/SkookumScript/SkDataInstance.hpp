//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Instance of a class with data members
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKDATAINSTANCE_HPP
#define __SKDATAINSTANCE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/APSorted.hpp>
#include <SkookumScript/SkInstance.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================


//---------------------------------------------------------------------------------------
// Notes      Class instance objects with one or more data members
// Subclasses SkDataInstance(SkActor), SkMetaClass
// Author(s)  Conan Reis
class SkDataInstance : public SkInstance
  {
  public:
  
  friend class AObjBlock<SkDataInstance>;

  // Common Methods

    SK_NEW_OPERATORS(SkDataInstance);
    SkDataInstance(SkClass * class_p);
    virtual ~SkDataInstance();


  // Methods

    void add_data_members();
    void data_empty()          { SkData::empty_table(&m_data); }

    virtual SkData * get_data_by_instance(const SkInstance & obj) const;

    // Overriding from SkInstance

    virtual void delete_this() override;

    // Overriding from SkObjectBase

    virtual SkInstance * find_data_by_name(const ASymbol & name) const override;
    virtual SkInstance * get_data_by_name(const ASymbol & name) const override;
    virtual void         set_data_by_name(const ASymbol & name, SkInstance * obj_p);

    
  // Class Methods

    static SkDataInstance * new_instance(SkClass * class_p);
    static AObjReusePool<SkDataInstance> & get_pool();


  protected:

  // Internal Methods

    // Default constructor only may be called by pool_new()
    SkDataInstance() {}

  // Data Members

    // Array of class instance data members - accessed by symbol name.
    // $Revisit - CReis It may be possible to rewrite this so that a direct index can be
    // used rather than a binary search of the symbols
    tSkDataTable m_data;

  };  // SkDataInstance


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         class_p - Class type for this instance
// Author(s):   Conan Reis
inline SkDataInstance::SkDataInstance(
  SkClass * class_p // = nullptr
  ) :
  SkInstance(class_p)
  {
  add_data_members();
  }


#endif  // __SKDATAINSTANCE_HPP

