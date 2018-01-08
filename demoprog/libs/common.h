/// \file common.h
/// \brief Types common for list.h and tree.h

#ifndef __common_h__
#define __common_h__

#include <stdbool.h>

/// element wrapper
typedef union element elem_t;

/// Element wrapper
///
/// Elements in the list are stored inside an ::elem_t which is
/// passed in by copy.
///
union element
{
  int           i;
  unsigned int  u;
  void         *p;
  float         f;
};

/// Function for copying an ::elem_t before adding it to a data structure
typedef elem_t(*element_copy_fun)(elem_t);

/// function for freeing an ::elem_t
typedef void(*element_free_fun)(elem_t);

/// function for comparing two ::elem_t, determining their order
typedef int(*element_comp_fun)(elem_t, elem_t);

/// function to apply to every element in tree_apply() and list_apply()
typedef bool(*elem_apply_fun)(elem_t elem, void *data);

#endif
