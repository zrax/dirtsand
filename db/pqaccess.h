/******************************************************************************
 * This file is part of dirtsand.                                             *
 *                                                                            *
 * dirtsand is free software: you can redistribute it and/or modify           *
 * it under the terms of the GNU Affero General Public License as             *
 * published by the Free Software Foundation, either version 3 of the         *
 * License, or (at your option) any later version.                            *
 *                                                                            *
 * dirtsand is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Affero General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the GNU Affero General Public License   *
 * along with dirtsand.  If not, see <http://www.gnu.org/licenses/>.          *
 ******************************************************************************/

#include "Types/Uuid.h"
#include <libpq-fe.h>

#ifdef DS_STORE_BLOBS_AS_BASE64
#include "encodings.h"
#endif

template <size_t count>
struct PostgresStrings
{
    DS::String m_strings[count];
#ifndef DS_STORE_BLOBS_AS_BASE64
    DS::Blob m_blobs[count];
#endif
    union
    {
        uint32_t ui;
        int i;
    } m_ints[count];

    const char* m_values[count];
    int m_lengths[count];
    int m_formats[count];

    void set(size_t idx, const DS::String& str)
    {
        m_strings[idx] = str;
        m_values[idx] = str.c_str();
        m_lengths[idx] = str.length();
        m_formats[idx] = 0; // Text
    }

    void set(size_t idx, uint32_t value)
    {
        m_ints[idx].ui = value;
        m_values[idx] = reinterpret_cast<const char*>(&m_ints[idx].ui);
        m_lengths[idx] = sizeof(value);
        m_formats[idx] = 1; // Binary
    }

    void set(size_t idx, int value)
    {
        m_ints[idx].i = value;
        m_values[idx] = reinterpret_cast<const char*>(&m_ints[idx].i);
        m_lengths[idx] = sizeof(value);
        m_formats[idx] = 1; // Binary
    }

    void set(size_t idx, const DS::Uuid& value)
    {
        set(idx, value.toString());
    }

    void set(size_t idx, const DS::Blob& value)
    {
#ifdef DS_STORE_BLOBS_AS_BASE64
        set(idx, DS::Base64Encode(value.buffer(), value.size()));
#else
        m_blobs[idx] = value;
        m_values[idx] = reinterpret_cast<const char*>(value.buffer());
        m_lengths[idx] = value.size();
        m_formats[idx] = 1; // Binary
#endif
    }
};

namespace DS
{
    template <typename... _Args>
    PGresult* PQexecVA(PGconn* conn, const char* command, _Args&&... args)
    {
        PostgresStrings<sizeof...(_Args)> parms;
        size_t i = 0;
        auto dummy = { (parms.set(i, std::forward<_Args>(args)), ++i)... };
        (void)dummy;
        return ::PQexecParams(conn, command, sizeof...(_Args), nullptr,
                              parms.m_values, parms.m_lengths, parms.m_formats, 0);
    }
}
