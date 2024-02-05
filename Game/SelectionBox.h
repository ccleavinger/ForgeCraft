#pragma once
#include <VulkanWrapper/VulkanWrapper.h>
#include <Engine/Engine.h>
#include <Engine/RenderGraph/TransferNode.h>
#include <Engine/CameraSystem.h>
#include "World.h"

class MeshManager;

class SelectionBox {
public:
    SelectionBox(VoxelEngine::Engine& engine, VoxelEngine::CameraSystem& cameraSystem);

    VoxelEngine::Image& image() { return *m_image; }
    vk::Sampler& sampler() { return *m_sampler; }
    vk::DescriptorSetLayout& descriptorSetLayout() { return *m_descriptorSetLayout; }
    vk::DescriptorSet& descriptorSet() { return *m_descriptorSet; }
    VoxelEngine::Mesh& mesh() { return *m_mesh; }

    void transfer(VoxelEngine::TransferNode& transferNode);
    void createPipeline(vk::RenderPass& renderPass);
    void createMesh(VoxelEngine::TransferNode& transferNode, MeshManager& meshManager);
    void setSelection(std::optional<RaycastResult>& raycast);

    void draw(vk::CommandBuffer& commandBuffer, vk::Viewport viewport, vk::Rect2D scissor);

private:
    VoxelEngine::Engine* m_engine;
    VoxelEngine::CameraSystem* m_cameraSystem;

    std::unique_ptr<VoxelEngine::Mesh> m_mesh;
    std::unique_ptr<VoxelEngine::Image> m_image;
    std::unique_ptr<vk::ImageView> m_imageView;
    std::unique_ptr<vk::Sampler> m_sampler;
    std::unique_ptr<vk::DescriptorSetLayout> m_descriptorSetLayout;
    std::unique_ptr<vk::DescriptorPool> m_descriptorPool;
    std::unique_ptr<vk::DescriptorSet> m_descriptorSet;
    std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
    std::unique_ptr<vk::Pipeline> m_pipeline;

    std::optional<RaycastResult> m_selection;

    void createTexture();
    void createImageView();
    void createSampler();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDecriptorSet();
    void writeDescriptorSet();
    void createPipelineLayout();
};