/**
 * Copyright 2014 Laurent Montel <montel@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "kpluralhandlingspinboxtest.h"
#include "kpluralhandlingspinbox.h"

#include <QTest>

QTEST_MAIN(KPluralHandlingSpinBoxTest)

KPluralHandlingSpinBoxTest::KPluralHandlingSpinBoxTest()
{
}

void KPluralHandlingSpinBoxTest::shouldHaveDefautValue()
{
    KPluralHandlingSpinBox spinbox;
    QCOMPARE(spinbox.suffix(), QString());
}

void KPluralHandlingSpinBoxTest::shouldUseSingularValueWhenUseValueEqualToOne()
{
    KPluralHandlingSpinBox spinbox;
    spinbox.setSuffix(ki18np("singular", "plural"));
    spinbox.setValue(1);
    QCOMPARE(spinbox.suffix(), QLatin1String("singular"));
}

void KPluralHandlingSpinBoxTest::shouldUsePlurialValueWhenUseValueSuperiorToOne()
{
    KPluralHandlingSpinBox spinbox;
    spinbox.setSuffix(ki18np("singular", "plural"));
    spinbox.setValue(2);
    QCOMPARE(spinbox.suffix(), QLatin1String("plural"));
}

void KPluralHandlingSpinBoxTest::shouldUseSingularValueWhenWeChangeValueAndFinishWithValueEqualOne()
{
    KPluralHandlingSpinBox spinbox;
    spinbox.setSuffix(ki18np("singular", "plural"));
    spinbox.setValue(2);
    spinbox.setValue(1);
    QCOMPARE(spinbox.suffix(), QLatin1String("singular"));
    QCOMPARE(spinbox.value(), 1);
}

void KPluralHandlingSpinBoxTest::shouldReturnEmptySuffix()
{
    KPluralHandlingSpinBox spinbox;
    spinbox.setValue(2);
    QCOMPARE(spinbox.suffix(), QString());
}

