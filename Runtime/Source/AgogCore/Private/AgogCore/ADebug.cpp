//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  ADebug class definition module
// Author(s):    Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ADebug.hpp>
#include <AgogCore/AFunctionArg.hpp>
#include <AgogCore/APArray.hpp>
#include <AgogCore/AString.hpp>
#include <stdio.h>     // Uses: _vsnprintf(), va_list
#include <exception>   // Uses: uncaught_exception()


//=======================================================================================
// Global Macros / Defines
//=======================================================================================


//=======================================================================================
// Local Global Structures
//=======================================================================================

// Enumerated constants
enum
  {
  ADebug_print_char_max  = 2047  // Not including null character
  };

namespace Agog
  {

  //#######################################################################################
  // These 4 functions *must* be defined somewhere in the app.
  // $Note - CReis These functions are declared in this way (giving a link error if they
  // are not defined) to ensure that they are not forgotten when setting up a new project.
  // [See _AgogCoreDefaults.cpp for examples]

  void               dprint(const char * cstr_p);
  AErrorOutputBase * on_error_pre(bool nested);
  void               on_error_post(eAErrAction action);
  void               on_error_quit();

  };


//=======================================================================================
// Global Variables
//=======================================================================================

namespace
  {

  // This is defined as a global rather than a class member so that AgogCore/APArray.hpp does not
  // have to be included in the header.
  APArrayFree<tAPrintFunc>   g_dprint_funcs;
  APArrayFree<tAContextFunc> g_context_funcs;

  };


//=======================================================================================
// AErrMsg Method Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor - used to supply custom error information to a common error
//             handling mechanism such as A_ASSERT() or A_VERIFY().
// Arg         desc_high - see 'm_desc_high_p' data member description.
// Arg         err_level - see eAErrLevel
// Arg         title_p - see 'm_title_p' data member description.
// Arg         desc_low_p - see 'm_desc_low_p' data member description.
// Examples:   A_ASSERT(1 = 2, AErrMsg("1 does not equal 2", AErrLevel_notify), ADebug);
// Author(s):   Conan Reis
AErrMsg::AErrMsg(
  const AString & desc_high,
  eAErrLevel      err_level,  // = AErrLevel_error
  const char *    title_p,    // = nullptr
  const char *    desc_low_p  // = nullptr
  ) :
  m_title_p(title_p),
  m_desc_high_p(desc_high),
  m_desc_low_p(desc_low_p),
  m_err_level(err_level),
  m_err_id(AErrId_generic),
  m_func_name_p(nullptr),
  m_source_path_p(nullptr),
  m_source_line(0u)
  {
  }

//---------------------------------------------------------------------------------------
// Constructor - used to supply custom error information to a common error
//             handling mechanism such as A_ASSERT() or A_VERIFY().
// Arg         desc_high_p - see 'm_desc_high_p' data member description.
// Arg         err_level - see eAErrLevel
// Arg         title_p - see 'm_title_p' data member description.
// Arg         desc_low_p - see 'm_desc_low_p' data member description.
// Examples:   A_ASSERT(1 = 2, AErrMsg("1 does not equal 2", AErrLevel_notify), ADebug);
// Author(s):   Conan Reis
AErrMsg::AErrMsg(
  const char * desc_high_p,
  eAErrLevel   err_level,  // = AErrLevel_error
  const char * title_p,    // = nullptr
  const char * desc_low_p  // = nullptr
  ) :
  m_title_p(title_p),
  m_desc_high_p(desc_high_p),
  m_desc_low_p(desc_low_p),
  m_err_level(err_level),
  m_err_id(AErrId_generic),
  m_func_name_p(nullptr),
  m_source_path_p(nullptr),
  m_source_line(0u)
  {
  }

//---------------------------------------------------------------------------------------
// Sort of a copy constructor, but adds preprocessor information - used by
//             common handling mechanisms such as A_ASSERT() and A_VERIFY().
// Notes:      Should probably not be used directly - see alternate constructor above.
// Author(s):   Conan Reis
AErrMsg::AErrMsg(
  const AErrMsg & err_msg,
  const char *    desc_low_p,
  const char *    func_name_p,
  const char *    source_path_p,
  uint            source_line,
  uint            err_id
  ) :
  m_title_p(err_msg.m_title_p),
  m_desc_high_p(err_msg.m_desc_high_p),
  m_desc_low_p(err_msg.m_desc_low_p ? err_msg.m_desc_low_p : desc_low_p),
  m_err_level(err_msg.m_err_level),
  m_err_id(err_id),
  m_func_name_p(func_name_p),
  m_source_path_p(source_path_p),
  m_source_line(source_line)
  {
  }


