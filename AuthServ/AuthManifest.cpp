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

#include "AuthManifest.h"
#include "errors.h"

DS::AuthManifest::~AuthManifest()
{
    while (!m_files.empty()) {
        delete m_files.front();
        m_files.pop_front();
    }
}

DS::NetResultCode DS::AuthManifest::loadManifest(const char* filename)
{
    FILE* mfs = fopen(filename, "r");
    if (!mfs)
        return e_NetFileNotFound;

    char lnbuf[4096];
    long mfsline = 0;
    while (fgets(lnbuf, 4096, mfs)) {
        ++mfsline;
        String line = String(lnbuf).strip('#');
        if (line.isEmpty())
            continue;

        std::vector<String> parts = line.split(',');
        if (parts.size() != 2) {
            fprintf(stderr, "Warning:  Ignoring invalid manifest entry on line %ld of %s:\n",
                    mfsline, filename);
            continue;
        }

        AuthFileInfo* info = new AuthFileInfo;
        info->m_filename = parts[0];
        info->m_fileSize = parts[1].toUint();
        m_files.push_back(info);
    }

    fclose(mfs);
    return e_NetSuccess;
}

uint32_t DS::AuthManifest::encodeToStream(DS::Stream* stream)
{
    uint32_t start = stream->tell();

    for (DS::AuthFileInfo* file : m_files) {
        StringBuffer<chr16_t> wstrbuf = file->m_filename.toUtf16();
        stream->writeBytes(wstrbuf.data(), wstrbuf.length() * sizeof(chr16_t));
        stream->write<chr16_t>(0);

        stream->write<uint16_t>(file->m_fileSize >> 16);
        stream->write<uint16_t>(file->m_fileSize & 0xFFFF);
        stream->write<uint16_t>(0);
    }
    stream->write<uint16_t>(0);

    DS_DASSERT((stream->tell() - start) % sizeof(chr16_t) == 0);
    return (stream->tell() - start) / sizeof(chr16_t);
}
