// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MergeClustersAction.h"
#include "ClustersAction.h"
#include "ClustersFilterModel.h"
#include "ClusterData.h"
#include "ClustersActionWidget.h"

MergeClustersAction::MergeClustersAction(ClustersActionWidget* clustersActionWidget) :
    TriggerAction(clustersActionWidget, "Merge"),
    _clustersActionWidget(clustersActionWidget)
{
    setToolTip("Merge selected clusters into one");
    setIconByName("object-group");

    // Merge selected clusters when the action is triggered
    connect(this, &TriggerAction::triggered, this, [this]() {

        // Get the selected rows from the selection model
        const auto selectedRows = _clustersActionWidget->getSelectionModel().selectedRows();

        // Determine merge cluster
        auto mergeCluster = static_cast<Cluster*>(_clustersActionWidget->getFilterModel().mapToSource(selectedRows.first()).internalPointer());

        // Alter name of the merge cluster
        mergeCluster->setName(QString("%1*").arg(mergeCluster->getName()));

        // Mark as modified
        _clustersActionWidget->getClustersAction().getClustersModel()._modifiedByUser[selectedRows.first().row()] = true;;

        // All cluster except the first one need to be removed after the merge process
        QStringList clusterIdsToRemove;

        // Move cluster indices of remaining clusters into the merge cluster
        for (const auto& selectedIndex : selectedRows) {

            // Do not move merge cluster indices into itself
            if (selectedIndex == selectedRows.first())
                continue;

            // Get pointer to cluster
            auto cluster = static_cast<Cluster*>(_clustersActionWidget->getFilterModel().mapToSource(selectedIndex).internalPointer());

            // Flag the cluster identifier to be removed
            clusterIdsToRemove << cluster->getId();

            // Move the cluster indices into the merge cluster
            mergeCluster->getIndices().insert(mergeCluster->getIndices().end(), cluster->getIndices().begin(), cluster->getIndices().end());
        }

        // Remove the redundant clusters
        _clustersActionWidget->getClustersAction().removeClustersById(clusterIdsToRemove);
    });

    // Update action read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersActionWidget->getSelectionModel().selectedRows().count() >= 2);
    };

    // Update action read only status when the item selection changes
    connect(&_clustersActionWidget->getSelectionModel(), &QItemSelectionModel::selectionChanged, updateReadOnly);
    connect(&_clustersActionWidget->getFilterModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Initialize read only status
    updateReadOnly();
}
