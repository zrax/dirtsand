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

#include "AvBrainGeneric.h"
#include "factory.h"

MOUL::AvBrainGeneric::~AvBrainGeneric()
{
    for (MOUL::AnimStage* stage : m_stages)
        stage->unref();
    m_startMessage->unref();
    m_endMessage->unref();
}

void MOUL::AvBrainGeneric::read(DS::Stream* stream)
{
    MOUL::ArmatureBrain::read(stream);

    for (MOUL::AnimStage* stage : m_stages)
        stage->unref();

    m_stages.resize(stream->read<uint32_t>());
    for (MOUL::AnimStage*& stage : m_stages) {
        stage = Factory::Read<AnimStage>(stream);
        stage->readAux(stream);
    }
    m_curStage = stream->read<int32_t>();

    m_type = stream->read<uint32_t>();
    m_exitFlags = stream->read<uint32_t>();
    m_mode = stream->read<uint8_t>();
    m_forward = stream->read<bool>();

    m_startMessage->unref();
    if (stream->read<bool>())
        m_startMessage = Factory::Read<Message>(stream);
    else
        m_startMessage = nullptr;

    m_endMessage->unref();
    if (stream->read<bool>())
        m_endMessage = Factory::Read<Message>(stream);
    else
        m_endMessage = nullptr;

    m_fadeIn = stream->read<float>();
    m_fadeOut = stream->read<float>();
    m_moveMode = stream->read<uint8_t>();
    m_bodyUsage = stream->read<uint8_t>();
    m_recipient.read(stream);
}

void MOUL::AvBrainGeneric::write(DS::Stream* stream)
{
    MOUL::ArmatureBrain::write(stream);

    stream->write<uint32_t>(m_stages.size());
    for (MOUL::AnimStage* stage : m_stages) {
        Factory::WriteCreatable(stream, stage);
        stage->writeAux(stream);
    }
    stream->write<int32_t>(m_curStage);

    stream->write<uint32_t>(m_type);
    stream->write<uint32_t>(m_exitFlags);
    stream->write<uint8_t>(m_mode);
    stream->write<bool>(m_forward);

    Factory::WriteCreatable(stream, m_startMessage);
    Factory::WriteCreatable(stream, m_endMessage);

    stream->write<float>(m_fadeIn);
    stream->write<float>(m_fadeOut);
    stream->write<uint8_t>(m_moveMode);
    stream->write<uint8_t>(m_bodyUsage);
    m_recipient.write(stream);
}
