/*
    Copyright 2020, Mitch Curtis

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

#ifndef QTUTILS_H
#define QTUTILS_H

#include <QDebug>
#include <QList>
#include <QQuickItem>

namespace QtUtils {
template<typename T>
   QString enumToString(T enumValue)
   {
       QString string;
       QDebug debug(&string);
       debug << enumValue;
       return string;
   }

   template <typename T>
   QString toString(const T &object) {
       QString buffer;
       QDebug stream(&buffer);
       stream.nospace() << object;
       return buffer;
   }

    template<typename T>
    inline T divFloor(const T dividend, const T divisor) {
        T quotient = dividend / divisor;
        const T remainder = dividend % divisor;
        if ((remainder != 0) && ((remainder < 0) != (divisor < 0))) --quotient;
        return quotient;
    }
    template<typename T>
    inline T divCeil(const T dividend, const T divisor) {
        return divFloor(dividend + (divisor - 1), divisor);
    }
    template<typename T>
    inline T modFloor(const T dividend, const T divisor) {
        T remainder = dividend % divisor;
        if ((remainder != 0) && ((remainder < 0) != (divisor < 0))) remainder += divisor;
        return remainder;
    }
    template<typename T>
    inline T modCeil(const T dividend, const T divisor) {
        return modFloor(dividend + (divisor - 1), divisor);
    }

    // TODO: use logging category
//#define FIND_CHILD_ITEMS_DEBUG

    template<typename T>
    void doFindChildItems(QQuickItem *item, const QString &objectName, QList<T> &childItemsFound, int nestLevel = 0)
    {
        const auto childItems = item->childItems();

        ++nestLevel;

        for (QQuickItem *child : childItems) {
            auto childAsT = qobject_cast<T>(child);
            if (childAsT && (objectName.isEmpty() || child->objectName() == objectName)) {
                childItemsFound.append(childAsT);
            }

#ifdef FIND_CHILD_ITEMS_DEBUG
            const QString indentation(nestLevel, QLatin1Char('.'));
            qDebug().noquote() << indentation << (found ? 'Y' : 'N') << child << "visible:" << child->isVisible();
#endif

            doFindChildItems(child, objectName, childItemsFound, nestLevel);
        }
    }

    template<typename T>
    QList<T> findChildItems(QQuickItem *item, const QString &objectName = QString())
    {
#ifdef FIND_CHILD_ITEMS_DEBUG
        qDebug().nospace() << "Looking for child items of " << item << " with optional objectName" << objectName << ":";
#endif
        QList<T> childItems;
        doFindChildItems(item, objectName, childItems);
        return childItems;
    }

    // Based on https://stackoverflow.com/a/28413370/904422.
    class ScopeGuard {
    public:
        template<class Callable>
        ScopeGuard(Callable && restoreFunc) : restoreFunc(std::forward<Callable>(restoreFunc)) {}

        ~ScopeGuard() {
            if(restoreFunc)
                restoreFunc();
        }

        ScopeGuard(const ScopeGuard&) = delete;
        void operator=(const ScopeGuard&) = delete;

    private:
        std::function<void()> restoreFunc;
    };

    // This was originally added to find unique guides.
    // QSet changes ordering which makes the comparison fail, so get the unique values manually.
    // Also, most approaches I've seen are unnecessarily complex: https://stackoverflow.com/questions/1041620.
    template<typename T>
    QVector<T> uniqueValues(const QVector<T> &vector)
    {
        QVector<T> unique;
        for (const auto t : vector) {
            if (!unique.contains(t))
                unique.append(t);
        }
        return unique;
    }

    template<typename Flags>
    bool flagsFromString(const QString &string, Flags &flags) {
        bool ok = false;
        Flags theFlags = static_cast<Flags>(QMetaEnum::fromType<Flags>().keysToValue(qPrintable(string), &ok));
        if (!ok)
            return false;
        flags = theFlags;
        return true;
    }

    template<typename Flags>
    bool flagsToString(const QMetaObject &staticMetaObject, const char *enumName, const Flags &flags, QString &string) {
        const QMetaEnum metaEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator(enumName));
        if (!metaEnum.isValid())
            return false;

        const QString enumValues = QString::fromLatin1(metaEnum.valueToKeys(flags));
        if (enumValues.isEmpty())
            return false;

        string = enumValues;
        return true;
    }
}

#endif // QTUTILS_H
