/*
 * Student names: Jan Deen, Gilian Honkoop
 * Student numbers: 12873551, 13710729
 *
 */

function myLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ) {

    // We assume the camera is looking at the center.
    // Calculate eye - lookat and normalize it.
    var t = [eyeX - centerX, eyeY - centerY, eyeZ - centerZ];
    tLength = vectorLength(t);
    var w =  [t[0]/tLength, t[1]/tLength, t[2]/tLength];

    // Calculate the cross product of up and eye-lookat, normalize it.
    var p = crossProduct([upX, upY, upZ], w);
    pLength = vectorLength(p);
    var u = [p[0]/pLength, p[1]/pLength, p[2]/pLength];

    // Calculate the cross product of the previous two vectors.
    var v = crossProduct(w, u);

    var mat = [
        u[0], v[0], w[0], 0.0,
        u[1], v[1], w[1], 0.0,
        u[2], v[2], w[2], 0.0,
        0.0, 0.0, 0.0, 1.0
    ];

    // Calculate the translation values after multiplying with mat.
    mat[12] = u[0] * -eyeX + u[1] * -eyeY + u[2] * -eyeZ;
    mat[13] = v[0] * -eyeX + v[1] * -eyeY + v[2] * -eyeZ;
    mat[14] = w[0] * -eyeX + w[1] * -eyeY + w[2] * -eyeZ;

    return mat;
}

function vectorLength(v) {
    return Math.sqrt(v[0]**2 + v[1]**2 + v[2]**2);
}

function crossProduct(v, w) {
    return ([v[1]*w[2]-v[2]*w[1], v[2]*w[0]-v[0]*w[2], v[0]*w[1]-v[1]*w[0]])
}