/*
 * Helpers for defining versioned symbols
 *
 * Copyright (C) 2021 Red Hat, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _LIBCRYPTSETUP_SYMVER_H
#define _LIBCRYPTSETUP_SYMVER_H

/*
 * Note on usage:
 *
 * Do not use CRYPT_SYMBOL_EXPORT_NEW and CRYPT_SYMBOL_EXPORT_OLD on public
 * symbols being exported only once. Linker will handle it automatically as
 * always.
 *
 * It's supposed to be used only with symbols that are exported in at least
 * two versions simultaneously as follows:
 *
 * - the latest version is marked with _NEW variant and oll other compatible
 *   symbols should be marked with _OLD variant
 *
 * Examples:
 *
 * - int crypt_func_X(unsigned *x, long y) gets introduced in CRYPTSETUP_2.4.
 *
 *   No need to use any macro referenced here, just add proper version
 *   mapping in libcryptsetup.sym file.
 *
 *   In later version CRYPTSETUP_2.5 symbol crypt_func_X has to fixed
 *   in incompatible way by adding new function parameter. The new version
 *   has to be added in mapping file libcryptsetup.sym as well.
 *
 *   The definition of compatible function gets prefixed with following macro:
 *
 *   CRYPT_SYMBOL_EXPORT_OLD(int, crypt_func_X, 2, 4,
 *   			     unsigned *x, long y)
 *   {
 *   	function body
 *   }
 *
 *   Whereas new version introduced in CRYPTSETUP_2.5 is defined as follows:
 *
 *   CRYPT_SYMBOL_EXPORT_NEW(int, crypt_func_X, 2, 5,
 *   			     unsigned *x, long y, void *new_parameter)
 *   {
 *   	function body
 *   }
 *
 *   If in later version CRYPTSETUP_2.6 yet another version of crypt_func_X gets
 *   introduced it will be prefixed with CRYPT_SYMBOL_EXPORT_NEW(int, crypt_func_X, 2, 6...)
 *   macro and all previous versions CRYPTSETUP_2.4 and CRYPTSETUP_2.5 will be
 *   under CRYPT_SYMBOL_EXPORT_OLD(int, crypt_func_X, ...) macro
 */

#ifdef __has_attribute
#  if __has_attribute(symver)
#    define _CRYPT_SYMVER(_local_sym, _public_sym, _ver_str, _maj, _min)     \
       __attribute__((__symver__(#_public_sym _ver_str #_maj "." #_min)))
#  endif
#endif

#if !defined(_CRYPT_SYMVER) && defined(__GNUC__)
#  define _CRYPT_SYMVER(_local_sym, _public_sym, _ver_str, _maj, _min)         \
     asm(".symver " #_local_sym "," #_public_sym _ver_str #_maj "." #_min);
#endif

#define _CRYPT_FUNC(_public_sym, _prefix_str, _maj, _min, _ret, ...)                                    \
  _ret __##_public_sym##_v##_maj##_##_min(__VA_ARGS__);                                                 \
  _CRYPT_SYMVER(__##_public_sym##_v##_maj##_##_min, _public_sym, _prefix_str "CRYPTSETUP_", _maj, _min) \
  _ret __##_public_sym##_v##_maj##_##_min(__VA_ARGS__)

#ifdef _CRYPT_SYMVER

#  define CRYPT_SYMBOL_EXPORT_OLD(_ret, _public_sym, _maj, _min, ...)  \
     _CRYPT_FUNC(_public_sym, "@", _maj, _min, _ret, __VA_ARGS__)
#  define CRYPT_SYMBOL_EXPORT_NEW(_ret, _public_sym, _maj, _min, ...)  \
     _CRYPT_FUNC(_public_sym, "@@", _maj, _min, _ret, __VA_ARGS__)

#else /* no support for symbol versioning at all */

#  define CRYPT_SYMBOL_EXPORT_OLD(_ret, _public_sym, _maj, _min, ...)  \
     static inline __attribute__((unused))                             \
       _ret __##_public_sym##_v##_maj##_##_min(__VA_ARGS__)

#  define CRYPT_SYMBOL_EXPORT_NEW(_ret, _public_sym, _maj, _min, ...) \
     _ret _public_sym(__VA_ARGS__)

#endif

#endif /* _LIBCRYPTSETUP_SYMVER_H */
