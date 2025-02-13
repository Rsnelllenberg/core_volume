// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

// Source file created by Niels Dekker
// LKEB, Division of Image Processing
// Copyright Leiden University Medical Center (LUMC)

// Its own header file:
#include "ModelResetter.h"

#include <QtCore/QAbstractItemModel>


namespace
{
    // Ugly but necessary hack to allow calling the protected QAbstractItemModel
    // member functions beginResetModel and endResetModel.
    struct BreakProtection : private QAbstractItemModel
    {
        static void callBeginResetModel(QAbstractItemModel& model)
        {
            (model.*(&BreakProtection::beginResetModel))();
        }

        static void callEndResetModel(QAbstractItemModel& model)
        {
            (model.*(&BreakProtection::endResetModel))();
        }
    };

}  // End of namespace.

namespace mv
{
    // Explicit constructor.
    ModelResetter::ModelResetter(QAbstractItemModel* const arg)
        :
        m_model(arg)
    {
        if (arg != nullptr)
        {
            // Calls the protected member function.
            BreakProtection::callBeginResetModel(*arg);
        }
    }


    void ModelResetter::EndResetModel()
    {
        if (m_model != nullptr)
        {
            auto& model = *m_model;

            // Note: m_model should be set before calling the function, for exception safety.
            m_model = nullptr;

            BreakProtection::callEndResetModel(model);
        }
    }


    ModelResetter::~ModelResetter()
    {
        if (m_model != nullptr)
        {
            try
            {
                BreakProtection::callEndResetModel(*m_model);
            }
            catch (const std::exception&)
            {
                // Out of luck.
            }
        }
    }

} // namespace mv

