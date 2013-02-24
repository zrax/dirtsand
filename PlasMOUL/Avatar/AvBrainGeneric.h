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

#ifndef _MOUL_AVBRAINGENERIC_H
#define _MOUL_AVBRAINGENERIC_H

#include "ArmatureBrain.h"
#include "AnimStage.h"
#include "Messages/Message.h"

namespace MOUL
{
    class AvBrainGeneric : public ArmatureBrain
    {
        FACTORY_CREATABLE(AvBrainGeneric)

        virtual void read(DS::Stream* stream);
        virtual void write(DS::Stream* stream);

    public:
        std::vector<AnimStage*> m_stages;
        int32_t m_curStage;
        uint32_t m_type, m_exitFlags;
        uint8_t m_mode;
        bool m_forward;
        Message* m_startMessage;
        Message* m_endMessage;
        float m_fadeIn, m_fadeOut;
        uint8_t m_moveMode, m_bodyUsage;
        Key m_recipient;

    protected:
        AvBrainGeneric(uint16_t type)
            : ArmatureBrain(type), m_startMessage(nullptr), m_endMessage(nullptr) { }
        virtual ~AvBrainGeneric();
    };
}

#endif
