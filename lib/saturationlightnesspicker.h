#ifndef SATURATIONLIGHTNESSPICKER_P_H
#define SATURATIONLIGHTNESSPICKER_P_H

#include <QColor>
#include <QQuickItem>

#include "slate-global.h"

class SaturationLightnessPickerPrivate;

class SLATE_EXPORT SaturationLightnessPicker : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal hue READ hue WRITE setHue NOTIFY hueChanged FINAL)
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY saturationChanged FINAL)
    Q_PROPERTY(qreal lightness READ lightness WRITE setLightness NOTIFY lightnessChanged FINAL)
    Q_PROPERTY(qreal alpha READ alpha WRITE setAlpha NOTIFY alphaChanged FINAL)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(bool pressed READ isPressed WRITE setPressed NOTIFY pressedChanged FINAL)
    Q_PROPERTY(QQuickItem *background READ background WRITE setBackground NOTIFY backgroundChanged FINAL)
    Q_PROPERTY(QQuickItem *contentItem READ contentItem WRITE setContentItem NOTIFY contentItemChanged FINAL)
    Q_PROPERTY(QQuickItem *handle READ handle WRITE setHandle NOTIFY handleChanged FINAL)
    QML_NAMED_ELEMENT(SaturationLightnessPickerTemplate)

public:
    explicit SaturationLightnessPicker(QQuickItem *parent = nullptr);

    qreal hue() const;
    void setHue(qreal hue);

    qreal saturation() const;
    void setSaturation(qreal saturation);

    qreal lightness() const;
    void setLightness(qreal lightness);

    qreal alpha() const;
    void setAlpha(qreal alpha);

    QColor color() const;
    void setColor(const QColor &color);

    bool isPressed() const;
    void setPressed(bool pressed);

    QQuickItem *background() const;
    void setBackground(QQuickItem *background);

    QQuickItem *contentItem() const;
    void setContentItem(QQuickItem *item);

    QQuickItem *handle() const;
    void setHandle(QQuickItem *handle);

signals:
    void hueChanged();
    void saturationChanged();
    void lightnessChanged();
    void alphaChanged();
    void colorChanged();
    void colorPicked();
    void pressedChanged();
    void backgroundChanged();
    void contentItemChanged();
    void handleChanged();

public slots:
    void increaseSaturation();
    void decreaseSaturation();

    void increaseLightness();
    void decreaseLightness();

protected:
    void componentComplete() override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseUngrabEvent() override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    void updateValuesForPos(const QPoint &pos);

    void resizeBackground();
    void resizeContent();

    void setHandleZ();
    qreal saturationAt(const QPointF &pos) const;
    qreal lightnessAt(const QPointF &pos) const;

    enum ColorChangedEmission {
        DontEmitColorChanged,
        EmitColorChanged
    };

    enum ColorPickedEmission {
        DontEmitColorPicked,
        EmitColorPicked
    };

    bool setHue(qreal mHue, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked);
    bool setSaturation(qreal mSaturation, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked);
    bool setLightness(qreal mLightness, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked);
    bool setAlpha(qreal mAlpha, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked);

    qreal mHue;
    qreal mSaturation;
    qreal mLightness;
    qreal mAlpha;
    bool mPressed;
    QPointF mPressPoint;
    QQuickItem *mBackground;
    QQuickItem *mContentItem;
    QQuickItem *mHandle;
};

QML_DECLARE_TYPE(SaturationLightnessPicker)

#endif // SATURATIONLIGHTNESSPICKER_P_H
