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

#include "AvBrainCoop.h"

void MOUL::AvBrainCoop::read(DS::Stream* s)
{
    MOUL::AvBrainGeneric::read(s);

    m_initiatorId = s->read<uint32_t>();
    m_initiatorSerial = s->read<uint16_t>();

    if (s->read<bool>())
        m_host.read(s);
    if (s->read<bool>())
        m_guest.read(s);
    m_waitingForClick = s->read<bool>();

    m_recipients.resize(s->read<uint16_t>());
    for (MOUL::Key& recip : m_recipients)
        recip.read(s);
}

void MOUL::AvBrainCoop::write(DS::Stream* s)
{
    MOUL::AvBrainGeneric::write(s);

    s->write<uint32_t>(m_initiatorId);
    s->write<uint16_t>(m_initiatorSerial);

    s->write<bool>(!m_host.isNull());
    if (!m_host.isNull())
        m_host.write(s);
    s->write<bool>(!m_guest.isNull());
    if (!m_guest.isNull())
        m_guest.write(s);
    s->write<bool>(m_waitingForClick);

    s->write<uint16_t>(m_recipients.size());
    for (MOUL::Key& recip : m_recipients)
        recip.write(s);
}
