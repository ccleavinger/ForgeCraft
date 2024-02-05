#pragma once
#include <stdint.h>
#include <vector>
#include "Engine/Clock.h"

namespace VoxelEngine {
    class System;
    class SystemGroup;

    class System {
    public:
        System();

        void setPriority(int32_t priority);
        int32_t getPriority() const;

        virtual void update(Clock& clock) = 0;

    private:
        int32_t m_priority;
        SystemGroup* m_group;
    };

    class SystemGroup {
    public:
        SystemGroup(Clock& clock);

        void add(System& system, uint32_t priority);
        void remove(System& remove);
        void update();

    private:
        Clock* m_clock;
        bool m_dirty;
        std::vector<System*> m_systems;

        void setDirty();

        friend class System;
    };
}