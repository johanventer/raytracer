#version 410 core

in vec2 uv;
out vec4 outColor;

uniform sampler2D sampledColor;

void main() {
  vec4 colorAndSample = texture(sampledColor, uv);
  outColor = sqrt(vec4(colorAndSample.rgb, 1) / colorAndSample.a);
}
