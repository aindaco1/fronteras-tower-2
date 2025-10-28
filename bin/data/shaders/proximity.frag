#version 150

uniform sampler2D tex0;
uniform float intensity;
uniform float time;

in vec2 texCoordVarying;
out vec4 fragColor;

void main() {
    vec2 uv = texCoordVarying;
    
    vec2 offset = vec2(
        sin(time + uv.y * 10.0) * 0.01 * intensity,
        cos(time + uv.x * 10.0) * 0.01 * intensity
    );
    
    vec4 color = texture(tex0, uv + offset);
    
    float desaturate = 1.0 - (intensity * 0.7);
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    color.rgb = mix(color.rgb, vec3(gray), desaturate);
    
    fragColor = color;
}
