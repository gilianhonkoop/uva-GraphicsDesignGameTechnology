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

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexture;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uNormal;

varying vec3 vColor;
varying vec2 vTexture;
varying vec3 vNormal;
varying vec3 position;

void main(void) {
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
    position = vec3(uModel * vec4(aPosition, 1.0));
    vNormal = normalize(aNormal);

    vTexture = aTexture;
}
