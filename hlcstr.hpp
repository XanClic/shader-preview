#ifndef HLCSTR_HPP
#define HLCSTR_HPP

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <gtkmm.h>


class hlcstr
{
    public:
        hlcstr(void) { cstr = NULL; }
        hlcstr(const char *from_cstr) { cstr = strdup(from_cstr); }
        hlcstr(const char *from_str, size_t len) { cstr = static_cast<char *>(malloc(len + 1)); strcpy(cstr, from_str); }
        ~hlcstr(void) { free(cstr); }

        template <typename T> hlcstr operator%(const T &param) { hlcstr ret; asprintf(&ret.cstr, cstr, param); return ret; }

        operator char *(void) { return cstr; }
        operator Glib::ustring(void) { return Glib::ustring(cstr); }

        char *cstr;
};


static inline hlcstr operator "" _hl(const char *lit, size_t len) { return hlcstr(lit, len); }

#endif
