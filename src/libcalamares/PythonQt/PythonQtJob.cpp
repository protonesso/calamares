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

#include "PythonQtJob.h"

#include "utils/Logger.h"
#include "PythonQtUtils.h"

namespace Calamares
{

PythonQtJob::PythonQtJob( CalamaresUtils::PythonQtModulePtr module,
                          PythonQtObjectPtr pyJob,
                          QObject* parent )
    : Calamares::Job( parent )
    , m_pythonModule( module )
    , m_pyJob( pyJob )
{
}

PythonQtJob::PythonQtJob( CalamaresUtils::PythonQtModulePtr module,
                          QObject* parent )
    : Calamares::Job( parent )
    , m_pythonModule( module )
    , m_pyJob()
{
}


QString
PythonQtJob::prettyName() const
{
    return m_pythonModule->lookupAndCall( m_pyJob,
                                          { "prettyName",
                                            "prettyname",
                                            "pretty_name" } ).toString();
}


QString
PythonQtJob::prettyDescription() const
{
    return m_pythonModule->lookupAndCall( m_pyJob,
                                          { "prettyDescription",
                                            "prettydescription",
                                            "pretty_description" } ).toString();
}


QString
PythonQtJob::prettyStatusMessage() const
{
    return m_pythonModule->lookupAndCall( m_pyJob,
                                          { "prettyStatusMessage",
                                            "prettystatusmessage",
                                            "pretty_status_message" } ).toString();
}


Calamares::JobResult
PythonQtJob::execPythonQtStyle()
{
    Q_ASSERT( !m_pyJob.isNull() );

    // If m_pyJob is null, this returns null, and it looks like
    // it succeeds -- this should be prevented by exec() or the Q_ASSERT
    QVariant response = m_pythonModule->lookupAndCall( m_pyJob, { "exec" } );
    if ( response.isNull() )
        return Calamares::JobResult::ok();

    QVariantMap map = response.toMap();
    if ( map.isEmpty() || map.value( "ok" ).toBool() )
        return Calamares::JobResult::ok();

    return Calamares::JobResult::error( map.value( "message" ).toString(),
                                        map.value( "details" ).toString() );
}

Calamares::JobResult
PythonQtJob::execBoostStyle()
{
    Q_ASSERT( m_pyJob.isNull() );

    QVariant response = m_pythonModule->lookupAndCall( { "run" } );
    if ( response.isNull() )
        return Calamares::JobResult::ok();

    if ( response.canConvert( QVariant::StringList ) )
    {
        QStringList errors = response.toStringList();
        if ( errors.length() == 2 )
        {
            return Calamares::JobResult::error( errors[0], errors[1] );
        }

        return Calamares::JobResult::error( tr( "Unknown PythonQt error" ), errors.join( '\n' ) );
    }
    else
        return Calamares::JobResult::error( tr( "Bad PythonQt error type" ), response.toString() );
}

Calamares::JobResult
PythonQtJob::exec()
{
    if ( m_pyJob.isNull() )
    {
        PythonQtJobInterface job( m_pythonModule );
        return execBoostStyle();
    }
    else
    {
        PythonQtJobInterface job( this );
        return execPythonQtStyle();
    }
}


PythonQtJobInterface::PythonQtJobInterface()
{
    PythonQtObjectPtr cala = PythonQt::self()->importModule( "libcalamares" );
    PythonQt::self()->addObject( cala, "job", this );
}

PythonQtJobInterface::PythonQtJobInterface( PythonQtJob* job )
    : PythonQtJobInterface()
{
    m_prettyName = job->prettyName();
}

PythonQtJobInterface::PythonQtJobInterface( CalamaresUtils::PythonQtModulePtr module )
    : PythonQtJobInterface()
{
    m_prettyName = module->lookupAndCall( {"pretty_name"} ).toString().trimmed();
    if ( m_prettyName.isEmpty() )
    {
        PythonQtObjectPtr callable = module->lookupCallable( "run" );
        QString doc = PythonQt::self()->getVariable( callable, "__doc__" ).toString().trimmed();
        auto i_newline = doc.indexOf('\n');
        if ( i_newline > 0 )
            doc.truncate( i_newline );
        if ( doc.isEmpty() )
            m_prettyName = tr( "PythonQt job" );
        else
            m_prettyName = doc;
        cDebug() << "No pretty_name found, using run.__doc__=" << m_prettyName;
    }
}

PythonQtJobInterface::~PythonQtJobInterface()
{
    PythonQtObjectPtr cala = PythonQt::self()->importModule( "libcalamares" );
    PythonQt::self()->removeVariable( cala, "job" );
}

}
