#pragma once
#include "Engine/Engine.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>

namespace VoxelEngine {
    struct BufferState;
    class Buffer;
    struct ImageState;
    class Image;

    class RenderGraph {
    public:
        class Node;
        class Edge;
        class BufferEdge;
        class ImageEdge;

        struct BufferSegment {
            vk::DeviceSize size;
            vk::DeviceSize offset;
        };

        struct ImageSegment {
            vk::ImageSubresourceRange subresource;
        };

        class BufferUsage {
            friend class Node;
            friend class BufferEdge;
        public:
            BufferUsage(Node& node, vk::AccessFlags accessMask, vk::PipelineStageFlags stageFlags);

            Node& node() const { return *m_node; }
            vk::AccessFlags accessMask() const { return m_accessMask; }
            vk::PipelineStageFlags stageFlags() const { return m_stageFlags; }

            void sync(Buffer& buffer, vk::DeviceSize size, vk::DeviceSize offset);

        private:
            Node* m_node;
            vk::AccessFlags m_accessMask;
            vk::PipelineStageFlags m_stageFlags;
            std::vector<std::unordered_map<vk::Buffer*, std::vector<BufferSegment>>> m_buffers;

            std::unordered_map<vk::Buffer*, std::vector<BufferSegment>>& getSyncs(uint32_t currentFrame) { return m_buffers[currentFrame]; }

            void clear(uint32_t currentFrame);
        };

        class ImageUsage {
            friend class Node;
            friend class ImageEdge;
        public:
            ImageUsage(Node& node, vk::ImageLayout imageLayout, vk::AccessFlags accessMask, vk::PipelineStageFlags stageFlags);

            Node& node() const { return *m_node; }
            vk::ImageLayout imageLayout() const { return m_imageLayout; }
            vk::AccessFlags accessMask() const { return m_accessMask; }
            vk::PipelineStageFlags stageFlags() const { return m_stageFlags; }

            void sync(Image& image, vk::ImageSubresourceRange subresource);

        private:
            Node* m_node;
            vk::ImageLayout m_imageLayout;
            vk::AccessFlags m_accessMask;
            vk::PipelineStageFlags m_stageFlags;
            std::vector<std::unordered_map<vk::Image*, std::vector<ImageSegment>>> m_images;

            std::unordered_map<vk::Image*, std::vector<ImageSegment>>& getSyncs(uint32_t currentFrame) { return m_images[currentFrame]; }

            void clear(uint32_t currentFrame);
        };

        class Edge {
            friend class RenderGraph;
        public:
            Edge(Node& source, Node& dest);

            Node& source() const { return *m_sourceNode; }
            Node& dest() const { return *m_destNode; }

        private:
            Node* m_sourceNode;
            Node* m_destNode;

            virtual void recordSourceBarriers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer) = 0;
            virtual void recordDestBarriers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer) = 0;
        };

        class BufferEdge : public Edge {
            friend class RenderGraph;
        public:
            BufferEdge(BufferUsage& sourceUsage, BufferUsage& destUsage);

        private:
            BufferUsage* m_sourceUsage;
            BufferUsage* m_destUsage;
            std::vector<vk::BufferMemoryBarrier> m_barriers;

            void recordSourceBarriers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer);
            void recordDestBarriers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer);
        };

        class ImageEdge : public Edge {
            friend class RenderGraph;
        public:
            ImageEdge(ImageUsage& sourceUsage, ImageUsage& destUsage);

        private:
            ImageUsage* m_sourceUsage;
            ImageUsage* m_destUsage;
            std::vector<vk::ImageMemoryBarrier> m_barriers;

            void recordSourceBarriers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer);
            void recordDestBarriers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer);
        };

        class Node {
            friend class RenderGraph;
            friend class BufferUsage;
            friend class ImageUsage;

        public:
            Node(RenderGraph& graph, const vk::Queue& queue, vk::PipelineStageFlags stages);
            virtual ~Node() = default;

            RenderGraph& graph() const { return *m_graph; }
            const vk::Queue& queue() const { return *m_queue; }
            uint32_t currentFrame() const { return m_graph->currentFrame(); }

            void addExternalWait(vk::Semaphore& semaphore, vk::PipelineStageFlags stages);
            void addExternalSignal(vk::Semaphore& semaphore);

            virtual void preRender(uint32_t currentFrame) = 0;
            virtual void render(uint32_t currentFrame, vk::CommandBuffer& commandBuffer) = 0;
            virtual void postRender(uint32_t currentFrame) = 0;

        protected:
            vk::CommandPool& commandPool() const { return *m_commandPool; }

        private:
            std::unique_ptr<vk::Semaphore> m_semaphore;

            void createCommandBuffers();
            void createSemaphore();

        private:
            const vk::Queue* m_queue;
            vk::PipelineStageFlags m_stages;
            RenderGraph* m_graph;
            std::vector<Node*> m_outputNodes;
            std::vector<BufferUsage*> m_bufferUsages;
            std::vector<ImageUsage*> m_imageUsages;
            std::vector<Edge*> m_inputEdges;
            std::vector<Edge*> m_outputEdges;

            std::unique_ptr<vk::CommandPool> m_commandPool;
            std::vector<vk::CommandBuffer> m_commandBuffers;
            vk::SubmitInfo m_submitInfo;
            vk::TimelineSemaphoreSubmitInfo m_timelineSubmitInfo;

            void addOutput(Node& output, Edge& edge);
            void addUsage(BufferUsage& usage);
            void addUsage(ImageUsage& usage);

            void makeInputTransfers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer);
            void makeOutputTransfers(uint32_t currentFrame, vk::CommandBuffer& commandBuffer);
            void clearSync(uint32_t currentFrame);
            void internalRender(uint32_t currentFrame);
            void submit(uint32_t currentFrame);
        };

        RenderGraph(vk::Device& device, uint32_t framesInFlight);
        ~RenderGraph();

        vk::Device& device() const { return *m_device; }
        uint32_t framesInFlight() const { return m_framesInFlight; }
        uint32_t currentFrame() const { return m_currentFrame; }
        uint32_t frameCount() const { return m_frameCount; }

        template<class T, class... Args>
        T& addNode(Args&&... args) {
            m_nodes.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
            return *static_cast<T*>(m_nodes.back().get());
        }

        void addEdge(BufferEdge&& edge);
        void addEdge(ImageEdge&& edge);
        void bake();
        void wait();

        void execute();

        void queueDestroy(BufferState&& state);
        void queueDestroy(ImageState&& state);

    private:
        struct SemaphoreInfo {
            std::unique_ptr<vk::Semaphore> semaphore;
            uint64_t value;
        };

        vk::Device* m_device;
        uint32_t m_framesInFlight;
        mutable uint32_t m_currentFrame;
        uint32_t m_frameCount;
        std::vector<std::unique_ptr<Node>> m_nodes;
        std::vector<std::unique_ptr<Edge>> m_edges;
        std::vector<Node*> m_nodeList;
        std::vector<SemaphoreInfo> m_semaphores;
        vk::SemaphoreWaitInfo m_semaphoreWaitInfo;

        std::queue<std::vector<BufferState>> m_bufferDestroyQueue;
        std::queue<std::vector<ImageState>> m_imageDestroyQueue;

        void makeSemaphores();
        void wait(uint32_t targetFrame);
    };
}