//=======================================================================================
// AErrorOutputBase Method Definitions
//=======================================================================================

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Common Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Destructor
// Notes:      Ensures that the proper destructor call is invoked virtually for any
//             class derived from AErrorOutputBase.
// Author(s):   Conan Reis
AErrorOutputBase::~AErrorOutputBase()
  {
  // Does nothing
  }


//=======================================================================================
// ADebug Class Data
//=======================================================================================

uint32_t ADebug::ms_resolve_error_depth = 0u;


//=======================================================================================
// ADebug Method Definitions
//=======================================================================================

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Prints out simple debug context.
// Notes:      This can be called in the C++ Debugging "Immediate" window to print out the
//             current debug context.
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::info()
  {
  AString str;

  if (context_append(&str))
    {
    print(str);
    }
  }

//---------------------------------------------------------------------------------------
// Adds additional context information to supplied string.
//             [Automatically called whenever there is an error/assert to add any extra
//             debugging context to the error/warning message.]
// Arg         str_p - string to append additional debug context. 
// Modifiers:   static
// Author(s):   Conan Reis
bool ADebug::context_append(AString * str_p)
  {
  // Send string to any additional print function objects
  uint func_num = g_context_funcs.get_length();

  if (func_num)
    {
    uint32_t         str_original_length  = str_p->get_length();
    tAContextFunc ** context_funcs_pp     = g_context_funcs.get_array();
    tAContextFunc ** context_funcs_end_pp = context_funcs_pp + func_num;

    for (; context_funcs_pp < context_funcs_end_pp; context_funcs_pp++)
      {
      (*context_funcs_pp)->invoke(str_p);
      }

    // If context was added - add a line break before.
    if (str_p->get_length() > str_original_length)
      {
      str_p->insert('\n', str_original_length);

      // Extra context was added
      return true;
      }
    }

  // No extra context added
  return false;
  }

//---------------------------------------------------------------------------------------
// Returns additional context information as a string.
//             [Automatically called whenever there is an error/assert to add any extra
//             debugging context to the error/warning message.]
// Modifiers:   static
// Author(s):   Conan Reis
AString ADebug::context_string()
  {
  AString str;

  context_append(&str);

  return str;
  }

//---------------------------------------------------------------------------------------
// Determines if in mid `resolve_error()` call or during uncaught exception.
bool ADebug::is_nested_error()
  {
  return (ms_resolve_error_depth > 0u) || std::uncaught_exception();;
  }

//---------------------------------------------------------------------------------------
// Writes the string to the debug console.
// Arg         str - string to print. 
// Arg         call_print_funcs_b - if true any registered print functions are called
// See:        print_format(), print_args(), register_print_func(), unregister_print_func(),
//             a_cstr_format()
// Notes:      If the application has no debugger, the system debugger displays the
//             string.  If the application has no debugger and the system debugger is
//             not active, this method does nothing.
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::print(
  const AString & str,
  bool            call_print_funcs_b // = true
  )
  {
  Agog::dprint(str);

  // Send string to any additional print function objects
  uint func_num = call_print_funcs_b
    ? g_dprint_funcs.get_length()
    : 0u;

  if (func_num)
    {
    tAPrintFunc ** print_funcs_pp     = g_dprint_funcs.get_array();
    tAPrintFunc ** print_funcs_end_pp = print_funcs_pp + func_num;

    for (; print_funcs_pp < print_funcs_end_pp; print_funcs_pp++)
      {
      (*print_funcs_pp)->invoke(str);
      }
    }
  }

//---------------------------------------------------------------------------------------
// Writes the C-string to the debug console.
// Arg         cstr_p - C-string to print. 
// Arg         call_print_funcs_b - if true any registered print functions are called
// See:        print_format(), print_args(), register_print_func(), unregister_print_func(),
//             a_cstr_format()
// Notes:      If the application has no debugger, the system debugger displays the
//             string.  If the application has no debugger and the system debugger is
//             not active, this method does nothing.
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::print(
  const char * cstr_p,
  bool         call_print_funcs_b // = true
  )
  {
  Agog::dprint(cstr_p);

  // Send string to any additional print function objects
  uint func_num = call_print_funcs_b
    ? g_dprint_funcs.get_length()
    : 0u;

  if (func_num)
    {
    AString print_str(cstr_p);

    tAPrintFunc ** print_funcs_pp     = g_dprint_funcs.get_array();
    tAPrintFunc ** print_funcs_end_pp = print_funcs_pp + func_num;

    for (; print_funcs_pp < print_funcs_end_pp; print_funcs_pp++)
      {
      (*print_funcs_pp)->invoke(print_str);
      }
    }
  }

