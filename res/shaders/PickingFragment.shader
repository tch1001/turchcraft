#version 330 core

layout(location = 0) out uvec4 color;

uniform uint u_Id;
in float u_FaceId;

void main() {
    color = uvec4(1.0, u_Id, uint(u_FaceId), 1);
}