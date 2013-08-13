// presentFbo.vert

attribute vec2 vPosition;
attribute vec2 vTex;

varying vec2 vfTex;

uniform mat4 prmtx;

void main()
{
    vfTex = vTex;
    gl_Position = prmtx * vec4(vPosition, 0.0, 1.0);
}
