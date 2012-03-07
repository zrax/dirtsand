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

#include "NotifyMsg.h"

MOUL::NotifyMsg::~NotifyMsg()
{
    for (MOUL::EventData* event : m_events)
        delete event;
}

void MOUL::NotifyMsg::read(DS::Stream* stream)
{
    Message::read(stream);

    m_type = static_cast<Type>(stream->read<uint32_t>());
    m_state = stream->read<float>();
    m_id = stream->read<int32_t>();

    for (MOUL::EventData* event : m_events)
        delete event;

    m_events.resize(stream->read<uint32_t>());
    for (MOUL::EventData*& event : m_events)
        event = EventData::Read(stream);
}

void MOUL::NotifyMsg::write(DS::Stream* stream)
{
    Message::write(stream);

    stream->write<uint32_t>(m_type);
    stream->write<float>(m_state);
    stream->write<int32_t>(m_id);

    stream->write<uint32_t>(m_events.size());
    for (MOUL::EventData* event : m_events)
        EventData::Write(stream, event);
}
