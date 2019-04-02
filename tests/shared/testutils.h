#ifndef TESTUTILS_H
#define TESTUTILS_H

#define ENSURE_ACTIVE_FOCUS(object) \
QVERIFY2_THROW(object->property("activeFocus").toBool(), \
    qPrintable(QString::fromLatin1("Expected %1 to have active focus, but %2 has it") \
        .arg(object->objectName()).arg(window->activeFocusItem()->objectName())));

#endif // TESTUTILS_H
