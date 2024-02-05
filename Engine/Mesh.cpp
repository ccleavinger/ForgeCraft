#include "Engine/Mesh.h"

using namespace VoxelEngine;

Mesh::Mesh() {
    m_indexCount = 0;
    m_vertexCount = 0;
    m_vertexOffset = 0;
}

void Mesh::addBinding(const std::shared_ptr<Buffer>& buffer, vk::DeviceSize offset) {
    m_bindings.push_back(buffer);
    m_buffers.push_back(buffer->buffer());
    m_offsets.push_back(offset);
}

void Mesh::setIndexBuffer(const std::shared_ptr<Buffer>& buffer, vk::IndexType type, vk::DeviceSize offset) {
    m_indexBuffer = buffer;
    m_indexType = type;
    m_indexOffset = offset;
}

void Mesh::clearBindings() {
    m_bindings.clear();
    m_buffers.clear();
    m_offsets.clear();
}

void Mesh::clearIndexBuffer() {
    m_indexBuffer.reset();
}

void Mesh::bindVertexBuffers(vk::CommandBuffer& commandBuffer) {
    commandBuffer.bindVertexBuffers(0, m_buffers, m_offsets);
}

void Mesh::bindIndexBuffer(vk::CommandBuffer& commandBuffer) {
    commandBuffer.bindIndexBuffer(m_indexBuffer->buffer(), 0, m_indexType);
}

void Mesh::draw(vk::CommandBuffer& commandBuffer, uint32_t vertexCount) const {
    commandBuffer.draw(vertexCount, 1, m_vertexOffset, 0);
}

void Mesh::drawIndexed(vk::CommandBuffer& commandBuffer, uint32_t indexCount) const {
    commandBuffer.drawIndexed(indexCount, 1, 0, m_vertexOffset, 0);
}

void Mesh::draw(vk::CommandBuffer& commandBuffer) const {
    draw(commandBuffer, m_vertexCount);
}

void Mesh::drawIndexed(vk::CommandBuffer& commandBuffer) const {
    drawIndexed(commandBuffer, m_indexCount);
}