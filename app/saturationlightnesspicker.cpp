#include "saturationlightnesspicker.h"

QT_BEGIN_NAMESPACE

SaturationLightnessPicker::SaturationLightnessPicker(QQuickItem *parent) :
    QQuickItem(parent),
    mHue(0),
    mSaturation(0),
    mLightness(0),
    mAlpha(0),
    mPressed(false),
    mBackground(nullptr),
    mContentItem(nullptr),
    mHandle(nullptr)
{
    setActiveFocusOnTab(true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

/*!
    This property controls the hue of the colors that are displayed on the
    picker. It is typically bound to a hue slider:

    The default value is \c 0.0.

    \sa saturation, lightness
*/
qreal SaturationLightnessPicker::hue() const
{
    return mHue;
}

void SaturationLightnessPicker::setHue(qreal hue)
{
    setHue(hue, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::DontEmitColorPicked);
}

/*!
    This property holds the saturation.

    The default value is \c 0.0.

    \sa lightness, hue
*/
qreal SaturationLightnessPicker::saturation() const
{
    return mSaturation;
}

void SaturationLightnessPicker::setSaturation(qreal saturation)
{
    setSaturation(saturation, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::DontEmitColorPicked);
}

/*!
    This property holds the lightness.

    The default value is \c 0.0.

    \sa saturation, hue
*/
qreal SaturationLightnessPicker::lightness() const
{
    return mLightness;
}

void SaturationLightnessPicker::setLightness(qreal lightness)
{
    setLightness(lightness, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::DontEmitColorPicked);
}

/*!
    This property holds the alpha.

    The default value is \c 0.0.

    \sa hue, saturation, lightness
*/
qreal SaturationLightnessPicker::alpha() const
{
    return mAlpha;
}

void SaturationLightnessPicker::setAlpha(qreal alpha)
{
    setAlpha(alpha, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::DontEmitColorPicked);
}

QColor SaturationLightnessPicker::color() const
{
    return QColor::fromHslF(mHue, mSaturation, mLightness, mAlpha);
}

void SaturationLightnessPicker::setColor(const QColor &color)
{
    if (this->color() == color)
        return;

    QColor hsl = color.toHsl();
    auto dontEmitColorChanged = SaturationLightnessPicker::DontEmitColorChanged;
    auto dontEmitColorPicked = SaturationLightnessPicker::DontEmitColorPicked;

    setHue(hsl.hslHueF(), dontEmitColorChanged, dontEmitColorPicked);
    setSaturation(hsl.hslSaturationF(), dontEmitColorChanged, dontEmitColorPicked);
    setLightness(hsl.lightnessF(), dontEmitColorChanged, dontEmitColorPicked);
    setAlpha(color.alphaF(), dontEmitColorChanged, dontEmitColorPicked);

    emit colorChanged();
}

/*!
    This property holds whether the slider is pressed.
*/
bool SaturationLightnessPicker::isPressed() const
{
    return mPressed;
}

void SaturationLightnessPicker::setPressed(bool pressed)
{
    if (pressed == mPressed)
        return;

    mPressed = pressed;
    emit pressedChanged();
}

QQuickItem *SaturationLightnessPicker::background() const
{
    return mBackground;
}

void SaturationLightnessPicker::setBackground(QQuickItem *background)
{
    if (mBackground == background)
        return;

    delete mBackground;
    mBackground = background;
    if (background) {
        background->setParentItem(this);
        if (qFuzzyIsNull(background->z()))
            background->setZ(-1);
        if (isComponentComplete())
            resizeBackground();
    }
    emit backgroundChanged();
}

QQuickItem *SaturationLightnessPicker::contentItem() const
{
    return mContentItem;
}

void SaturationLightnessPicker::setContentItem(QQuickItem *item)
{
    if (mContentItem == item)
        return;

    delete mContentItem;
    mContentItem = item;
    if (item) {
        if (!item->parentItem())
            item->setParentItem(this);
        if (isComponentComplete())
            resizeContent();
    }
    emit contentItemChanged();
}

/*!
    This property holds the handle item.

    \sa {Customizing Slider}
*/
QQuickItem *SaturationLightnessPicker::handle() const
{
    return mHandle;
}

void SaturationLightnessPicker::setHandle(QQuickItem *handle)
{
    if (handle == mHandle)
        return;

    delete mHandle;
    mHandle = handle;

    if (mHandle) {
        if (!mHandle->parentItem())
            mHandle->setParentItem(this);

        setHandleZ();
    }

    emit handleChanged();
}

static const qreal changeAmount = 0.01;

void SaturationLightnessPicker::increaseSaturation()
{
    setSaturation(mSaturation + changeAmount, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::EmitColorPicked);
}

void SaturationLightnessPicker::decreaseSaturation()
{
    setSaturation(mSaturation - changeAmount, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::EmitColorPicked);
}

void SaturationLightnessPicker::increaseLightness()
{
    setLightness(mLightness + changeAmount, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::EmitColorPicked);
}

void SaturationLightnessPicker::decreaseLightness()
{
    setLightness(mLightness - changeAmount, SaturationLightnessPicker::EmitColorChanged,
        SaturationLightnessPicker::EmitColorPicked);
}

void SaturationLightnessPicker::componentComplete()
{
    QQuickItem::componentComplete();
    resizeContent();
    setHandleZ();
}

void SaturationLightnessPicker::keyPressEvent(QKeyEvent *event)
{
    QQuickItem::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_Left:
        setPressed(true);
        decreaseLightness();
        event->accept();
        break;
    case Qt::Key_Right:
        setPressed(true);
        increaseLightness();
        event->accept();
        break;
    case Qt::Key_Up:
        setPressed(true);
        increaseSaturation();
        event->accept();
        break;
    case Qt::Key_Down:
        setPressed(true);
        decreaseSaturation();
        event->accept();
        break;
    }
}

void SaturationLightnessPicker::keyReleaseEvent(QKeyEvent *event)
{
    QQuickItem::keyReleaseEvent(event);
    setPressed(false);
    event->accept();
}

void SaturationLightnessPicker::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    mPressPoint = event->pos();
    setPressed(true);
    event->accept();

    updateValuesForPos(event->pos());
}

