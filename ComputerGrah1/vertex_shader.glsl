#version 410 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec3 NormalOut;

void main() {
    FragPos = vec3(model * vec4(Position, 1.0));
    NormalOut = mat3(transpose(inverse(model))) * Normal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
