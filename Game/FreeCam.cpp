#include "FreeCam.h"
#include <algorithm>
#include <cmath>
#include <glm/ext/quaternion_transform.hpp>
#include "BlockManager.h"

using Key = VoxelEngine::Key;
using MouseButton = VoxelEngine::MouseButton;

FreeCam::FreeCam(VoxelEngine::Camera& camera, VoxelEngine::Input& input, World& world, BlockManager& blockManager, SelectionBox& selectionBox) {
    m_camera = &camera;
    m_input = &input;
    m_world = &world;
    m_blockManager = &blockManager;
    m_selectionBox = &selectionBox;
    m_placeType = 0;

    m_look = {};
    m_position = { 0, 0, 2 };
    m_rotation = glm::identity<glm::quat>();

    m_input->onKeyChanged().connect<&FreeCam::handleKeyInput>(*this);
}

void FreeCam::setPosition(glm::vec3 pos) {
    m_position = pos;
}

void FreeCam::handleKeyInput(VoxelEngine::Key key, VoxelEngine::KeyState) {
    if (key >= VoxelEngine::Key::Key1 || key <= VoxelEngine::Key::Key9) {
        int32_t newPlaceType = static_cast<int32_t>(key) - static_cast<int32_t>(VoxelEngine::Key::Key1);

        if (newPlaceType + 1 + World::airBlock().type < m_blockManager->typeCount()) {
            m_placeType = newPlaceType;
        }
    }
}

void FreeCam::update(VoxelEngine::Clock& clock) {
    bool lockedThisFrame = false;
    const auto moveSpeed = 5.0f;

    if (!m_locked && m_input->mouseButtonDown(MouseButton::Button1)) {
        m_locked = true;
        m_input->setCursorState(VoxelEngine::CursorState::Locked);
        lockedThisFrame = true;
    }

    if (m_locked && m_input->keyDown(Key::Escape)) {
        m_locked = false;
        m_input->setCursorState(VoxelEngine::CursorState::Normal);
    }

    if (m_locked) {
        m_look += m_input->mouseDelta() * 0.05f;
        m_look.x = fmod(m_look.x, 360.0f);
        m_look.y = std::clamp<float>(m_look.y, -80, 80);

        m_rotation = glm::rotate(glm::identity<glm::quat>(), glm::radians(-m_look.x), glm::vec3{ 0, 1, 0 }) * glm::rotate(glm::identity<glm::quat>(), glm::radians(-m_look.y), glm::vec3{ 1, 0, 0 });

        glm::vec3 movement = {};

        if (m_input->keyHold(Key::W)) {
            movement += glm::vec3(0, 0, -moveSpeed);
        }

        if (m_input->keyHold(Key::S)) {
            movement += glm::vec3(0, 0, moveSpeed);
        }

        if (m_input->keyHold(Key::A)) {
            movement += glm::vec3(-moveSpeed, 0, 0);
        }

        if (m_input->keyHold(Key::D)) {
            movement += glm::vec3(moveSpeed, 0, 0);
        }

        if (m_input->keyHold(Key::E)) {
            movement += glm::vec3(0, 1, 0);
        }

        if (m_input->keyHold(Key::Q)) {
            movement += glm::vec3(0, -1, 0);
        }

        float speed = 1;

        if (m_input->keyHold(Key::LeftShift)) {
            speed = 5;
        }

        m_position += m_rotation * (speed * movement * clock.delta());
    }

    m_camera->setPosition(m_position);
    m_camera->setRotation(m_rotation);

    if (m_locked && !lockedThisFrame) {
        auto raycastResult = m_world->raycast(m_position, m_rotation * glm::vec3(0, 0, -1), 5);

        m_selectionBox->setSelection(raycastResult);

        bool remove = m_input->mouseButtonDown(VoxelEngine::MouseButton::Button1);
        bool place = m_input->mouseButtonDown(VoxelEngine::MouseButton::Button2);

        if (raycastResult) {
            if (remove && !place) {
                raycastResult->chunk->queueBlockUpdate({ World::airBlock(), Chunk::worldToChunk(raycastResult->blockPosition) });
            } else if(!remove && place) {
                Block block = Block(World::airBlock().type + 1 + m_placeType);
                raycastResult->chunk->queueBlockUpdate({ block, Chunk::worldToChunk(raycastResult->blockPosition + raycastResult->normal) });
            }
        }
    }
}

void FreeCam::setCurrentBlockType(int32_t type)
{
    if (type < m_blockManager->typeCount())
    {
        m_placeType = type;
    }
}
