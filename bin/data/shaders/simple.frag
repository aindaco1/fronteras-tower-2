#version 120

uniform sampler2DRect tex0;

void main() {
    vec2 uv = gl_TexCoord[0].st;
    vec4 color = texture2DRect(tex0, uv);
    
    // Just make everything more red
    color.r += 0.5;
    
    gl_FragColor = color;
}
