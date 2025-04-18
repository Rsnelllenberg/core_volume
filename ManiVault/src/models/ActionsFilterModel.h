// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "SortFilterProxyModel.h"

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"

#include "AbstractActionsModel.h"

#include <QCompleter>

namespace mv
{

/**
 * Actions filter class
 *
 * Proxy model for filtering concrete actions model classes (derived from AbstractActionsModel)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ActionsFilterModel final : public SortFilterProxyModel
{
public:

    /** Construct with parent \p parent object
     * @param parent Pointer to parent object
    */
    ActionsFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

    /**
     * Get action for filter model \p index
     * @param rowIndex Row index to retrieve the action for
     * @return Pointer to action (nullptr if not found)
     */
    gui::WidgetAction* getAction(std::int32_t rowIndex);

private:

    /**
     * Get source variant data for \p index, \p column and data role
     * @param index Source model index
     * @param column Source model column
     * @param role Data role
     * @return Data in variant form
     */
    QVariant getSourceData(const QModelIndex& index, const AbstractActionsModel::Column& column, int role) const;

public: // Action getters

    gui::StringAction& getTypeFilterAction() { return _typeFilterAction; }
    gui::StringAction& getTypeFilterHumanReadableAction() { return _typeFilterHumanReadableAction; }
    gui::OptionsAction& getScopeFilterAction() { return _scopeFilterAction; }
    gui::OptionsAction& getFilterForceHiddenAction() { return _filterForceHiddenAction; }
    gui::OptionsAction& getFilterForceDisabledAction() { return _filterForceDisabledAction; }
    gui::OptionsAction& getFilterMayPublishAction() { return _filterMayPublishAction; }
    gui::OptionsAction& getFilterMayConnectAction() { return _filterMayConnectAction; }
    gui::OptionsAction& getFilterMayDisconnectAction() { return _filterMayDisconnectAction; }
    gui::ToggleAction& getPublicRootOnlyAction() { return _publicRootOnlyAction; }
    gui::TriggerAction& getRemoveFiltersAction() { return _removeFiltersAction; }

private:
    gui::StringAction   _typeFilterAction;                  /** Action for filtering by action type */
    gui::StringAction   _typeFilterHumanReadableAction;     /** Action for filtering by human-readable action type */
    QCompleter          _typeCompleter;                     /** Completer for human-readable action type filter */
    gui::OptionsAction  _scopeFilterAction;                 /** Action for filtering based on action scope */
    gui::OptionsAction  _filterForceHiddenAction;           /** Filter actions based on their force hidden state */
    gui::OptionsAction  _filterForceDisabledAction;         /** Filter actions based on their force disabled state */
    gui::OptionsAction  _filterMayPublishAction;            /** Filter actions based on whether they may publish */
    gui::OptionsAction  _filterMayConnectAction;            /** Filter actions based on whether they may connect to a public action */
    gui::OptionsAction  _filterMayDisconnectAction;         /** Filter actions based on whether they may disconnect from a public action */
    gui::ToggleAction   _publicRootOnlyAction;              /** Filter public root items (used solely by the public actions model) */
    gui::TriggerAction  _removeFiltersAction;               /** Remove filters action */
};

}
