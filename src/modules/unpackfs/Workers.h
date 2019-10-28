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

#ifndef UNPACKFS_WORKERS_H
#define UNPACKFS_WORKERS_H

#include <QString>

namespace UnpackFS
{
class Worker
{
public:
    Worker();
    virtual ~Worker();

    ///@brief Check requirements at creation time: does the source exist
    virtual bool checkRequirements() = 0;

    ///@brief Check requirements at run time: does the target satisfy
    virtual bool checkRequirements( const QString& target ) = 0;

    ///@brief Do the work (only if both requirements are met)
    virtual bool run() = 0;
};

class UnsquashWorker : public Worker
{
public:
    UnsquashWorker( const QString& sourceSquashFS, const QString& destination );
    bool checkRequirements() override;
    bool checkRequirements( const QString& target ) override;
    bool run() override;

private:
    const QString m_source;
    const QString m_dest;
};

}  // namespace UnpackFS

#endif
