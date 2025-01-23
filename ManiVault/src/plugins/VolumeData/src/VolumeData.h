// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Common.h"
#include "Volume.h"

#include <RawData.h>
#include <Set.h>

#include <QDebug>
#include <QString>

#include <vector>

class Points;

const mv::DataType VolumeType = mv::DataType(QString("Volumes"));

class VOLUMEDATA_EXPORT VolumeData : public mv::plugin::RawData
{
public:
    enum ReturnFormat
    {
        Undefined = -1,
        Single, // Every voxel contains all the channels (used for analyzing purposes)
        Atlas   // Every voxel contains only 4 channels (R, G, B, A) and the stack is a bunch of volumes with the same size (used for texture purposes)
    };

    static QString getTypeName(const ReturnFormat& type)
    {
        switch (type)
        {
        case ReturnFormat::Single:
            return "Single";

        case ReturnFormat::Atlas:
            return "Atlas";

        default:
            break;
        }

        return "";
    }

public:
    VolumeData(const mv::plugin::PluginFactory* factory);

    void init() override;

public:

    /** Gets the volume collection type e.g. stack or sequence */
    ReturnFormat getReturnFormat() const;

    /**
     * Sets the volume collection type e.g. stack or sequence
     * @param type Volume collection type
     */
    void setReturnFormat(const ReturnFormat& type);

    /** Get the volume size */
    Size3D getVolumeSize() const;

    /**
     * Set the volume size
     * @param volumeSize Size of the volume(s)
     */
    void setVolumeSize(const Size3D& volumeSize);

    /** Gets the number of components per voxel */
    std::uint32_t getNumberOfComponentsPerVoxel() const;

    /**
     * Sets the number of components per voxel
     * @param numberOfVoxelComponents Number of voxel components
     */
    void setNumberOfComponentsPerVoxel(const std::uint32_t& numberOfComponentsPerVoxel);

    /** Gets the volume file paths */
    QStringList getVolumeFilePaths() const;

    /**
     * Sets the volume file paths
     * @param volumeFilePaths Volume file paths
     */
    void setVolumeFilePaths(const QStringList& volumeFilePaths);

    /**
     * Create dataset for raw data
     * @param guid Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to dataset
     */
    mv::Dataset<mv::DatasetImpl> createDataSet(const QString& guid = "") const override;

private:
    ReturnFormat        _returnFormat;                  /** Type of data that should be returned */
    Size3D              _volumeSize;                    /** Volume size */
    std::uint32_t       _numberOfComponentsPerVoxel;    /** Number of components per voxel */
    QStringList         _volumeFilePaths;               /** Volume file paths */
    QStringList         _dimensionNames;                /** Dimension names */
};

class VolumeDataFactory : public mv::plugin::RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.VolumeData"
                      FILE  "VolumeData.json")

public:
    VolumeDataFactory() {}
    ~VolumeDataFactory() override {}

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    /**
     * Get the read me markdown file URL
     * @return URL of the read me markdown file
     */
    QUrl getReadmeMarkdownUrl() const override;

    /**
     * Get the URL of the GitHub repository
     * @return URL of the GitHub repository
     */
    QUrl getRepositoryUrl() const override;

    mv::plugin::RawData* produce() override;
};
