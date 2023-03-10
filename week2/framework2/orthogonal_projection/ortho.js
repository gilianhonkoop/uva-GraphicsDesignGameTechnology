/*
 * Student names: Jan Deen, Gilian Honkoop
 * Student numbers: 12873551, 13710729
 *
 */

function myOrtho(left, right, bottom, top, near, far) {

    var mat = [
        1.0 ,0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    ];

    mat[0] = 2 / (right - left);
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = 0;
    mat[4] = 0;
    mat[5] = 2 / (top - bottom);
    mat[6] = 0;
    mat[7] = 0;
    mat[8] = 0;
    mat[9] = 0;
    mat[10] = 2 / (near - far);
    mat[11] = 0;
    mat[12] = (left + right) / (left - right);
    mat[13] = (bottom + top) / (bottom - top);
    mat[14] = (near + far) / (near - far);
    mat[15] = 1;

    return mat;
}
