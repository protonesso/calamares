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

#include "PartitionViewStep.h"

#include <QLabel>

PartitionViewStep::PartitionViewStep( QObject* parent )
    : Calamares::ViewStep( parent )
    , m_widget( nullptr )
{
}

PartitionViewStep::~PartitionViewStep()
{
}


QString
PartitionViewStep::prettyName() const
{
    return tr( "Partitions" );
}


QWidget*
PartitionViewStep::widget()
{
    if ( !m_widget )
        makeWidget();
    return m_widget;
}

bool
PartitionViewStep::isNextEnabled() const
{
    return true;
}


bool
PartitionViewStep::isBackEnabled() const
{
    return true;
}


bool
PartitionViewStep::isAtBeginning() const
{
    return true;
}


bool
PartitionViewStep::isAtEnd() const
{
    return true;
}


void
PartitionViewStep::onActivate()
{
}


void
PartitionViewStep::onLeave()
{
}


void
PartitionViewStep::setConfigurationMap( const QVariantMap& configurationMap )
{
}


Calamares::JobList
PartitionViewStep::jobs() const
{
    return Calamares::JobList();
}

Calamares::RequirementsList
PartitionViewStep::checkRequirements()
{
    return Calamares::RequirementsList();
}

void
PartitionViewStep::makeWidget()
{
    m_widget = new QLabel( tr( "Partitions" ) );
}


CALAMARES_PLUGIN_FACTORY_DEFINITION( PartitionViewStepFactory, registerPlugin<PartitionViewStep>(); )
