#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_vertexColor;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main() {
    vec4 textColor = texture(u_Texture, v_TexCoord);
    vec4 tempColor = u_Color * textColor; // Use u_Color but don't assign it to color
    color = vec4(v_vertexColor, 1.0); // Use v_vertexColor for the final color
}