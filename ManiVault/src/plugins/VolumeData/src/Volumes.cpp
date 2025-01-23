// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Volumes.h"

#include "VolumeData.h"
#include "InfoAction.h"

#include <util/Exception.h>
#include <util/Timer.h>

#include <DataHierarchyItem.h>
#include <Dataset.h>
#include <LinkedData.h>

#include <PointData/PointData.h>

#include <cmath>
#include <exception>
#include <limits>
#include <numeric>
#include <stdexcept>

#include <QDebug>


using namespace mv::util;

Volumes::Volumes(QString dataName, bool mayUnderive /*= false*/, const QString& guid /*= ""*/) :
    DatasetImpl(dataName, mayUnderive, guid),
    _indices(),
    _volumeData(nullptr),
    _infoAction()
{
    _volumeData = getRawData<VolumeData>();

    setLinkedDataFlags(0);
}

void Volumes::init()
{
    DatasetImpl::init();

    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());

    // parent data set must be valid and derived from points
    if (!getDataHierarchyItem().getParent()->getDataset<DatasetImpl>().isValid() ||
        getDataHierarchyItem().getParent()->getDataType() != PointType)
        qCritical() << "Volumes: warning: volume data set must be derived from points.";
}

mv::Dataset<Volumes> Volumes::addVolumeDataset(QString datasetGuiName, const mv::Dataset<Points>& parentDataSet)
{
    mv::Dataset<Volumes> volumes = mv::data().createDataset<Volumes>("Volumes", "volumes", parentDataSet);

    return volumes;
}

//mv::Dataset<Volumes> Volumes::addVolumeDataset(QString datasetGuiName, const mv::Dataset<Points>& spatialDataset, const mv::Dataset<Points>& valueDataset)
//{
//    mv::Dataset<Volumes> volumes = mv::data().createDataset<Volumes>("Volumes", "volumes", parentDataSet);
//
//    return volumes;
//}

Dataset<DatasetImpl> Volumes::createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    return mv::data().createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
}

Dataset<DatasetImpl> Volumes::copy() const
{
    auto volumes = new Volumes(getRawDataName());

    volumes->setText(text());

    return volumes;
}

VolumeData::ReturnFormat Volumes::getReturnFormat() const
{
    return _volumeData->getReturnFormat();
}

void Volumes::setReturnFormat(const VolumeData::ReturnFormat& type)
{
    _volumeData->setReturnFormat(type);
}

Size3D Volumes::getVolumeSize() const
{
    return _volumeData->getVolumeSize();
}

void Volumes::setVolumeSize(const Size3D& volumeSize)
{
    _volumeData->setVolumeSize(volumeSize);
}

std::uint32_t Volumes::getNumberOfComponentsPerVoxel() const
{
    return _volumeData->getNumberOfComponentsPerVoxel();
}

void Volumes::setNumberOfComponentsPerVoxel(const std::uint32_t& numberOfComponentsPerVoxel)
{
    _volumeData->setNumberOfComponentsPerVoxel(numberOfComponentsPerVoxel);
}

QStringList Volumes::getVolumeFilePaths() const
{
    return _volumeData->getVolumeFilePaths();
}

void Volumes::setVolumeFilePaths(const QStringList& volumeFilePaths)
{
    _volumeData->setVolumeFilePaths(volumeFilePaths);
}

std::uint32_t Volumes::getNumberOfVoxels() const
{
    const auto size = getVolumeSize();
    return size.width() * size.height() * size.depth();
}

std::uint32_t Volumes::valuesPerVoxel()
{
    return 4;
}

QIcon Volumes::getIcon(const QColor& color /*= Qt::black*/) const
{
    return mv::Application::getIconFont("FontAwesome").getIcon("cube", color);
}

std::vector<std::uint32_t>& Volumes::getSelectionIndices()
{
    return _indices;
}

void Volumes::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
}

bool Volumes::canSelect() const
{
    return false;
}

bool Volumes::canSelectAll() const
{
    return false;
}

bool Volumes::canSelectNone() const
{
    return false;
}

bool Volumes::canSelectInvert() const
{
    return false;
}

void Volumes::selectAll()
{
}

void Volumes::selectNone()
{
}

void Volumes::selectInvert()
{
}

void Volumes::getScalarData(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    try
    {
        const auto numberOfElementsRequired = getNumberOfVoxels();

        if (static_cast<std::uint32_t>(scalarData.count()) < numberOfElementsRequired)
            throw std::runtime_error("Scalar data vector number of elements is smaller than the number of voxels");

        getScalarDataForVolumeStack(dimensionIndex, scalarData, scalarDataRange);

        // Initialize scalar data range
        scalarDataRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };

        // Compute the actual scalar data range
        for (auto& scalar : scalarData) {
            scalarDataRange.first = std::min(scalar, scalarDataRange.first);
            scalarDataRange.second = std::max(scalar, scalarDataRange.second);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get scalar data for the given dimension index", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get scalar data for the given dimension index");
    }
}

