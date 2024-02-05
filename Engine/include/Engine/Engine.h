#pragma once
#include "Engine/Buffer.h"
#include "Engine/Camera.h"
#include "Engine/Clock.h"
#include "Engine/Mesh.h"
#include "Engine/Graphics.h"
#include "Engine/Image.h"
#include "Engine/Input.h"
#include "Engine/MemoryManager.h"
#include "Engine/Mesh.h"
#include "Engine/RenderGraph/RenderGraph.h"
#include "Engine/System.h"
#include "Engine/Window.h"
#include "FreeListAllocator.h"

namespace VoxelEngine {
    class RenderGraph;

    class Engine {
    public:
        Engine();
        ~Engine();

        Graphics& getGraphics() { return *m_graphics; }
        SystemGroup& getUpdateGroup() { return *m_updateGroup; }
        RenderGraph& renderGraph() { return *m_renderGraph; }

        void run();

        void addWindow(Window& window);
        void setRenderGraph(RenderGraph& renderGraph);

    private:
        Window* m_window;
        RenderGraph* m_renderGraph;

        std::unique_ptr<SystemGroup> m_updateGroup;
        std::unique_ptr<Clock> m_updateClock;
        std::unique_ptr<Graphics> m_graphics;
    };
}