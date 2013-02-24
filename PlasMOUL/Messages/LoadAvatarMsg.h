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

#ifndef _MOUL_LOADAVATARMSG_H
#define _MOUL_LOADAVATARMSG_H

#include "LoadCloneMsg.h"
#include "Avatar/AvTask.h"

namespace MOUL
{
    class LoadAvatarMsg : public LoadCloneMsg
    {
        FACTORY_CREATABLE(LoadAvatarMsg)

        virtual void read(DS::Stream* stream);
        virtual void write(DS::Stream* stream);

    public:
        bool m_isPlayer;
        Key m_spawnPoint;
        AvTask* m_initTask;
        DS::String m_userString;

    protected:
        LoadAvatarMsg(uint16_t type)
            : LoadCloneMsg(type), m_isPlayer(true), m_initTask(nullptr) { }

        virtual ~LoadAvatarMsg()
        {
            m_initTask->unref();
        }
    };
}

#endif
