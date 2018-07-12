/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2016, Teo Mrnjavac <teo@kde.org>
 *   Copyright 2018, Adriaan de Groot <groot@kde.org>
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

#include "PythonQtViewStep.h"
#include "utils/Logger.h"
#include "utils/CalamaresUtilsGui.h"
#include "utils/PythonQtUtils.h"
#include "viewpages/PythonQtJob.h"

#ifdef HAVE_PYTHONQT_CONSOLE
#include <gui/PythonQtScriptingConsole.h>
#endif

#include <QBoxLayout>
#include <QWidget>


namespace Calamares
{

PythonQtViewStep::PythonQtViewStep( CalamaresUtils::PythonQtModulePtr module,
                                    QObject* parent )
    : ViewStep( parent )
    , m_widget( new QWidget() )
    , m_pythonModule( module )
{
    Q_ASSERT( PythonQt::self() );

    // Prepare the base widget for the module's pages
    m_widget->setLayout( new QVBoxLayout );
    CalamaresUtils::unmarginLayout( m_widget->layout() );

    m_obj = module->createViewStep( m_widget );
}


QString
PythonQtViewStep::prettyName() const
{
    return m_pythonModule->lookupAndCall( m_obj,
                                          { "prettyName",
                                            "prettyname",
                                            "pretty_name" } ).toString();
}


QWidget*
PythonQtViewStep::widget()
{
    if ( m_widget->layout()->count() > 1 )
        cWarning() << "PythonQtViewStep wrapper widget has more than 1 child. "
                    "This should never happen.";

    return m_widget;
}


void
PythonQtViewStep::next()
{
    m_pythonModule->lookupAndCall( m_obj, { "next" } );
}


void
PythonQtViewStep::back()
{
    m_pythonModule->lookupAndCall( m_obj, { "back" } );
}


bool
PythonQtViewStep::isNextEnabled() const
{
    return m_pythonModule->lookupAndCall( m_obj,
                                          { "isNextEnabled",
                                            "isnextenabled",
                                            "is_next_enabled" } ).toBool();
}


bool
PythonQtViewStep::isBackEnabled() const
{
    return m_pythonModule->lookupAndCall( m_obj,
                                          { "isBackEnabled",
                                            "isbackenabled",
                                            "is_back_enabled" } ).toBool();
}


bool
PythonQtViewStep::isAtBeginning() const
{
    return m_pythonModule->lookupAndCall( m_obj,
                                          { "isAtBeginning",
                                            "isatbeginning",
                                            "is_at_beginning" } ).toBool();
}


bool
PythonQtViewStep::isAtEnd() const
{
    return m_pythonModule->lookupAndCall( m_obj,
                                          { "isAtEnd",
                                            "isatend",
                                            "is_at_end" } ).toBool();
}

void
PythonQtViewStep::onActivate()
{
    m_pythonModule->lookupAndCall( m_obj,
                                          { "onActivate",
                                            "onactivate",
                                            "on_activate" });
}

void
PythonQtViewStep::onLeave()
{
    m_pythonModule->lookupAndCall( m_obj,
                                          { "onLeave",
                                            "onleave",
                                            "on_leave" });
}


JobList
PythonQtViewStep::jobs() const
{
    JobList jobs;

    PythonQtObjectPtr jobsCallable = PythonQt::self()->lookupCallable( m_obj, "jobs" );
    if ( jobsCallable.isNull() )
        return jobs;

    PythonQtObjectPtr response = PythonQt::self()->callAndReturnPyObject( jobsCallable );
    if ( response.isNull() )
        return jobs;

    PythonQtObjectPtr listPopCallable = PythonQt::self()->lookupCallable( response, "pop" );
    if ( listPopCallable.isNull() )
        return jobs;

    forever
    {
        PythonQtObjectPtr aJob = PythonQt::self()->callAndReturnPyObject( listPopCallable, { 0 } );
        if ( aJob.isNull() )
            break;

        jobs.append( Calamares::job_ptr( new PythonQtJob( m_pythonModule, aJob ) ) );
    }

    return jobs;
}


void
PythonQtViewStep::setConfigurationMap( const QVariantMap& configurationMap )
{
    m_obj.addVariable( "configuration", configurationMap );
}


QWidget*
PythonQtViewStep::createScriptingConsole()
{
#ifdef HAVE_PYTHONQT_CONSOLE
    PythonQtScriptingConsole* console = new PythonQtScriptingConsole( nullptr, m_cxt );
    console->setProperty( "classname",
                          m_cxt.getVariable( "_calamares_module_typename" ).toString() );
    return console;
#else
    return nullptr;
#endif
}

}
