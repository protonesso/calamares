/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2016, Teo Mrnjavac <teo@kde.org>
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

#ifndef PYTHONQTUTILS_H
#define PYTHONQTUTILS_H

#include <PythonQt.h>

#include <QMutex>
#include <QSharedPointer>
#include <QVariant>

namespace Calamares
{
    class PythonQtViewStep;
}

namespace CalamaresUtils
{
/** @brief A handle for a Python module
 *
 * Different from a Calamares module, this is a handle for a Python module,
 * which will generally be impored from main.py in a Calamares module.
 */
class PythonQtModule
{
public:
    /** @brief Create a Python module handle for the given name (e.g. instance key) */
    PythonQtModule( const QString& name );
    ~PythonQtModule();

    bool isNull() const { return m_module.isNull(); }
    QString name() const { return m_moduleName; }
    QString viewClass() const { return m_viewclassName; }

    /** @brief Load the given Python file (e.g. path to main.py) */
    void load( const QString& modulePath );

    //NOTE: when running this, it is assumed that Python is initialized and
    //      PythonQt::self() is valid.
    QVariant lookupAndCall( PythonQtObjectPtr object,
                            const QStringList& candidateNames,
                            const QVariantList& args = QVariantList(),
                            const QVariantMap& kwargs = QVariantMap() );

    // Calls a method on the module itself instead of on an object.
    QVariant lookupAndCall( const QStringList& candidateNames,
                            const QVariantList& args = QVariantList(),
                            const QVariantMap& kwargs = QVariantMap() );

protected:
    friend class Calamares::PythonQtViewStep;

    ::PythonQtObjectPtr m_module;
    QString m_moduleName;
    QString m_viewclassName;
    QMutex m_locker;
};

using PythonQtModulePtr = QSharedPointer< PythonQtModule >;

} //ns

#endif // PYTHONQTUTILS_H