void Volumes::getScalarData(const std::vector<std::uint32_t>& dimensionIndices, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    try
    {
        const auto numberOfVoxels = static_cast<std::int32_t>(getNumberOfVoxels());
        const auto numberOfElementsRequired = dimensionIndices.size() * getNumberOfVoxels();
        const auto numberOfComponentsPerVoxel = static_cast<std::int32_t>(getNumberOfComponentsPerVoxel());

        if (static_cast<std::uint32_t>(scalarData.count()) < numberOfElementsRequired)
            throw std::runtime_error("Scalar data vector number of elements is smaller than (nDimensions * nVoxels)");

        QVector<float> tempScalarData(numberOfElementsRequired);
        QPair<float, float> tempScalarDataRange(std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest());

        std::int32_t componentIndex = 0;

        for (const auto& dimensionIndex : dimensionIndices) {
            getScalarDataForVolumeStack(dimensionIndex, tempScalarData, tempScalarDataRange);

            for (std::int32_t voxelIndex = 0; voxelIndex < numberOfVoxels; voxelIndex++)
                scalarData[static_cast<size_t>(voxelIndex * numberOfComponentsPerVoxel) + componentIndex] = tempScalarData[voxelIndex];

            componentIndex++;
        }

        scalarDataRange = { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };

        for (auto& scalar : scalarData) {
            scalarDataRange.first = std::min(scalar, scalarDataRange.first);
            scalarDataRange.second = std::max(scalar, scalarDataRange.second);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get scalar data for the given dimension indices", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get scalar data for the given dimension indices");
    }
}

void Volumes::getScalarDataForVolumeStack(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange)
{
    auto parent = getParent();

    if (parent->getDataType() == PointType) {
        auto points = Dataset<Points>(parent);

        std::vector<std::uint32_t> globalIndices;

        points->getGlobalIndices(globalIndices);


        points->visitData([this, &points, dimensionIndex, &globalIndices, &scalarData](auto pointData) {
            if (points->isFull()) {
                for (std::uint32_t localPointIndex = 0; localPointIndex < globalIndices.size(); localPointIndex++) {
                    scalarData[globalIndices[localPointIndex]] = pointData[localPointIndex][dimensionIndex];
                }
            }
            else {
                for (std::uint32_t pointIndex = 0; pointIndex < pointData.size(); pointIndex++) {
                    scalarData[globalIndices[pointIndex]] = pointData[pointIndex][dimensionIndex];
                }
            }
        });
    }   
}

mv::Vector3f Volumes::getVoxelCoordinateFromVoxelIndex(const std::int32_t& voxelIndex) const
{
    const auto size = getVolumeSize();
    return mv::Vector3f(voxelIndex % size.width(), static_cast<std::int32_t>(voxelIndex / static_cast<float>(size.width())), static_cast<std::int32_t>(voxelIndex / static_cast<float>(size.width() * size.height())));
}

std::int32_t Volumes::getVoxelIndexFromVoxelCoordinate(const mv::Vector3f& voxelCoordinate) const
{
    const auto size = getVolumeSize();
    return voxelCoordinate.z * size.width() * size.height() + voxelCoordinate.y * size.width() + voxelCoordinate.x;
}

void Volumes::fromVariantMap(const QVariantMap& variantMap)
{
    DatasetImpl::fromVariantMap(variantMap);

    auto volumeData = getRawData<VolumeData>();

    if (variantMap.contains("TypeIndex"))
        getRawData<VolumeData>()->setReturnFormat(static_cast<VolumeData::ReturnFormat>(variantMap["TypeIndex"].toInt()));

    if (variantMap.contains("VolumeSize")) {
        const auto volumeSize = variantMap["VolumeSize"].toMap();

        setVolumeSize(Size3D(volumeSize["Width"].toInt(), volumeSize["Height"].toInt(), volumeSize["Depth"].toInt()));
    }

    if (variantMap.contains("NumberOfComponentsPerVoxel"))
        setNumberOfComponentsPerVoxel(variantMap["NumberOfComponentsPerVoxel"].toInt());

    if (variantMap.contains("VolumeFilePaths"))
        setVolumeFilePaths(variantMap["VolumeFilePaths"].toStringList());

    events().notifyDatasetDataChanged(this);
}

QVariantMap Volumes::toVariantMap() const
{
    auto variantMap = DatasetImpl::toVariantMap();

    variantMap["TypeIndex"] = static_cast<std::int32_t>(getReturnFormat());
    variantMap["TypeName"] = VolumeData::getTypeName(getReturnFormat());
    variantMap["VolumeSize"] = QVariantMap({ { "Width", getVolumeSize().width() }, { "Height", getVolumeSize().height() }, { "Depth", getVolumeSize().depth() } });
    variantMap["NumberOfComponentsPerVoxel"] = getNumberOfComponentsPerVoxel();
    variantMap["VolumeFilePaths"] = getVolumeFilePaths();

    return variantMap;
}

