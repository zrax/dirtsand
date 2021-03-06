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

#ifndef _MOUL_AVBRAINCOOP_H
#define _MOUL_AVBRAINCOOP_H

#include "AvBrainGeneric.h"
#include "Key.h"
#include <vector>

namespace MOUL
{
    class AvBrainCoop : public AvBrainGeneric
    {
        FACTORY_CREATABLE(AvBrainCoop)

        void read(DS::Stream* stream) override;
        void write(DS::Stream* stream) const override;

    public:
        uint32_t m_initiatorId;
        uint16_t m_initiatorSerial;
        Key m_host, m_guest;
        bool m_waitingForClick;
        std::vector<Key> m_recipients;

    protected:
        AvBrainCoop(uint16_t type)
            : AvBrainGeneric(type), m_initiatorId(), m_initiatorSerial(),
              m_waitingForClick() { }
    };
};

#endif // _MOUL_AVBRAINCOOP_H
