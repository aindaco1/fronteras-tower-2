#version 120

uniform sampler2DRect tex0;
uniform float intensity;

void main() {
    vec2 uv = gl_TexCoord[0].st;
    vec4 color = texture2DRect(tex0, uv);
    
    // Simple desaturation based on intensity
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    vec3 desaturated = mix(color.rgb, vec3(gray), intensity);
    
    // Add red tint based on intensity
    desaturated.r += intensity * 0.3;
    
    gl_FragColor = vec4(desaturated, 1.0);
}
