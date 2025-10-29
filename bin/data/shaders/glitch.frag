#version 120

uniform sampler2DRect tex0;
uniform float intensity;
uniform float time;

// Simple random function
float random(float x) {
    return fract(sin(x * 12.9898) * 43758.5453);
}

void main() {
    vec2 uv = gl_TexCoord[0].st;
    
    // VHS-style chromatic aberration - increases with intensity
    float offset = intensity * 50.0;
    float r = texture2DRect(tex0, uv + vec2(offset, 0.0)).r;
    float g = texture2DRect(tex0, uv).g;
    float b = texture2DRect(tex0, uv - vec2(offset, 0.0)).b;
    
    // Wide glitch bands with randomness
    float bandSize = 40.0; // Wider bands
    float bandY = floor(uv.y / bandSize);
    float bandRandom = random(bandY + floor(time * 3.0));
    
    // Create glitch on random bands
    float isGlitchBand = step(0.7 - intensity * 0.3, bandRandom);
    float displacement = isGlitchBand * intensity * 200.0 * (random(bandY) - 0.5);
    
    // Vertical hold shift (VHS tracking issues)
    float vholdShift = sin(time * 2.0 + uv.y * 0.01) * intensity * intensity * 100.0;
    
    vec2 glitchUV = uv + vec2(displacement + vholdShift, 0.0);
    vec4 glitchColor = texture2DRect(tex0, glitchUV);
    
    // Mix RGB split with glitched image
    vec3 finalColor = mix(vec3(r, g, b), glitchColor.rgb, isGlitchBand * intensity);
    
    // VHS color bleed / saturation boost
    finalColor = mix(finalColor, finalColor * 1.3, intensity * 0.3);
    
    // Scan lines
    float scanline = sin(uv.y * 1.5) * 0.04 * intensity;
    finalColor -= scanline;
    
    // Static noise
    float noise = random(uv.y * time) * 0.05 * intensity;
    finalColor += noise;
    
    gl_FragColor = vec4(finalColor, 1.0);
}
