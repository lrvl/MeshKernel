//---- GPL ---------------------------------------------------------------------
//
// Copyright (C)  Stichting Deltares, 2011-2021.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// contact: delft3d.support@deltares.nl
// Stichting Deltares
// P.O. Box 177
// 2600 MH Delft, The Netherlands
//
// All indications and logos of, and references to, "Delft3D" and "Deltares"
// are registered trademarks of Stichting Deltares, and remain the property of
// Stichting Deltares. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once
#include <memory>
#include <string>

#include <MeshKernel/Mesh2D.hpp>
#include <MeshKernelApi/MeshGeometry.hpp>
#include <MeshKernelApi/MeshGeometryDimensions.hpp>

std::tuple<meshkernelapi::MeshGeometry, meshkernelapi::MeshGeometryDimensions> ReadLegacyMeshFromFileForApiTesting(std::string filePath);

std::shared_ptr<meshkernel::Mesh2D> ReadLegacyMeshFromFile(std::string filePath, meshkernel::Projection projection = meshkernel::Projection::cartesian);

std::shared_ptr<meshkernel::Mesh2D> MakeRectangularMeshForTesting(int n, int m, double delta, meshkernel::Projection projection, meshkernel::Point origin = {0.0, 0.0});

std::tuple<meshkernelapi::MeshGeometry, meshkernelapi::MeshGeometryDimensions> MakeRectangularMeshForApiTesting(int n, int m, double delta);

void DeleteRectangularMeshForApiTesting(const meshkernelapi::MeshGeometry& meshgeometry);

std::shared_ptr<meshkernel::Mesh2D> MakeSmallSizeTriangularMeshForTestingAsNcFile();

std::shared_ptr<meshkernel::Mesh2D> MakeCurvilinearGridForTesting();
