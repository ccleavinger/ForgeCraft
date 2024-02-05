#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragUV;

layout(location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler textureSampler;
layout (set = 1, binding = 1) uniform texture2DArray textureArray;

void main() {
    const float fogNear = 7.5; // distance from the camera where fog starts
    const float fogFar = 35.0; // distance from the camera where fog ends
    const vec3 fogColor = vec3(0.75, 0.75, 0.75); // color of the fog

    // calculate fog factor based on frag depth
    float fogFactor = (gl_FragCoord.z / gl_FragCoord.w - fogNear) / (fogFar - fogNear);
    fogFactor = clamp(fogFactor, 0.0, 1.0); // clamp to 0-1 range

    // apply fog to the fragment color
    vec3 fogBlendColor = mix(fragColor, fogColor, fogFactor);

    // sample texture and apply fog
    vec4 textureColor = texture(sampler2DArray(textureArray, textureSampler), fragUV);
    outColor = vec4(textureColor.rgb * fogBlendColor, textureColor.a);
}