//---------------------------------------------------------------------------------------
// Writes the formatted string to the debug console.
// Arg         format_str_p - follows the same format as the C printf(), sprintf(), etc.
//             See the MSDev online help for 'Format Specification Fields' for a
//             description. 
// Arg         args - variable length arguments expected by the formatted string.
//
//             #### IMPORTANT #### Since this method currently uses the standard C
//             string formatting functions, passing a AString object as an argument will
//             try to use the address of the object with disastrous results.  Use the
//             %s specifier and 'str.as_cstr()' as an argument for AString objects.
//
//             In the future, a %$ type could be added to use AString objects natively.
// See:        print(), print_format(), register_print_func(), unregister_print_func(),
//             a_cstr_format()
// Notes:      If the application has no debugger, the system debugger displays the
//             string.  If the application has no debugger and the system debugger is
//             not active, this method does nothing.
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::print_args(
  const char * format_cstr_p,
  va_list      args
  )
  {
  char buffer_p[ADebug_print_char_max + 1];  // Add spot for null char

  #if defined(A_PLAT_PS3) || defined(A_PLAT_PS4) || defined(A_PLAT_LINUX64)
    int length = vsnprintf(buffer_p, ADebug_print_char_max + 1, format_cstr_p, args);
  #else
    int length = _vsnprintf(buffer_p, ADebug_print_char_max, format_cstr_p, args);
  #endif

  // If more characters than buffer has, truncate
  if ((length == -1) || (length == ADebug_print_char_max))
    {
    // Put in null-terminator
    buffer_p[ADebug_print_char_max] = '\0';
    }

  Agog::dprint(buffer_p);

  // Send string to any additional print function objects
  uint func_num = g_dprint_funcs.get_length();

  if (func_num)
    {
    AString        str(buffer_p, uint(length));
    tAPrintFunc ** print_funcs_pp     = g_dprint_funcs.get_array();
    tAPrintFunc ** print_funcs_end_pp = print_funcs_pp + func_num;

    for (; print_funcs_pp < print_funcs_end_pp; print_funcs_pp++)
      {
      (*print_funcs_pp)->invoke(str);
      }
    }
  }

//---------------------------------------------------------------------------------------
// Writes the formatted string to the debug console.
// Arg         format_str_p - follows the same format as the C printf(), sprintf(), etc.
//             See the MSDev online help for 'Format Specification Fields' for a
//             description. 
// Arg         args - variable length arguments expected by the formatted string.
//
//             #### IMPORTANT #### Since this method currently uses the standard C
//             string formatting functions, passing a AString object as an argument will
//             try to use the address of the object with disastrous results.  Use the
//             %s specifier and 'str.as_cstr()' as an argument for AString objects.
//
//             In the future, a %$ type could be added to use AString objects natively.
// See:        print(), print_args(), register_print_func(), unregister_print_func(),
//             a_cstr_format()
// Notes:      If the application has no debugger, the system debugger displays the
//             string.  If the application has no debugger and the system debugger is
//             not active, this method does nothing.
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::print_format(
  const char * format_cstr_p, ...)
  {
  va_list args;

  // initialize argument list
  va_start(args, format_cstr_p);

  print_args(format_cstr_p, args);

  // end argument list processing
  va_end(args);
  }

//---------------------------------------------------------------------------------------
// Writes the string to standard output.
// 
// Params:
//   str: string to print. 
// 
// Notes:
//   The standard output `stdout` may only be written to with MS Windows console apps and
//   not GUI apps.
//   
//   $Revisit - CReis There may be a way to hack use of `stdout` in GUI apps - see:
//     http://stackoverflow.com/a/26087606/1060973
//     http://www.codeproject.com/Articles/10073/Console-Output-from-a-GUI-program?msg=1477550#xx1477550xx
//   
// Modifiers: static
// Author(s): Conan Reis
void ADebug::print_std(const AString & str)
  {
  fwrite(str.as_cstr(), sizeof(char), str.get_length(), stdout);
  fflush(stdout);
  }

//---------------------------------------------------------------------------------------
// Adds additional context information to supplied string.
//             [These functions are automatically called whenever there is an error/assert
//             to add any extra context to the error/warning message.]
// See:        unregister_context_func(), context_append(), context_string()
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::register_context_func(tAContextFunc * context_func_p)
  {
  g_context_funcs.append_absent(*context_func_p);
  }

