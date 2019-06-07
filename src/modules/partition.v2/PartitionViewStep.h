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

#ifndef PARTITIONVIEWSTEP_H
#define PARTITIONVIEWSTEP_H

#include "PluginDllMacro.h"

#include "utils/PluginFactory.h"
#include "viewpages/ViewStep.h"

class QWidget;

/** @brief Partitioning module.
 *
 * This is version 2 of the module, using only KPMCore 4.
 */
class PLUGINDLLEXPORT PartitionViewStep : public Calamares::ViewStep
{
    Q_OBJECT

public:
    explicit PartitionViewStep( QObject* parent = nullptr );
    virtual ~PartitionViewStep() override;

    QString prettyName() const override;

    QWidget* widget() override;

    bool isNextEnabled() const override;
    bool isBackEnabled() const override;

    bool isAtBeginning() const override;
    bool isAtEnd() const override;

    void onActivate() override;
    void onLeave() override;

    void setConfigurationMap( const QVariantMap& configurationMap ) override;

    Calamares::JobList jobs() const override;

    Calamares::RequirementsList checkRequirements() override;

private:
    void makeWidget();  ///< Create widget on demand

    QWidget* m_widget;
};

CALAMARES_PLUGIN_FACTORY_DECLARATION( PartitionViewStepFactory )

#endif // PARTITIONVIEWSTEP_H
