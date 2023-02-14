//
// XSP data management.
//

#ifndef XBASE_UTIL_XSPMAN_H
#define XBASE_UTIL_XSPMAN_H

#include <stdbool.h>

//
// 1) Initialize
//

// Call this first to initialize data structures.
void xspman_init(void);

//
// 2) File registration
//

// Call the following two functions for each sprite file you want to load.
// For an XOBJ file call xspman_reg_xsp(), and for a regular PCG tile call
// xspman_reg_sp().

// Registers an XSP file set.
// Returns the base sprite number for xobj_set(), or -1 on error.
short xspman_reg_xsp(const char *fname_base);

// Registers an XSP file set.
// Returns the base PCG pattern number for xsp_set(), or -1 on error.
short xspman_reg_sp(const char *fname_base);

//
// 3) File loading
//

// Loads all registered data. Returns true if data was loaded.
// Allocates memory based on what was requested during registration.
// After calling this function, use the functions below to get pointers for
// use with xsp_pcgdat_set() and xsp_objdat_set(). Afterwards, XSP may be used.
bool xspman_load(void);

//
// 4) Post-Init
//
// These functions should be used after xspman_load() has been called.

// Returns a pointer to PCG pattern data. The data it points to remains valid
// until xspman_shutdown() is called.
const void *xspman_get_pcgdat_ptr(void);

// Returns a pointer to PCG "alt" scratchpad memory. The data it points to
// remains valid until xspman_shutdown() is called.
char *xspman_get_pcg_alt_ptr(void);

// Returns the size of the array referened by xspman_get_pcg_alt_ptr().
short xspman_get_pcg_alt_bytes(void);

// Returns a pointer to OBJ reference data. The data it points to remains valid
// until xspman_shutdown() is called.
const void *xspman_get_objdat_ptr(void);

//
// 5) Finish
//

// Frees memory allocated by xspman_load. Once called, the pre-init functions
// may be called again. Call this after xsp_off().
void xspman_shutdown(void);


#endif  // XBASE_UTIL_XSPMAN_H
