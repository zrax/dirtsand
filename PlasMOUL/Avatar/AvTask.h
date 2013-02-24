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

#ifndef _MOUL_AVTASK_H
#define _MOUL_AVTASK_H

#include "ArmatureBrain.h"

namespace MOUL
{
    class AvTask : public Creatable
    {
    protected:
        AvTask(uint16_t type) : Creatable(type) { }
    };

    class AvAnimTask : public AvTask
    {
        FACTORY_CREATABLE(AvAnimTask)

        virtual void read(DS::Stream* stream);
        virtual void write(DS::Stream* stream);

    public:
        DS::String m_animName;
        float m_initialBlend, m_targetBlend;
        float m_fadeSpeed, m_setTime;
        bool m_start, m_loop, m_attach;

    protected:
        AvAnimTask(uint16_t type) : AvTask(type) { }
    };

    class AvOneShotTask : public AvTask
    {
        FACTORY_CREATABLE(AvOneShotTask)

    protected:
        AvOneShotTask(uint16_t type) : AvTask(type) { }
    };

    class AvOneShotLinkTask : public AvOneShotTask
    {
        FACTORY_CREATABLE(AvOneShotLinkTask)

        virtual void read(DS::Stream* stream);
        virtual void write(DS::Stream* stream);

    public:
        DS::String m_animName, m_markerName;

    protected:
        AvOneShotLinkTask(uint16_t type) : AvOneShotTask(type) { }
    };

    class AvSeekTask : public AvTask
    {
        FACTORY_CREATABLE(AvSeekTask)

    protected:
        AvSeekTask(uint16_t type) : AvTask(type) { }
    };

    class AvTaskBrain : public AvTask
    {
        FACTORY_CREATABLE(AvTaskBrain)

        virtual void read(DS::Stream* stream);
        virtual void write(DS::Stream* stream);

    public:
        ArmatureBrain* m_brain;

    protected:
        AvTaskBrain(uint16_t type) : AvTask(type), m_brain(nullptr) { }

        virtual ~AvTaskBrain()
        {
            m_brain->unref();
        }
    };

    class AvTaskSeek : public AvTask
    {
        FACTORY_CREATABLE(AvTaskSeek)

    protected:
        AvTaskSeek(uint16_t type) : AvTask(type) { }
    };
}

#endif
