import QtQuick 2.12
import App 1.0

SaturationLightnessPickerTemplate {
    id: control
    implicitWidth: Math.max(background ? background.implicitWidth : 0, contentItem.implicitWidth)
    implicitHeight: Math.max(background ? background.implicitHeight : 0, contentItem.implicitHeight)

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 200
        color: control.visualFocus ? (control.pressed ? "#cce0ff" : "#f0f6ff") : (control.pressed ? "#d6d6d6" : "#f6f6f6")
        border.color: "#353637"
    }

    contentItem: Rectangle {
        ShaderEffect {
            id: map
            width: 64
            height: 64
            scale: contentItem.width / width
            layer.enabled: true
            layer.smooth: true
            anchors.centerIn: parent
            property alias hue: control.hue

            fragmentShader: contentItem.OpenGLInfo.profile === OpenGLInfo.CoreProfile ? "#version 150
            in vec2 qt_TexCoord0;
            uniform float qt_Opacity;
            uniform float hue;
            out vec4 fragColor;

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
            " : "
            varying mediump vec2 qt_TexCoord0;
            uniform highp float qt_Opacity;
            uniform highp float hue;

            highp float hueToIntensity(highp float v1, highp float v2, highp float h) {
                h = fract(h);
                if (h < 1.0 / 6.0)
                    return v1 + (v2 - v1) * 6.0 * h;
                else if (h < 1.0 / 2.0)
                    return v2;
                else if (h < 2.0 / 3.0)
                    return v1 + (v2 - v1) * 6.0 * (2.0 / 3.0 - h);

                return v1;
            }

            highp vec3 HSLtoRGB(highp vec3 color) {
                highp float h = color.x;
                highp float l = color.z;
                highp float s = color.y;

                if (s < 1.0 / 256.0)
                    return vec3(l, l, l);

                highp float v1;
                highp float v2;
                if (l < 0.5)
                    v2 = l * (1.0 + s);
                else
                    v2 = (l + s) - (s * l);

                v1 = 2.0 * l - v2;

                highp float d = 1.0 / 3.0;
                highp float r = hueToIntensity(v1, v2, h + d);
                highp float g = hueToIntensity(v1, v2, h);
                highp float b = hueToIntensity(v1, v2, h - d);
                return vec3(r, g, b);
            }

            void main() {
                lowp vec4 c = vec4(1.0);
                c.rgb = HSLtoRGB(vec3(hue, 1.0 - qt_TexCoord0.t, qt_TexCoord0.s));
                gl_FragColor = c * qt_Opacity;
            }
            "
        }
    }
    //! [contentItem]

    handle: Rectangle {
        implicitWidth: 20
        implicitHeight: 20
        radius: 10
        color: "transparent"
        border.color: control.visualFocus ? "#0066ff" : (control.pressed ? "#36383a" : "#454647")
        border.width: 1

        Rectangle {
            x: 1
            y: 1
            width: 18
            height: 18
            radius: 9
            color: "transparent"
            border.color: control.visualFocus ? "#0066ff" : (control.pressed ? "#86888a" : "#959697")
            border.width: 2
        }
    }
}
