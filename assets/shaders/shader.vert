#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in ivec3 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in ivec3 vUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragUV;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform Transform {
	ivec4 pos;
} transform;

void main() {
    gl_Position = ubo.proj * ubo.view * vec4(vPosition + transform.pos.xyz, 1.0);
    fragColor = vColor.xyz;
    fragUV = vUV;
}