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

#include "Workers.h"

namespace UnpackFS
{
Worker::Worker() {}

Worker::~Worker() {}

UnsquashWorker::UnsquashWorker( const QString& sourceSquashFS, const QString& destination )
    : m_source( sourceSquashFS )
    , m_dest( destination )
{
}

bool
UnsquashWorker::checkRequirements()
{
    // TODO: actually check for existence
    return false;
}

bool
UnsquashWorker::checkRequirements( const QString& target )
{
    // TODO: actually check for existence of target
    // TODO: check for presence of m_dest in target
    return false;
}

bool
UnsquashWorker::run()
{
    // TODO: actually unsquash
    return false;
}


}  // namespace UnpackFS
