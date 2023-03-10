/*
 * Student names: Jan Deen & Gilian Honkoop
 * Student numbers: 12873551 & 13710729
 *
 */

function myTranslate(x, y, z) {
    // Translate by x, y and z.
    var mat = [
               1.0, 0.0, 0.0, 0.0,
               0.0, 1.0, 0.0, 0.0,
               0.0, 0.0, 1.0, 0.0,
               x, y, z, 1.0
              ];

   return mat;
}

function myScale(x, y, z) {
    // Scale by x, y and z.
    var mat = [
               x, 0.0, 0.0, 0.0,
               0.0, y, 0.0, 0.0,
               0.0, 0.0, z, 0.0,
               0.0, 0.0, 0.0, 1.0
              ];

   return mat;
}

function myRotate(angle, x, y, z) {
    // Rotate by angle around [x, y, z]^T.

    //
    // 1. Create the orthonormal basis
    //

    //
    // 2. Set up the three matrices making up the rotation
    //

    // console.log(x, y, z);

    var u = [];
    var v = [];

    // Normalize vector w.
    var wLength = vectorLength([x,y,z]);
    var w = [x/wLength, y/wLength, z/wLength];

    var closest = 0;

    // Get the index of the number in the array closest to 0.
    if (Math.abs(w[1]) < Math.abs(w[0])) {
        closest = 1;
    }
    if (Math.abs(w[2]) < Math.abs(w[closest])) {
        closest = 2;
    }

    // Get vector orthonorgal to w and make it length 1.
    var t = [x/wLength, y/wLength, z/wLength];
    t[closest] = 1;
    u = crossProduct(t,w);
    var uLength = vectorLength(u);
    u = [u[0]/uLength, u[1]/uLength, u[2]/uLength];

    // Get orthonorgal vector to w and u, which is automatically length 1.
    v = crossProduct(w, u);

    var A = [
            u[0], u[1], u[2], 0.0,
            v[0], v[1], v[2], 0.0,
            w[0], w[1], w[2], 0.0,
            0.0, 0.0, 0.0, 1.0
            ];

    var B = [
             Math.cos(angle), Math.sin(angle), 0.0, 0.0,
             -Math.sin(angle), Math.cos(angle), 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.0, 0.0, 0.0, 1.0
            ];

    var C = [
             u[0], v[0], w[0], 0.0,
             u[1], v[1], w[1], 0.0,
             u[2], v[2], w[2], 0.0,
             0.0, 0.0, 0.0, 1.0
            ];

    // Calculate rotation matrix
    var mat = m4.multiply(A, m4.multiply(B, C));
    return mat;
}

function vectorLength(v) {
    return Math.sqrt(v[0]**2 + v[1]**2 + v[2]**2);
}

function crossProduct(v, w) {
    return ([v[1]*w[2]-v[2]*w[1], v[2]*w[0]-v[0]*w[2], v[0]*w[1]-v[1]*w[0]])
}