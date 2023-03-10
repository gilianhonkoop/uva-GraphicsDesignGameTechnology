// UvA Graphics and Game Technology, Shaders assignment
//
// Description: Vertex and fragment shaders
// Created by Florens Douwes
//
// Student names:
// Student 1 name: ...
// Student 1 id  : ....
// Student 2 name: ....
// Student 2 id  : ....
// Date          : ....
// Comments      : ....
//
// (always fill in these fields before submitting!!)
//

precision highp float;

varying vec2 vPos;
varying float vFrames;


// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 437588.5453123);
}


// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation
    vec2 u = smoothstep(0.,1.,f);

    // Mix 4 corner percentages
    return mix(a, b, u.x) +
        (c - a)* u.y * (1.0 - u.x) +
        (d - b) * u.x * u.y;
}


float noise_with_octaves(vec2 pos) {
    // Add multiple noises togheter to make it more realistic.
    float value = 0.0;
    float c = 0.5;
    vec2 shift = vec2(100.0);
    for (int i = 0; i < 6; ++i) {
        value += c * noise(pos);
        pos = pos * 2.0 + shift;
        c *= 0.5;
    }
    return value;
}


void main(void) {
    // Let the vPos.x change when the frames change to make the stars move.
    vec2 seed = 60.0 * vec2(vPos.x - 0.008 * vFrames, vPos.y);
    float cloud_noise = noise_with_octaves(seed);

    // Only the highest noises are used to create the stars,
    // stars need to be bright and somewhat scarce.
    if (cloud_noise < 0.78) {
        cloud_noise = 0.0;
    }

    gl_FragColor = vec4(vec3(cloud_noise), 1.0);
}