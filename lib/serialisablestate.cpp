/*
    Copyright 2016, Mitch Curtis

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

#include "serialisablestate.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcSerialisableState, "app.serialisableState")

/*
    This exists for the following reasons:

    - It's for saving project-specific data: ApplicationSettings is used for application-wide settings.

    - The following will not affect a QVariant(Map) property:

      uiState.foo = "abc"
      print(uiState.foo) // undefined

      You have to create a temporary and then reassign it:

      var map = {}
      map.foo = "abc"
      uiState = map
      print(uiState.foo) // "abc"

    - There's no one-liner to get a default value from QML if a certain key
      doesn't exist in a QVariantMap, meaning you end up with stuff like this:

      contentY = uiState.foo !== undefined ? uiState.foo : 0

    - Previously we had strongly typed properties in LayeredImageProject (layerListViewContentY),
      but now that we use SplitView, we need properties in the base Project type since it applies
      to all subclasses. This class allows us to support this and future properties, which is
      not really possible with a C++ class that uses strongly typed properties:

      class Project : public QObject {
          Q_PROPERTY(ProjectUiState *uiState ...)
      }

      class LayeredImageProject : public Project {
          // I dunno if this (shadowing a base class property with a different type)
          // even works, but it's not nice either way.
          Q_PROPERTY(LayeredImageProjectUiState *uiState ...)
      }

    - Having lots of UI state properties littering the project API is not nice.

    - Both ImageCanvas and QML elements such as SplitView need to be able to save state;
      if it was just ImageCanvas, we could store the state as a QJsonObject.

*/
SerialisableState::SerialisableState()
{
}

bool SerialisableState::contains(const QString &key) const
{
    return mState.contains(key);
}

QVariant SerialisableState::value(const QString &key, const QVariant &defaultValue) const
{
    return mState.value(key, defaultValue);
}

void SerialisableState::setValue(const QString &key, const QVariant &value)
{
    qCDebug(lcSerialisableState) << "setting serialisable value at key" << key
        << "to" << value;
    // Even though QVariant can store QByteArray, QJsonValue can't.
    // In the future, when QTBUG-71511 is fixed, we will use Qt.btoa() in QML instead,
    // but for now, reading/writing SplitView state will fail.
    // We could intercept the variant in setValue() and convert it to a Base64 string,
    // but it's more difficult when reading (how do we know it's Base64 and not just a string?)
    mState[key] = value;
}

QByteArray SerialisableState::base64ToBinary(const QString &base64String)
{
    return QByteArray::fromBase64(base64String.toLatin1());
}

QString SerialisableState::binaryToBase64(const QByteArray &binaryByteArray)
{
    return QString::fromLatin1(binaryByteArray.toBase64());
}

QVariantMap SerialisableState::map() const
{
    return mState;
}

void SerialisableState::reset(const QVariantMap &map)
{
    mState = map;
}
