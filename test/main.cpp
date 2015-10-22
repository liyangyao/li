#include <QtTest>
#include "tst_singleton.h"
#include "tst_wmiquery.h"

int main(int argc, char *argv[])
{
    TstSingleton tc;
    QTest::qExec(&tc, argc, argv);

    TstWmiQuery tq;
    QTest::qExec(&tq, argc, argv);

    return 0;
}
