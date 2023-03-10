/* UvA Graphics and Game Technology, Shaders Assignment
 *
 * This file is NOT to be modified.
 *
 */

(function () {
    let gl = null;

    class Program {
        async init(vertexSourcePath, fragmentSourcePath) {
            let vertex = this.compileShader(gl.VERTEX_SHADER, await fetchData(vertexSourcePath, 'text'));
            let fragment = this.compileShader(gl.FRAGMENT_SHADER, await fetchData(fragmentSourcePath, 'text'));
            this.program = this.compile(vertex, fragment);

            this.use();

            return this;
        }

        use() {
            gl.useProgram(this.program);
            checkGlError('Program.use()');
        }

        drawArrays(count) {
            gl.drawArrays(gl.TRIANGLES, 0, count);
            checkGlError('Program.drawArrays()');
        }

        attribute(buffer, name, size, stride, offset) {
            buffer.bind();
            let location = gl.getAttribLocation(this.program, name);
            if (location < 0) throw new Error('Attribute location not found: ' + name);
            gl.vertexAttribPointer(location, size, gl.FLOAT, false,
                stride * Float32Array.BYTES_PER_ELEMENT, offset * Float32Array.BYTES_PER_ELEMENT);
            gl.enableVertexAttribArray(location);
            checkGlError('Program.attribute()');
        }

        setUniformMatrixData(name, matrixData) {
            let location = gl.getUniformLocation(this.program, name);
            if (location < 0) throw new Error('Uniform location not found: ' + name);
            gl.uniformMatrix4fv(location, false, matrixData);
            checkGlError('Program.setUniformMatrixData()');
        }

        setUniformFloatData(name, floatData) {
            let location = gl.getUniformLocation(this.program, name);
            if (location < 0) throw new Error('Uniform location not found: ' + name);
            gl.uniform1f(location, floatData);
            checkGlError('Program.setUniformFloatData()');
        }

        setUniformSamplerData(name, samplerData) {
            let location = gl.getUniformLocation(this.program, name);
            if (location < 0) throw new Error('Uniform location not found: ' + name);
            gl.uniform1i(location, 0);
            checkGlError('Program.setUniformSamplerData()');
        }

        compile(vertex, fragment) {
            let program = gl.createProgram();
            gl.attachShader(program, vertex);
            checkGlError('Program: gl.attachShader(vertex)');
            gl.attachShader(program, fragment);
            checkGlError('Program: gl.attachShader(fragment)');
            gl.linkProgram(program);
            checkGlError('Program: gl.linkProgram()');
            if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
                console.error('Error while linking program: ', gl.getProgramInfoLog(program));
                return null;
            }
            return program;
        }

        compileShader(type, source) {
            let shader = gl.createShader(type);
            gl.shaderSource(shader, source);
            checkGlError('Program: gl.shaderSource()');
            gl.compileShader(shader);
            checkGlError('Program: gl.compileShader()');
            if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
                console.error('Error while compiling fragment shader: ', gl.getShaderInfoLog(shader));
                return null;
            }
            return shader;
        }
    }

    class Buffer {
        constructor() {
            this.buffer = gl.createBuffer();
            this.size = 0;
        }

        bind() {
            gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);
            checkGlError('Buffer.bind()');
        }

        data(data) {
            this.bind();
            this.size = data.length;
            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data), gl.STATIC_DRAW);
            checkGlError('Buffer.data()');
        }
    }

    class Texture {
        async init(imagePath) {
            let img = new Image();
            img.src = imagePath;
            await new Promise(function(resolve, reject) {
                img.onload = resolve;
                img.onerror = reject;
            });

            this.texture = gl.createTexture();
            gl.bindTexture(gl.TEXTURE_2D, this.texture);
            gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
            checkGlError('Texture.init()');

            return this;
        }

        bind() {
            gl.activeTexture(gl.TEXTURE0);
            gl.bindTexture(gl.TEXTURE_2D, this.texture);
            checkGlError('Texture.bind()');
        }
    }

    async function fetchData(path, type) {
        let response = await fetch(path);

        if (type === 'text') {
            return await response.text();
        } else if (type === 'buffer') {
            return await response.arrayBuffer();
        } else {
            throw new Error('Unknown fetch type');
        }
    }

    let element = null;
    let width = 0, height = 0;
    let callbacks = {};

    function init() {
        element = document.querySelector('#gl-canvas');
        width = element.width;
        height = element.height;
        gl = element.getContext('webgl');
        gl.width = width;
        gl.height = height;

        setupGl();

        main(gl, callbacks, Program, Buffer, Texture, m4)

        callbacks.setup().then(() => {
            loopInternal();
        });
    }

    function setupGl() {
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.clearDepth(1.0);
        // gl.depthFunc(gl.LESS);
        gl.enable(gl.DEPTH_TEST);

        gl.enable(gl.CULL_FACE);
        // gl.cullFace(gl.BACK);
        checkGlError('setupGl()');
    }

    function loopInternal() {
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        gl.viewport(0, 0, width, height);
        checkGlError('gl.viewport()')

        callbacks.loop();
        checkGlError('loop()');

        requestAnimationFrame(loopInternal);
    }

    document.addEventListener('DOMContentLoaded', init);

    function checkGlError(debugText) {
        function getErrorString(value) {
            switch (value) {
                case gl.INVALID_ENUM:
                    return 'INVALID_ENUM';
                case gl.INVALID_VALUE:
                    return 'INVALID_VALUE';
                case gl.INVALID_OPERATION:
                    return 'INVALID_OPERATION';
                case gl.INVALID_FRAMEBUFFER_OPERATION:
                    return 'INVALID_FRAMEBUFFER_OPERATION'
                case gl.OUT_OF_MEMORY:
                    return 'OUT_OF_MEMORY'
                case gl.CONTEXT_LOST_WEBGL:
                    return 'CONTEXT_LOST_WEBGL';
                default:
                    return 'Unknown error';
            }
        }

        let error = null;
        while ((error = gl.getError()) !== gl.NO_ERROR) {
            console.error('WebGL error for "' + debugText + '": ' + getErrorString(error));
        }
    }
})();
