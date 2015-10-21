#include <QtTest>
#include "tst_singleton.h"

int main(int argc, char *argv[])
{
    TstSingleton tc;
    return QTest::qExec(&tc, argc, argv);
}
