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

#ifndef PYTHONQTJOB_H
#define PYTHONQTJOB_H

#include "Job.h"

#include "PythonQtUtils.h"

#include <PythonQt.h>

namespace Calamares
{
class PythonQtViewStep;
class PythonQtJobModule;
}

/** @brief Interface exposed to Python as libcalamares.job
 *
 * This is the PythonQt implementation of PythonJobApi.h,
 * which is visible in Python as libcalamares.job for
 * Boost.Python-style jobs.
 */
class PythonQtJobInterface : public QObject
{
    Q_OBJECT
public:
    PythonQtJobInterface();
    ~PythonQtJobInterface();

    Q_PROPERTY( QString pretty_name READ prettyName );
    Q_PROPERTY( QString working_path READ workingPath );
    Q_PROPERTY( QString module_name READ moduleName );

public slots:
    QString prettyName() const;
    QString workingPath() const;
    QString moduleName() const;
} ;

/** @brief Job for running Python code via PythonQt
 *
 * This Job can handle "new style" jobs, which come
 * from the jobs() method of a PythonQt View, and
 * also Boost.Python style jobs, with a run() method
 * in the Python module.
 */
class PythonQtJob : public Calamares::Job
{
    Q_OBJECT
public:
    virtual ~PythonQtJob() {}

    QString prettyName() const override;
    QString prettyDescription() const override;
    QString prettyStatusMessage() const override;
    Calamares::JobResult exec() override;

private:
    /** @brief Constructor for jobs in PythonQt style, via ViewStep.jobs() */
    explicit PythonQtJob( CalamaresUtils::PythonQtModulePtr module,
                          PythonQtObjectPtr pyJob,
                          QObject* parent = nullptr );
    /** @brief Constructor for jobs in (old) Python style, via run() */
    explicit PythonQtJob( CalamaresUtils::PythonQtModulePtr module,
                          QObject* parent = nullptr );
    friend class Calamares::PythonQtViewStep;  // only these two can call the ctor
    friend class Calamares::PythonQtJobModule;

    Calamares::JobResult execPythonQtStyle();
    Calamares::JobResult execBoostStyle();

    CalamaresUtils::PythonQtModulePtr m_pythonModule;
    PythonQtObjectPtr m_pyJob;
};

#endif // PYTHONQTJOB_H
