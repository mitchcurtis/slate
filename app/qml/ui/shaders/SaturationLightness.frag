/*
    Copyright 2021, Mitch Curtis

    This file is part of Slate.

    Slate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slate. If not, see <http://www.gnu.org/licenses/>.
*/

#version 450

/*
    The .qsb is generated with this command:

    qsb --glsl "100 es,120,150" --hlsl 50 --msl 12 -o SaturationLightness.frag.qsb SaturationLightness.frag

    More info:
    - https://doc-snapshots.qt.io/qt6-dev/qml-qtquick-shadereffect.html#migrating-from-qt-5
    - https://doc-snapshots.qt.io/qt6-dev/qtshadertools-qsb.html#example
*/

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float hue;
};

float hueToIntensity(float v1, float v2, float h) {
    h = fract(h);
    if (h < 1.0 / 6.0)
        return v1 + (v2 - v1) * 6.0 * h;
    else if (h < 1.0 / 2.0)
        return v2;
    else if (h < 2.0 / 3.0)
        return v1 + (v2 - v1) * 6.0 * (2.0 / 3.0 - h);

    return v1;
}

vec3 HSLtoRGB(vec3 color) {
    float h = color.x;
    float l = color.z;
    float s = color.y;

    if (s < 1.0 / 256.0)
        return vec3(l, l, l);

    float v1;
    float v2;
    if (l < 0.5)
        v2 = l * (1.0 + s);
    else
        v2 = (l + s) - (s * l);

    v1 = 2.0 * l - v2;

    float d = 1.0 / 3.0;
    float r = hueToIntensity(v1, v2, h + d);
    float g = hueToIntensity(v1, v2, h);
    float b = hueToIntensity(v1, v2, h - d);
    return vec3(r, g, b);
}

void main() {
    vec4 c = vec4(1.0);
    c.rgb = HSLtoRGB(vec3(hue, 1.0 - qt_TexCoord0.t, qt_TexCoord0.s));
    fragColor = c * qt_Opacity;
}
