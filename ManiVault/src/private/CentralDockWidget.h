// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DockWidget.h"

/**
 * Central dock widget class
 *
 * Widget class which is placed in the central area of a docking manager.
 *
 * @author Thomas Kroes
 */
class CentralDockWidget : public DockWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param parent Pointer to parent widget
     */
    CentralDockWidget(QWidget* parent = nullptr);

    /**
     * Get string that describes the dock widget type
     * @return Type string
     */
    QString getTypeString() const override;
};