//---------------------------------------------------------------------------------------
// Adds an additional print function object to be invoked whenever print(),
//             print_format(), or print_args() is called.
// Arg         print_func_p - address of print function object to add
// See:        print(), print_format(), print_args(), unregister_print_func()
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::register_print_func(tAPrintFunc * print_func_p)
  {
  g_dprint_funcs.append_absent(*print_func_p);
  }

//---------------------------------------------------------------------------------------
// Also prints to standard output (stdout) whenever print(), print_format(),
//             or print_args() is called.
// See:        print(), print_format(), print_args(), register_print_func()
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::register_print_std()
  {
  register_print_func(new AFunctionArg<const AString &>(&ADebug::print_std));
  }

//---------------------------------------------------------------------------------------
// Removes a previously added context function object.
// See:        register_context_func(), context_append(), context_string()
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::unregister_context_func(tAContextFunc * context_func_p)
  {
  g_context_funcs.remove(*context_func_p);
  }

//---------------------------------------------------------------------------------------
// Removes a previously added print function object.
// Arg         print_func_p - address of print function object to remove
// See:        print(), print_format(), print_args(), register_print_func()
// Modifiers:   static
// Author(s):   Conan Reis
void ADebug::unregister_print_func(tAPrintFunc * print_func_p)
  {
  g_dprint_funcs.remove(*print_func_p);
  }

//---------------------------------------------------------------------------------------
// Calls the default error output object with the error info and determines
//             the error resolution action from the user (or defaults to appropriate).
// Returns:    true if a user break should be made in the debugger, false if not
// Arg         msg - information on the error - See AErrMsg.
// Arg         action_p - address to store chosen course of action to take to resolve error
// Arg         test_again_p - pointer to boolean that specifies whether this error
//             should be checked again in the future or if all further occurrences should
//             be ignored.
// Author(s):   Conan Reis
bool ADebug::resolve_error(
  const AErrMsg & msg,
  eAErrAction *   action_p,
  bool *          test_again_p // = nullptr
  )
  {
  bool nested_err = is_nested_error();

  ms_resolve_error_depth++;

  AErrorOutputBase * err_output_p = Agog::on_error_pre(nested_err);	// See _AgogCoreDefaults.cpp

  eAErrAction action;
  bool        debug_break = (err_output_p && !nested_err)
    ? err_output_p->determine_choice(msg, &action)  // Use supplied error output object
    : determine_choice(msg, &action, nested_err);

  if (action == AErrAction_ignore_all)
    {
    if (test_again_p)
      {
      *test_again_p = false;
      }

    action = AErrAction_ignore;
    }

  if (action_p)
    {
    *action_p = action;
    }

  ms_resolve_error_depth--;

  Agog::on_error_post(action);

  if (action == AErrAction_quit)
    {
    Agog::on_error_quit();
    }

  return debug_break;
  }

//---------------------------------------------------------------------------------------
// Default unprompted action to take when there is an error/warning.
//             It also writes out information to the IDE Output Window.
// Returns:    true if a user break should be made in the debugger, false if not
// Arg         msg - See the definition of AErrMsg in ADebug.h for more information.
// Arg         action_p - address to store chosen course of action to take to resolve error
// Modifiers:   static
// Author(s):   Conan Reis
bool ADebug::determine_choice(
  const AErrMsg & msg,
  eAErrAction *   action_p,
  bool            nested_err
  )
  {
  *action_p = (nested_err || (msg.m_err_level <= AErrLevel_notify)) ? AErrAction_ignore : AErrAction_quit;

  if (nested_err)
    {
    print("### Had another exception while a previous exception is being handled: ###");
    }

  const char * title_p     = (msg.m_title_p ? msg.m_title_p : "");
  const char * desc_high_p = msg.m_desc_high_p ? msg.m_desc_high_p : "An error has occurred.";
  const char * desc_low_p  = msg.m_desc_low_p ? msg.m_desc_low_p : "";
  const char * func_name_p = msg.m_func_name_p ? msg.m_func_name_p : "";

  if (msg.m_source_path_p)
    {
    print_format(
      "\n%s : %s\n\n  Internal Info:\n    %s\n    %s(%u) : %s\n    Exception Id: %u\n",
      title_p,
      desc_high_p,
      func_name_p,
      msg.m_source_path_p,
      msg.m_source_line,
      desc_low_p,
      msg.m_err_id);
    }
  else
    {
    print_format(
      "\n%s : %s\n\n  Internal Info:\n    %s\n    %s\n    Exception Id: %u\n",
      title_p,
      desc_high_p,
      func_name_p,
      desc_low_p,
      msg.m_err_id);
    }

  if (!nested_err)
    {
    info();
    }

  // Unprompted so don't do a user break
  return false;
  }