void SaturationLightnessPicker::mouseMoveEvent(QMouseEvent *event)
{
    QQuickItem::mouseMoveEvent(event);
    event->accept();

    updateValuesForPos(event->pos());
}

void SaturationLightnessPicker::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);
    mPressPoint = QPoint();
    setPressed(false);
    event->accept();

    updateValuesForPos(event->pos());
}

void SaturationLightnessPicker::mouseUngrabEvent()
{
    QQuickItem::mouseUngrabEvent();
    mPressPoint = QPoint();
    setPressed(false);
}

void SaturationLightnessPicker::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    resizeBackground();
    resizeContent();
}

void SaturationLightnessPicker::updateValuesForPos(const QPoint &pos)
{
    auto dontEmitColorChanged = SaturationLightnessPicker::DontEmitColorChanged;
    auto dontEmitColorPicked = SaturationLightnessPicker::DontEmitColorPicked;

    const bool saturationChanged = setSaturation(saturationAt(pos),
        dontEmitColorChanged, dontEmitColorPicked);
    const bool lightnessChanged = setLightness(lightnessAt(pos),
        dontEmitColorChanged, dontEmitColorPicked);

    if (saturationChanged || lightnessChanged) {
        emit colorChanged();
        emit colorPicked();
    }
}

void SaturationLightnessPicker::resizeBackground()
{
    if (mBackground) {
        if (qFuzzyIsNull(mBackground->x()))
            mBackground->setWidth(width());
        if (qFuzzyIsNull(mBackground->y()))
            mBackground->setHeight(height());
    }
}

void SaturationLightnessPicker::resizeContent()
{
    if (mContentItem) {
        mContentItem->setX(0);
        mContentItem->setY(0);
        mContentItem->setWidth(width());
        mContentItem->setHeight(height());
    }
}

void SaturationLightnessPicker::setHandleZ()
{
    if (mHandle && qFuzzyIsNull(mHandle->z()))
        mHandle->setZ(2);
}

qreal SaturationLightnessPicker::saturationAt(const QPointF &pos) const
{
    if (pos.y() < 0)
        return 1.0;

    if (pos.y() >= height())
        return 0.0;

    return 1.0 - pos.y() / height();
}

qreal SaturationLightnessPicker::lightnessAt(const QPointF &pos) const
{
    if (pos.x() < 0)
        return 0.0;

    if (pos.x() >= width())
        return 1.0;

    return pos.x() / width();
}

bool SaturationLightnessPicker::setHue(qreal hue, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked)
{
    hue = qBound(0.0, hue, 360.0);

    if (qFuzzyCompare(this->mHue, hue))
        return false;

    this->mHue = hue;

    emit hueChanged();
    if (emitColorChanged == EmitColorChanged)
        emit colorChanged();
    if (emitColorPicked == EmitColorPicked)
        emit colorPicked();
    return true;
}

bool SaturationLightnessPicker::setSaturation(qreal saturation, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked)
{
    saturation = qBound(0.0, saturation, 1.0);

    if (qFuzzyCompare(this->mSaturation, saturation))
        return false;

    this->mSaturation = saturation;

    if (mHandle)
        mHandle->setY((1.0 - this->mSaturation) * height() - mHandle->height() / 2);

    emit saturationChanged();
    if (emitColorChanged == EmitColorChanged)
        emit colorChanged();
    if (emitColorPicked == EmitColorPicked)
        emit colorPicked();
    return true;
}

bool SaturationLightnessPicker::setLightness(qreal lightness, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked)
{
    lightness = qBound(0.0, lightness, 1.0);

    if (qFuzzyCompare(this->mLightness, lightness))
        return false;

    this->mLightness = lightness;

    if (mHandle)
        mHandle->setX(this->mLightness * width() - mHandle->width() / 2);

    emit lightnessChanged();
    if (emitColorChanged == EmitColorChanged)
        emit colorChanged();
    if (emitColorPicked == EmitColorPicked)
        emit colorPicked();
    return true;
}

bool SaturationLightnessPicker::setAlpha(qreal alpha, ColorChangedEmission emitColorChanged, ColorPickedEmission emitColorPicked)
{
    alpha = qBound(0.0, alpha, 1.0);
    if (this->mAlpha == alpha)
        return false;

    this->mAlpha = alpha;
    emit alphaChanged();
    if (emitColorChanged == EmitColorChanged)
        emit colorChanged();
    if (emitColorPicked == EmitColorPicked)
        emit colorPicked();
    return true;
}

QT_END_NAMESPACE
