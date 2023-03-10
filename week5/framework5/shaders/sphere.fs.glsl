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


// zelf
uniform sampler2D uSampler;

varying vec2 vTexture;
varying vec3 vNormal;
varying vec3 position;

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
    vec3 ambientLight = vec3(0.2, 0.2, 0.2);
    vec3 lightPosition = vec3(7.0, 2.0, 6.0);
    vec3 lightColor = vec3(0.6, 0.6, 0.6);
    vec3 objectColor = vec3(1.0, 1.0, 1.0);
    float specularStrength = 0.5;

    vec3 l = normalize(lightPosition - position);
    float dot_ln = dot(l, vNormal);

    dot_ln = clamp(dot_ln, 0.0, 1.0);

    vec3 diffuse = lightColor * dot_ln;
    vec3 specular = lightColor * pow(dot_ln, 64.0) * specularStrength;
    vec3 vColor = (ambientLight + diffuse + specular) * objectColor;

    gl_FragColor = vec4(vColor, 1.0);

    vec4 color = texture2D(uSampler, vTexture);
    vec2 seed = 60.0 * vec2(vTexture.x, 0.5 * vTexture.y);

    float cloud_noise = noise_with_octaves(seed);

    if (cloud_noise < 0.5) {
        cloud_noise = 0.0;
    }
    else {
        cloud_noise -= 0.5;
    }
    gl_FragColor = gl_FragColor * color + vec4(vec3(cloud_noise), 0.0);


}