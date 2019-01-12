/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2014-2016, Teo Mrnjavac <teo@kde.org>
 *   Copyright 2017-2019, Adriaan de Groot <groot@kde.org>
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

#include "PythonJobApi.h"
#include "PythonHelper.h"

#include "utils/Logger.h"
#include "utils/CalamaresUtilsSystem.h"
#include "utils/CalamaresUtils.h"

#include "GlobalStorage.h"
#include "JobQueue.h"
#include "PythonGettext.h"

#include <QDir>

#undef slots
#include <boost/python.hpp>

namespace bp = boost::python;

static int
_handle_check_target_env_call_error( const CalamaresUtils::ProcessResult& ec, const QString& cmd )
{
    if ( !ec.first )
        return ec.first;

    QString raise = QString( "import subprocess\n"
                             "e = subprocess.CalledProcessError(%1,\"%2\")\n" )
                    .arg( ec.first )
                    .arg( cmd );
    if ( !ec.second.isEmpty() )
        raise.append( QStringLiteral("e.output = \"\"\"%1\"\"\"\n").arg( ec.second ) );
    raise.append("raise e");
    bp::exec( raise.toStdString().c_str() );
    bp::throw_error_already_set();
    return ec.first;
}

namespace CalamaresPython
{

int
mount( const std::string& device_path,
       const std::string& mount_point,
       const std::string& filesystem_name,
       const std::string& options )
{
    return CalamaresUtils::System::instance()->
           mount( QString::fromStdString( device_path ),
                  QString::fromStdString( mount_point ),
                  QString::fromStdString( filesystem_name ),
                  QString::fromStdString( options ) );
}


static inline QStringList
_bp_list_to_qstringlist( const bp::list& args )
{
    QStringList list;
    for ( int i = 0; i < bp::len( args ); ++i )
    {
        list.append( QString::fromStdString(
                         bp::extract< std::string >( args[ i ] ) ) );
    }
    return list;
}

static inline CalamaresUtils::ProcessResult
_target_env_command(
    const QStringList& args,
    const std::string& stdin,
    int timeout )
{
    return CalamaresUtils::System::instance()->
        targetEnvCommand( args,
                          QString(),
                          QString::fromStdString( stdin ),
                          timeout );
}

int
target_env_call( const std::string& command,
                 const std::string& stdin,
                 int timeout )
{
    return _target_env_command(
        QStringList{ QString::fromStdString( command ) }, stdin, timeout ).first;
}


int
target_env_call( const bp::list& args,
                 const std::string& stdin,
                 int timeout )
{
    return _target_env_command(
        _bp_list_to_qstringlist( args ), stdin, timeout ).first;
}


int
check_target_env_call( const std::string& command,
                       const std::string& stdin,
                       int timeout )
{
    auto ec = _target_env_command(
        QStringList{ QString::fromStdString( command ) }, stdin, timeout );
    return _handle_check_target_env_call_error( ec, QString::fromStdString( command ) );
}


int
check_target_env_call( const bp::list& args,
                       const std::string& stdin,
                       int timeout )
{
    auto ec = _target_env_command( _bp_list_to_qstringlist( args ), stdin, timeout );
    if ( !ec.first )
        return ec.first;

    QStringList failedCmdList = _bp_list_to_qstringlist( args );
    return _handle_check_target_env_call_error( ec, failedCmdList.join( ' ' ) );
}


std::string
check_target_env_output( const std::string& command,
                         const std::string& stdin,
                         int timeout )
{
    auto ec = _target_env_command(
        QStringList{ QString::fromStdString( command ) }, stdin, timeout );
    _handle_check_target_env_call_error( ec, QString::fromStdString( command ) );
    return ec.second.toStdString();
}


std::string
check_target_env_output( const bp::list& args,
                         const std::string& stdin,
                         int timeout )
{
    QStringList list = _bp_list_to_qstringlist( args );
    auto ec = _target_env_command(
        list, stdin, timeout );
    _handle_check_target_env_call_error( ec, list.join( ' ' ) );
    return ec.second.toStdString();
}

void
debug( const std::string& s )
{
    cDebug() << "[PYTHON JOB]: " << QString::fromStdString( s );
}

void
warning( const std::string& s )
{
    cWarning() << "[PYTHON JOB]: " << QString::fromStdString( s );
}

PythonJobInterface::PythonJobInterface( Calamares::PythonJob* parent )
    : m_parent( parent )
{
    auto moduleDir = QDir( m_parent->m_workingPath );
    moduleName = moduleDir.dirName().toStdString();
    prettyName = m_parent->prettyName().toStdString();
    workingPath = m_parent->m_workingPath.toStdString();
    configuration = CalamaresPython::variantMapToPyDict( m_parent->m_configurationMap );
}


void
PythonJobInterface::setprogress( qreal progress )
{
    if ( progress >= 0 && progress <= 1 )
        m_parent->emitProgress( progress );
}


std::string
obscure( const std::string& string )
{
    return CalamaresUtils::obscure( QString::fromStdString( string ) ).toStdString();
}


bp::list
gettext_languages()
{
    bp::list pyList;
    for ( auto lang : CalamaresPython::Gettext::languages() )
        pyList.append( lang.toStdString() );
    return pyList;
}

bp::object
gettext_path()
{
    QString s( CalamaresPython::Gettext::path() );
    if ( s.isEmpty() )
        return bp::object();  // None
    else
        return bp::object( s.toStdString() );
}


}
