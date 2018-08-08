/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2016, Teo Mrnjavac <teo@kde.org>
 *   Copyright 2018, Adriaan de Groot <groot@kde.org>
 *   Copyright 2018, Raul Rodrigo Segura <raurodse@gmail.com>
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

#include "PythonQtViewModule.h"

#include "utils/Logger.h"
#include "viewpages/ViewStep.h"
#include "viewpages/PythonQtViewStep.h"
#include "ViewManager.h"
#include "CalamaresConfig.h"
#include "PythonQt/PythonQtGlobalStorageWrapper.h"
#include "PythonQt/PythonQtJob.h"
#include "PythonQt/PythonQtUtilsWrapper.h"
#include "GlobalStorage.h"
#include "JobQueue.h"

#include <PythonQt.h>
#include <PythonQt_QtAll.h>

#include <QDir>
#include <QPointer>


static QPointer< GlobalStorage > s_gs = nullptr;
static QPointer< Utils > s_utils = nullptr;

namespace Calamares
{

Module::Type
PythonQtViewModule::type() const
{
    return View;
}


Module::Interface
PythonQtViewModule::interface() const
{
    return PythonQtInterface;
}


void
PythonQtViewModule::loadSelf()
{
    if ( !m_scriptFileName.isEmpty() )
    {
        loadScript();
        if ( m_pythonModule->viewClass().isEmpty() )
        {
            cDebug() << "No View class defined for" << instanceKey();
            return;
        }

        m_viewStep = new PythonQtViewStep( m_pythonModule );

        m_viewStep->setModuleInstanceKey( instanceKey() );
        m_viewStep->setConfigurationMap( m_configurationMap );
        ViewManager::instance()->addViewStep( m_viewStep );
        m_loaded = true;
        cDebug() << "PythonQtViewModule" << instanceKey() << "loading complete.";
    }
}


JobList
PythonQtViewModule::jobs() const
{
    return m_viewStep->jobs();
}


PythonQtViewModule::PythonQtViewModule()
    : PythonQtModuleBase()
{
}

PythonQtViewModule::~PythonQtViewModule()
{
}



Module::Type
PythonQtJobModule::type() const
{
    return Job;
}


Module::Interface
PythonQtJobModule::interface() const
{
    return PythonInterface;
}

PythonQtJobModule::PythonQtJobModule()
    : PythonQtModuleBase()
{
}

PythonQtJobModule::~PythonQtJobModule()
{
}

JobList
PythonQtJobModule::jobs() const
{
    return JobList() << Calamares::job_ptr( new PythonQtJob( m_pythonModule) );
}

void
PythonQtJobModule::loadSelf()
{
    if ( !m_scriptFileName.isEmpty() )
    {
        loadScript();
        if ( !m_pythonModule->viewClass().isEmpty() )
        {
            cWarning() << "Python Job defines a View class in" << instanceKey();
        }

        m_loaded = true;
        cDebug() << "PythonQtJobModule" << instanceKey() << "loading complete.";
    }
}





PythonQtModuleBase::PythonQtModuleBase()
    : Module()
{
}

PythonQtModuleBase::~PythonQtModuleBase()
{
}

void
PythonQtModuleBase::initFrom( const QVariantMap& moduleDescriptor )
{
    Module::initFrom( moduleDescriptor );
    QDir directory( location() );
    m_workingPath = directory.absolutePath();

    if ( !moduleDescriptor.value( "script" ).toString().isEmpty() )
        m_scriptFileName = moduleDescriptor.value( "script" ).toString();
}

void PythonQtModuleBase::loadScript()
{
    static QMutex mutex;
    QMutexLocker l(&mutex);

    if ( PythonQt::self() == nullptr )
    {
        if ( Py_IsInitialized() )
            PythonQt::init( PythonQt::IgnoreSiteModule |
                            PythonQt::RedirectStdOut |
                            PythonQt::PythonAlreadyInitialized );
        else
            PythonQt::init();

        PythonQt_QtAll::init();
        cDebug() << "Initializing PythonQt bindings."
                    << "This should only happen once.";

        //TODO: register classes here into the PythonQt environment, like this:
        //PythonQt::self()->registerClass( &PythonQtViewStep::staticMetaObject,
        //                                 "calamares" );

        // We only do the following to force PythonQt to create a submodule
        // "calamares" for us to put our static objects in
        PythonQt::self()->registerClass( &::GlobalStorage::staticMetaObject,
                                            "calamares" );

        // Get a PythonQtObjectPtr to the PythonQt.calamares submodule
        PythonQtObjectPtr pqtm = PythonQt::priv()->pythonQtModule();
        PythonQtObjectPtr cala = PythonQt::self()->lookupObject( pqtm, "calamares" );

        // Prepare GlobalStorage object, in module PythonQt.calamares
        if ( !s_gs )
            s_gs = new ::GlobalStorage( Calamares::JobQueue::instance()->globalStorage() );
        cala.addObject( "globalstorage", s_gs );

        // Prepare Utils object, in module PythonQt.calamares
        if ( !s_utils )
            s_utils = new ::Utils( Calamares::JobQueue::instance()->globalStorage() );
        cala.addObject( "utils", s_utils );

        // Basic stdout/stderr handling
        QObject::connect( PythonQt::self(), &PythonQt::pythonStdOut,
            []( const QString& message )
            {
                cDebug() << "PythonQt OUT>" << message;
            }
        );
        QObject::connect( PythonQt::self(), &PythonQt::pythonStdErr,
            []( const QString& message )
            {
                cDebug() << "PythonQt ERR>" << message;
            }
        );

        auto libcala = PythonQt::self()->createModuleFromScript( "libcalamares" );
        if ( libcala.isNull() || PythonQt::self()->hadError() )
            cWarning() << "Populating libcalamares failed.";
        else
            libcala.addObject( "utils", s_utils );
    }

    QDir workingDir( m_workingPath );
    if ( !workingDir.exists() )
    {
        cDebug() << "Invalid working directory"
                    << m_workingPath
                    << "for module"
                    << name();
        return;
    }

    QString fullPath = workingDir.absoluteFilePath( m_scriptFileName );
    QFileInfo scriptFileInfo( fullPath );
    if ( !scriptFileInfo.isReadable() )
    {
        cDebug() << "Invalid main script file path"
                    << fullPath
                    << "for module"
                    << name();
        return;
    }

    m_pythonModule.reset( new CalamaresUtils::PythonQtModule( name() ) );
    if ( m_pythonModule.isNull() || m_pythonModule->isNull() )
    {
        cDebug() << "Cannot load PythonQt context from file"
                    << fullPath
                    << "for module"
                    << name();
        return;
    }

    m_pythonModule->load( fullPath );
}

} // namespace Calamares
