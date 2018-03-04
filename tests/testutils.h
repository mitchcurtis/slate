#ifndef TESTUTILS_H
#define TESTUTILS_H

// https://bugreports.qt.io/browse/QTBUG-66320
// Taken from qtestcase.h and modified to return bool and store a failureMessage
// so that they can be used in helper functions (non-test functions).
// We don't have COMPARE, because it would require us to get the failure message
// into a string, the logic for which is hidden in testlib.
#define VERIFY(statement) \
do { \
    if (!static_cast<bool>(statement)) { \
        failureMessage = #statement; \
        return false; \
    } \
} while (false)

#define FAIL(message) \
do { \
    failureMessage = message; \
    return false; \
} while (false)

#define VERIFY2(statement, description) \
do { \
    if (!static_cast<bool>(statement)) { \
        failureMessage = description; \
        return false; \
    } \
} while (false)

#define TRY_LOOP_IMPL(expr, timeoutValue, step) \
    if (!(expr)) { \
        QTest::qWait(0); \
    } \
    int qt_test_i = 0; \
    for (; qt_test_i < timeoutValue && !(expr); qt_test_i += step) { \
        QTest::qWait(step); \
    }

#define TRY_TIMEOUT_DEBUG_IMPL(expr, timeoutValue, step)\
    if (!(expr)) { \
        TRY_LOOP_IMPL((expr), (2 * timeoutValue), step);\
        if (expr) { \
            QString msg = QString::fromUtf8("QTestLib: This test case check (\"%1\") failed because the requested timeout (%2 ms) was too short, %3 ms would have been sufficient this time."); \
            msg = msg.arg(QString::fromUtf8(#expr)).arg(timeoutValue).arg(timeoutValue + qt_test_i); \
            FAIL(qPrintable(msg)); \
        } \
    }

// Ideally we'd use qWaitFor instead of QTRY_LOOP_IMPL, but due
// to a compiler bug on MSVC < 2017 we can't (see QTBUG-59096)
#define TRY_IMPL(expr, timeout)\
    const int qt_test_step = 50; \
    const int qt_test_timeoutValue = timeout; \
    TRY_LOOP_IMPL((expr), qt_test_timeoutValue, qt_test_step); \
    TRY_TIMEOUT_DEBUG_IMPL((expr), qt_test_timeoutValue, qt_test_step)\

// Will try to wait for the expression to become true while allowing event processing
#define TRY_VERIFY_WITH_TIMEOUT(expr, timeout) \
do { \
    TRY_IMPL((expr), timeout);\
    VERIFY(expr); \
} while (false)

#define TRY_VERIFY(expr) TRY_VERIFY_WITH_TIMEOUT((expr), 5000)

// Will try to wait for the expression to become true while allowing event processing
#define TRY_VERIFY2_WITH_TIMEOUT(expr, messageExpression, timeout) \
do { \
    TRY_IMPL((expr), timeout);\
    VERIFY2(expr, messageExpression); \
} while (false)

#define TRY_VERIFY2(expr, messageExpression) TRY_VERIFY2_WITH_TIMEOUT((expr), (messageExpression), 5000)

#endif // TESTUTILS_H
