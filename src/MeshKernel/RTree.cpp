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

#include <MeshKernel/RTree.hpp>

void meshkernel::RTree::NearestNeighborsOnSquaredDistance(Point node, double searchRadiusSquared)
{
    const auto searchRadius = std::sqrt(searchRadiusSquared);

    const Box2D box(Point2D(node.x - searchRadius, node.y - searchRadius), Point2D(node.x + searchRadius, node.y + searchRadius));
    Point2D nodeSought = Point2D(node.x, node.y);

    m_queryCache.reserve(m_queryVectorCapacity);
    m_queryCache.clear();
    m_rtree2D.query(
        bgi::within(box) &&
            bgi::satisfies([&nodeSought, &searchRadiusSquared](value2D const& v) { return bg::comparable_distance(v.first, nodeSought) <= searchRadiusSquared; }),
        std::back_inserter(m_queryCache));

    m_queryIndices.reserve(m_queryCache.size());
    m_queryIndices.clear();
    for (const auto& [first, second] : m_queryCache)
    {
        m_queryIndices.emplace_back(second);
    }
}

void meshkernel::RTree::NearestNeighbors(Point node)
{

    m_queryCache.reserve(m_queryVectorCapacity);
    m_queryCache.clear();
    const Point2D nodeSought = Point2D(node.x, node.y);
    m_rtree2D.query(bgi::nearest(nodeSought, 1), std::back_inserter(m_queryCache));

    if (!m_queryCache.empty())
    {
        m_queryIndices.clear();
        m_queryIndices.emplace_back(m_queryCache[0].second);
    }
}

void meshkernel::RTree::DeleteNode(size_t position)
{
    const auto numberRemoved = m_rtree2D.remove(m_points[position]);
    if (numberRemoved != 1)
    {
        throw std::invalid_argument("DeleteNode: Could not remove node at given position.");
    }
    m_points[position] = {Point2D{doubleMissingValue, doubleMissingValue}, std::numeric_limits<size_t>::max()};
}

void meshkernel::RTree::InsertNode(const Point& node)
{
    m_points.emplace_back(Point2D{node.x, node.y}, m_points.size());
    m_rtree2D.insert(m_points.end() - 1, m_points.end());
}
