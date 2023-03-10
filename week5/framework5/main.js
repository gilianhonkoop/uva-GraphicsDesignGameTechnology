/* UvA Graphics and Game Technology, Shaders assignment
 *
 * Description: Vertex and fragment shaders
 * Created by Florens Douwes
 *
 * Student names:
 * Student 1 name: ...
 * Student 1 id  : ....
 * Student 2 name: ....
 * Student 2 id  : ....
 * Date          : ....
 * Comments      : ....
 *
 * (always fill in these fields before submitting!!)
 *
 */

let main = function (gl, callbacks, Program, Buffer, Texture, m4) {
    let frames = 0;

    let triangleProgram = null;
    let triangleBuffer = null;

    let projMatrix = m4.identity();
    let modelMatrix = m4.identity();
    let viewMatrix = m4.identity();
    let normalMatrix = m4.identity();

    // The first-time setup, called once to set up programs, buffers, etc.
    callbacks.setup = async function () {
        triangleProgram = await new Program().init('shaders/sphere.vs.glsl', 'shaders/sphere.fs.glsl');

        // Create a WebGL buffer
        triangleBuffer = new Buffer();

        // The data intertwined per point, and is laid out as follows:
        // 3 floats for the x, y, and z position of the vertex.
        // 3 floats for the x, y, and z of the normal vector.
        // 2 floats for the u, v coordinates of the texture.
        // triangleBuffer.data(createTriangle());
        // createSphere()
        triangleBuffer.data(createSphere());

        // Load the image.
        sphereTexture = await new Texture().init('images/earthmap1k.jpg');

        starsProgram = await new Program().init('shaders/star.vs.glsl', 'shaders/star.fs.glsl');
        starsBuffer = new Buffer();
        starsBuffer.data(createPlane());
    }

    function createPlane() {
        // 3x pos, 3x normal, 2x texture coordinates,
        // and three triangle points in total.
        return [
            -1.0, -1.0, 0.0, -1.0, -1.0, 0.0, 0.0, 0.0,
            1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0,
            -1.0, 1.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0,

            -1.0, 1.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0,
            1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0,
            1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0,

        ]
    }

    function createTriangle() {
        // 3x pos, 3x normal, 2x texture coordinates,
        // and three triangle points in total.
        return [
            0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
            1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0,
            -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 1.0,
        ]
    }


    function createSphere() {
        var buff = []
        var steps = 20

        // for (let phi = 0.0; phi < 2*Math.PI + Math.PI/steps; phi += Math.PI/steps) {
        //     for (let theta = 0.0; theta < 2 * Math.PI + Math.PI/steps; theta += Math.PI/steps) {
        //         buff = buff.concat(get_the8(phi, theta))
        //     }
        // }

        // s steps result in s+1 points.
        for (let i = 0; i <= steps; i += 1) {
            for (let j = 0; j < steps; j += 1) {
                buff = buff.concat(get_the8(i, j, steps))
            }
        }

        buff_triangles = points_to_triangles(buff, steps)
        return buff_triangles
    }


    function get_the8(i, j, steps) {
        var r = 2;
        phi = 2 * Math.PI * i / steps ;
        theta = 2 * Math.PI * j / steps ;


        var x = r  * Math.cos(phi) * Math.sin(theta)
        var y = r * Math.cos(theta)
        var z = r * Math.sin(phi) * Math.sin(theta)

        return [x, y, z, x, y, z, 1 - i/steps, 2 * (j/(steps-1)) - 1]
        // return [x, y, z, x, y, z, 1 - i/steps, (0.5*steps - j)/(0.5*steps)]
        // return [x, y, z, x, y, z, 1 - (phi / (2*Math.PI), (Math.PI - theta)/Math.PI)]
    }


    function get_the_array(buff, place) {
        place = place * 8
        the_array = []
        for (let i = 0; i < 8; i++) {
            the_array = the_array.concat(buff[place + i])
        }

        return the_array
    }


    function add_triangles_to_buffer(buff_triangles, buff_points, i, j, steps) {
        // The order of point is important!
        buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps + j))
        buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps + j + 1))
        buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps + j))


        buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps + j + 1))
        buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps + j + 1))
        buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps + j))

        // buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps + j))
        // buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i+1) * steps + j))
        // buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps + j+1))


        // buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps + j))
        // buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps + j + 1))
        // buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i) * steps + j + 1))

        return buff_triangles
    }


    function points_to_triangles(buff_points, steps) {
        buff_triangles = []

        for (let i = 0; i < steps; i++) {
            for (let j = 0; j < steps; j++) {
                buff_triangles = add_triangles_to_buffer(buff_triangles, buff_points, i, j, steps)
            }

            // Drawing the triangles that connect the two sides.
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps + steps - 1))


            buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, (i + 1) * steps + steps - 1))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, i * steps + steps - 1))

        }


        for (let j = 0; j < 2 * steps; j++) {
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, j))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, steps *  steps + j))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, j + 1))


            buff_triangles = buff_triangles.concat(get_the_array(buff_points, j + 1))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, steps * steps + j))
            buff_triangles = buff_triangles.concat(get_the_array(buff_points, steps * steps + j + 1))
        }

        // misschien mist er nog 1?


        return buff_triangles
    }


    // The loop is called 60 times a second. Redraw the scene here.
    callbacks.loop = function () {
        // Set projection matrix
        m4.perspective(90.0 / (gl.width / gl.height) * (Math.PI / 180),
            gl.width / gl.height, 0.1, 100.0, projMatrix);

        // Set view (camera) matrix
        m4.identity(viewMatrix);
        // Moves the camera back 5 units.
        m4.translate(viewMatrix, 0.0, 0.0, -5.0, viewMatrix);

        // Set object matrix
        m4.identity(modelMatrix);
        // Rotate based on the frames rendered.
        m4.yRotate(modelMatrix, frames * 0.01, modelMatrix);

        // Set normal matrix
        m4.inverse(modelMatrix, normalMatrix);
        m4.transpose(modelMatrix, normalMatrix);

        triangleProgram.use();

        // Set the (modified) matrices once at the start of the draw cycle.
        triangleProgram.setUniformMatrixData("uProj", projMatrix);
        triangleProgram.setUniformMatrixData("uModel", modelMatrix);
        triangleProgram.setUniformMatrixData("uView", viewMatrix);
        triangleProgram.setUniformMatrixData("uNormal", normalMatrix);

        // zelf
        triangleProgram.setUniformSamplerData("uSampler", sphereTexture);

        triangleBuffer.bind();
        // Tell the program what the data in the buffer means.
        // The three float arguments are the size, stride and offset of the buffer.
        // The stride is 8 float values, as there are 8 values in total per vertex.
        // The offset is 0, 3 and 6 respectively, as the starting index of the buffer.
        triangleProgram.attribute(triangleBuffer, "aPosition", 3, 8, 0);

        // aNormal and aTexture are currently not used in the shader, and therefore disabled here
        // to avoid errors. Enable them when you use them in the shader:
        triangleProgram.attribute(triangleBuffer, "aNormal", 3, 8, 3);
        triangleProgram.attribute(triangleBuffer, "aTexture", 2, 8, 6);


        // There are 8 floats per point.
        triangleProgram.drawArrays(triangleBuffer.size / 8);

        starsProgram.use();
        starsBuffer.bind();

        starsProgram.setUniformFloatData("frames", frames);
        starsProgram.attribute(starsBuffer, "aPosition", 3, 8, 0);
        starsProgram.drawArrays(starsBuffer.size / 8);

        frames++;
    }
}
