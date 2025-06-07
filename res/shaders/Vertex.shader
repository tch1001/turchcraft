#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 vertexColor;
layout (location = 3) in float face;

out vec2 v_TexCoord;
out vec3 v_vertexColor;
out float u_FaceId;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * position;
    v_TexCoord = texCoord;
    v_vertexColor = vertexColor;
    u_FaceId = face;
}
