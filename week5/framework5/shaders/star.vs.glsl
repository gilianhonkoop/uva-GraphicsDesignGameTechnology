// UvA Graphics and Game Technology, Shaders assignment
//
// Description: Vertex and fragment shaders
// Created by Florens Douwes
//
// Student names:
// Student 1 name: Jan Deen
// Student 1 id  : 12873551
// Student 2 name: Gilian Honkoop
// Student 2 id  : 13710729
// Date          : 14-10-2022
// Comments      : Mostly used to give variables to fs.
//
// (always fill in these fields before submitting!!)
//

attribute vec2 aPosition;
uniform float frames;

varying vec2 vPos;
varying float vFrames;

void main(void) {
    // Calculate the position and give all the needed variables
    // to fs using "varying".
    gl_Position = vec4(aPosition.x, aPosition.y, 0.9999, 1.0);
    vPos = aPosition.xy;
    vFrames = frames;
}