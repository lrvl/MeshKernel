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

namespace meshkernelapi
{
    /// @brief A struct used to describe a list of geometries in a C-compatible manner
    struct GeometryList
    {
        /// @brief To be detailed
        int type;

        /// @brief The value used as separator in xCoordinates, yCoordinates and zCoordinates
        double geometrySeparator;

        /// @brief The value used to separate the inner part of a polygon from its outer part
        double innerOuterSeparator;

        /// @brief The number of coordinate values present
        int numberOfCoordinates;

        /// @brief The x coordinate values
        double* xCoordinates = nullptr;

        /// @brief The y coordinate values
        double* yCoordinates = nullptr;

        /// @brief The z coordinate values
        double* zCoordinates = nullptr;
    };
} // namespace meshkernelapi
