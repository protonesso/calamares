/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2019, Adriaan de Groot <groot@kde.org>
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils/Logger.h"

#include <QtTest/QtTest>

class UnpackFSTests : public QObject
{
    Q_OBJECT
public:
    UnpackFSTests() {}
    virtual ~UnpackFSTests() {}

private Q_SLOTS:
    void initTestCase();

    void testBogus();
};

void
UnpackFSTests::initTestCase()
{
    Logger::setupLogLevel( Logger::LOGDEBUG );
}

void UnpackFSTests::testBogus()
{
    QVERIFY(true);
}


QTEST_GUILESS_MAIN( UnpackFSTests )

#include "utils/moc-warnings.h"

#include "Tests.moc"
