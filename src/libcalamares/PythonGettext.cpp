/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2014-2016, Teo Mrnjavac <teo@kde.org>
 *   Copyright 2017-2018, Adriaan de Groot <groot@kde.org>
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

#include "PythonGettext.h"

#include "utils/Logger.h"
#include "GlobalStorage.h"
#include "JobQueue.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QVariant>

namespace CalamaresPython
{
namespace Gettext
{

QStringList
languages()
{
    QStringList languages;

    // There are two ways that Python jobs can be initialised:
    //  - through JobQueue, in which case that has an instance which holds
    //    a GlobalStorage object, or
    //  - through the Python test-script, which initialises its
    //    own GlobalStoragePythonWrapper, which then holds a
    //    GlobalStorage object for all of Python.
    Calamares::JobQueue* jq = Calamares::JobQueue::instance();
    Calamares::GlobalStorage* gs = jq ? jq->globalStorage() : nullptr;

#ifdef WITH_PYTHONBOOST
    if ( !gs )
        gs = CalamaresPython::GlobalStoragePythonWrapper::globalStorageInstance();
#endif

    if ( !gs )
    {
        cWarning() << "No GlobalStorage is available. gettext_languages is unavailable.";
        return languages;
    }

    QVariant localeConf_ = gs->value( "localeConf" );
    if ( localeConf_.canConvert< QVariantMap >() )
    {
        QVariant lang_ = localeConf_.value< QVariantMap >()[ "LANG" ];
        if ( lang_.canConvert< QString >() )
        {
            QString lang = lang_.value< QString >();
            languages.append( lang );
            if ( lang.indexOf( '.' ) > 0 )
            {
                lang.truncate( lang.indexOf( '.' ) );
                languages.append( lang );
            }
            if ( lang.indexOf( '_' ) > 0 )
            {
                lang.truncate( lang.indexOf( '_' ) );
                languages.append( lang );
            }
        }
    }
    return languages;
}

static void
_add_localedirs( QStringList& pathList, const QString& candidate )
{
    if ( !candidate.isEmpty() && !pathList.contains( candidate ) )
    {
        pathList.prepend( candidate );
        if ( QDir( candidate ).cd( "lang" ) )
            pathList.prepend( candidate + "/lang" );
    }
}

QString
path()
{
    // TODO: distinguish between -d runs and normal runs
    // TODO: can we detect DESTDIR-installs?
    QStringList candidatePaths = QStandardPaths::locateAll( QStandardPaths::GenericDataLocation, "locale", QStandardPaths::LocateDirectory );
    QString extra = QCoreApplication::applicationDirPath();
    _add_localedirs( candidatePaths, extra ); // Often /usr/local/bin
    if ( !extra.isEmpty() )
    {
        QDir d( extra );
        if ( d.cd( "../share/locale" ) ) // Often /usr/local/bin/../share/locale -> /usr/local/share/locale
            _add_localedirs( candidatePaths, d.canonicalPath() );
    }
    _add_localedirs( candidatePaths, QDir().canonicalPath() ); // .

    cDebug() << "Standard paths" << candidatePaths;

    for ( auto lang : languages() )
        for ( auto localedir : candidatePaths )
        {
            QDir ldir( localedir );
            cDebug() << "Checking" << lang << "in" <<ldir.canonicalPath();
            if ( ldir.cd( lang ) )
                return localedir;
        }
    return QString();  // None
}

}  // namespace Gettext
}  // namespace CalamaresPython
