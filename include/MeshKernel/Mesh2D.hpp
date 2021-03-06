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

#include <vector>

#include <MeshKernel/Entities.hpp>
#include <MeshKernel/Mesh.hpp>
#include <MeshKernel/RTree.hpp>

/// \namespace meshkernel
/// @brief Contains the logic of the C++ static library
namespace meshkernel
{
    // Forward declarations
    class CurvilinearGrid;
    class Polygons;
    class GeometryList;

    /// @brief A class derived from Mesh, which describes unstructures 2d meshes.
    ///
    /// When communicating with the client only unstructured meshes are used.
    /// Some algorithms generate curvilinear grids, but these are converted to a mesh
    /// instance when communicating with the client.
    class Mesh2D : public Mesh
    {
    public:
        /// Enumerator describing the different options to delete a mesh
        enum DeleteMeshOptions
        {
            AllNodesInside = 0,
            FacesWithIncludedCircumcenters = 1,
            FacesCompletelyIncluded = 2
        };

        /// Enumerator describing the different node types
        enum class NodeTypes
        {
            internalNode,
            onRing,
            cornerNode,
            hangingNode,
            other
        };

        /// @brief Default constructor
        Mesh2D() = default;

        /// @brief Construct a mesh2d starting from the edges and nodes
        /// @param[in] edges The input edges
        /// @param[in] nodes The input nodes
        /// @param[in] projection The projection to use
        /// @param[in] administration Type of administration to perform
        Mesh2D(const std::vector<Edge>& edges, const std::vector<Point>& nodes, Projection projection, AdministrationOptions administration = AdministrationOptions::AdministrateMeshEdgesAndFaces);

        /// @brief Create triangular grid from nodes (triangulatesamplestonetwork)
        /// @param[in] nodes Input nodes
        /// @param[in] polygons Selection polygon
        /// @param[in] projection The projection to use
        Mesh2D(const std::vector<Point>& nodes, const Polygons& polygons, Projection projection);

        /// @brief Add meshes: result is a mesh composed of the additions
        /// firstMesh += secondmesh results in the second mesh being added to the first
        /// @param[in] rhs The mesh to add
        /// @returns The resulting mesh
        Mesh2D& operator+=(Mesh2D const& rhs);

        /// @brief Perform mesh administration
        /// @param administrationOption Type of administration to perform
        void Administrate(AdministrationOptions administrationOption);

        /// @brief Compute face circumcenters
        void ComputeFaceCircumcentersMassCentersAndAreas(bool computeMassCenters = false);

        /// @brief Find faces: constructs the m_facesNodes mapping, face mass centers and areas (findcells)
        void FindFaces();

        /// @brief Offset the x coordinates if m_projection is spherical
        /// @param[in] minx
        /// @param[in] miny
        void OffsetSphericalCoordinates(double minx, double miny);

        /// @brief Merge close mesh nodes inside a polygon (MERGENODESINPOLYGON)
        /// @param[in] polygons Polygon where to perform the merging
        void MergeNodesInPolygon(const Polygons& polygons);

        /// @brief Masks the edges of all faces included in a polygon
        /// @param polygons The selection polygon
        /// @param invertSelection Invert selection
        /// @param includeIntersected Included the edges intersected by the polygon
        void MaskFaceEdgesInPolygon(const Polygons& polygons, bool invertSelection, bool includeIntersected);

        /// @brief From the masked edges compute the masked nodes
        void ComputeNodeMaskFromEdgeMask();

        /// @brief For a face create a closed polygon and fill local mapping caches (get_cellpolygon)
        /// @param[in]  faceIndex              The face index
        /// @param[out] polygonNodesCache      The node cache array filled with the nodes values
        /// @param[out] localNodeIndicesCache  The consecutive node index in polygonNodesCache (0, 1, 2,...)
        /// @param[out] globalEdgeIndicesCache The edge cache array filled with the global edge indices
        void ComputeFaceClosedPolygonWithLocalMappings(size_t faceIndex,
                                                       std::vector<Point>& polygonNodesCache,
                                                       std::vector<size_t>& localNodeIndicesCache,
                                                       std::vector<size_t>& globalEdgeIndicesCache) const;

        /// @brief For a face create a closed polygon
        /// @param[in]     faceIndex         The face index
        /// @param[in,out] polygonNodesCache The cache array to be filled with the nodes values
        void ComputeFaceClosedPolygon(size_t faceIndex, std::vector<Point>& polygonNodesCache) const;

        /// @brief Determine if a face is fully contained in polygon or not, based on m_nodeMask
        /// @param[in] faceIndex The face index
        /// @returns   If the face is fully contained in the polygon or not
        [[nodiscard]] bool IsFullFaceNotInPolygon(size_t faceIndex) const;

        /// @brief Mask all nodes in a polygon
        /// @param[in] polygons The input polygon
        /// @param[in] inside   Inside/outside option
        void MaskNodesInPolygons(const Polygons& polygons, bool inside);

        /// @brief For a closed polygon, compute the circumcenter of a face (getcircumcenter)
        /// @param[in,out] polygon       Cache storing the face nodes
        /// @param[in]     edgesNumFaces For meshes, the number of faces sharing the edges
        /// @returns       The computed circumcenter
        [[nodiscard]] Point ComputeFaceCircumenter(std::vector<Point>& polygon,
                                                   const std::vector<size_t>& edgesNumFaces) const;

        /// @brief Gets the mass centers of obtuse triangles
        /// @returns The center of obtuse triangles
        [[nodiscard]] std::vector<Point> GetObtuseTrianglesCenters();

        /// @brief Gets the edges crossing the small flow edges
        /// @param[in] smallFlowEdgesThreshold The configurable threshold for detecting the small flow edges
        /// @returns The indices of the edges crossing small flow edges
        [[nodiscard]] std::vector<size_t> GetEdgesCrossingSmallFlowEdges(double smallFlowEdgesThreshold);

        /// @brief Gets the flow edges centers from the crossing edges
        /// @param[in] edges The crossing edges indices
        /// @returns The centers of the flow edges
        [[nodiscard]] std::vector<Point> GetFlowEdgesCenters(const std::vector<size_t>& edges) const;

        /// @brief Deletes small flow edges (removesmallflowlinks, part 1)
        ///
        /// An unstructured mesh can be used to calculate water flow. This involves
        /// a pressure gradient between the circumcenters of neighbouring faces.
        /// That procedure is numerically unreliable when the distance between face
        /// circumcenters (flow edges) becomes too small. Let's consider the following figure
        /// \image html coincide_circumcenter.svg  "Coincide circumcenter"
        /// The algorithm works as follow:
        ///
        /// -   Any degenerated triangle (e.g. those having a coinciding node) is
        ///     removed by collapsing the second and third node into the first one.
        ///
        /// -   The edges crossing small flow edges are found. The flow edge length
        ///     is computed from the face circumcenters and compared to an estimated
        ///     cut off distance. The cutoff distance is computed using the face
        ///     areas as follow:
        ///
        ///     \f$\textrm{cutOffDistance} = \textrm{threshold} \cdot 0.5 \cdot (\sqrt{\textrm{Area}_I}+\sqrt{\textrm{Area}_{II}})\f$
        ///
        /// -   All small flow edges are flagged with invalid indices and removed
        ///     from the mesh. Removal occors in the \ref Mesh2D::Administrate method.
        /// @param[in] smallFlowEdgesThreshold The configurable threshold for detecting the small flow edges
        void DeleteSmallFlowEdges(double smallFlowEdgesThreshold);

        /// @brief Deletes small triangles at the boundaries (removesmallflowlinks, part 2)
        ///
        /// This algorithm removes triangles having the following properties:
        /// - The are at mesh boundary.
        ///
        /// - One or more neighboring faces are non-triangles.
        ///
        /// - The ratio of the face area to the average area of neighboring non
        ///   triangles is less than a minimum ratio (defaults to 0.2).
        ///
        /// - The absolute cosine of one internal angle is less than 0.2.
        ///
        /// These triangles having the above properties are merged by collapsing the
        /// face nodes to the node having the minimum absolute cosine (e.g. the node
        /// where the internal angle is closer to 90 degrees).
        /// @param[in] minFractionalAreaTriangles Small triangles at the boundaries will be eliminated.
        /// This threshold is the ration of the face area to the average area of neighboring faces.
        void DeleteSmallTrianglesAtBoundaries(double minFractionalAreaTriangles);

        /// @brief Computes m_nodesNodes, see class members
        void ComputeNodeNeighbours();

        /// @brief Get the orthogonality values, the inner product of edges and segments connecting the face circumcenters
        /// @return The edge orthogonality
        [[nodiscard]] std::vector<double> GetOrthogonality();

        /// @brief Gets the smoothness values, ratios of the face areas
        /// @return The smoothness at the edges
        [[nodiscard]] std::vector<double> GetSmoothness();

        /// @brief Gets the aspect ratios (the ratios edges lengths to flow edges lengths)
        /// @param[in,out] aspectRatios The aspect ratios (passed as reference to avoid re-allocation)
        void ComputeAspectRatios(std::vector<double>& aspectRatios);

        ///  @brief Classifies the nodes (makenetnodescoding)
        void ClassifyNodes();

        /// @brief Deletes coinciding triangles
        void DeleteDegeneratedTriangles();

        /// @brief Transform non-triangular faces in triangular faces
        void TriangulateFaces();

        /// @brief Make a dual face around the node, enlarged by a factor
        /// @param[in] node The node index
        /// @param[in] enlargementFactor The factor by which the dual face is enlarged
        /// @param[out] dualFace The dual face to be calculated
        void MakeDualFace(size_t node, double enlargementFactor, std::vector<Point>& dualFace);

        /// @brief Sorts the faces around a node, sorted in counter clock wise order
        /// @param[in] node The node index
        /// @return The face indexses
        [[nodiscard]] std::vector<size_t> SortedFacesAroundNode(size_t node) const;

        /// @brief Convert all mesh boundaries to a vector of polygon nodes, including holes (copynetboundstopol)
        /// @param[in] polygon The polygon where the operation is performed
        /// @return The resulting polygon mesh boundary
        [[nodiscard]] std::vector<Point> MeshBoundaryToPolygon(const std::vector<Point>& polygon);

        /// @brief Constructs a polygon from the meshboundary, by walking through the mesh
        /// @param[in] polygonNodes The input mesh
        /// @param[in] isVisited the visited mesh nodes
        /// @param[in] currentNode the current node
        /// @param[out] meshBoundaryPolygon The resulting polygon points
        void WalkBoundaryFromNode(const std::vector<Point>& polygonNodes,
                                  std::vector<bool>& isVisited,
                                  size_t& currentNode,
                                  std::vector<Point>& meshBoundaryPolygon) const;

        /// @brief Gets the hanging edges
        /// @return A vector with the indices of the hanging edges
        [[nodiscard]] std::vector<size_t> GetHangingEdges() const;

        /// @brief Deletes the hanging edges
        void DeleteHangingEdges();

        /// @brief For a collection of points compute the face indices including them
        /// @return The face indices including the points
        [[nodiscard]] std::vector<size_t> PointFaceIndices(const std::vector<Point>& points);

        /// @brief Deletes a mesh in a polygon, using several options (delnet)
        /// @param[in] polygons The polygon where to perform the operation
        /// @param[in] deletionOption The deletion option
        /// @param[in] invertDeletion Inverts the selected node to delete (instead of outside the polygon, inside the polygon)
        void DeleteMesh(const Polygons& polygons, int deletionOption, bool invertDeletion);

        /// @brief Inquire if a segment is crossing a face
        /// @param firstPoint The first point of the segment
        /// @param secondPoint The second point of the segment
        /// @return A tuple with the intersectedFace face index and intersected  edge index
        [[nodiscard]] std::tuple<size_t, size_t> IsSegmentCrossingABoundaryEdge(const Point& firstPoint, const Point& secondPoint) const;

        size_t m_maxNumNeighbours = 0; ///< Maximum number of neighbours

        std::vector<Point> m_polygonNodesCache; ///< Cache to store the face nodes

    private:
        /// @brief Find cells recursive, works with an arbitrary number of edges
        /// @param[in] startingNode The starting node
        /// @param[in] node The current node
        /// @param[in] numEdges The number of edges visited so far
        /// @param[in] previousEdge The previously visited edge
        /// @param[in] numClosingEdges The number of edges closing a face (3 for triangles, 4 for quads, etc)
        /// @param[in,out] edges The vector storing the current edges forming a face
        /// @param[in,out] nodes The vector storing the current nodes forming a face
        /// @param[in,out] sortedEdges The caching array used for sorting the edges, used to inquire if an edge has been already visited
        /// @param[in,out] sortedNodes The caching array used for sorting the nodes, used to inquire if a node has been already visited
        /// @param[in,out] nodalValues The nodal values building a closed polygon
        void FindFacesRecursive(size_t startingNode,
                                size_t node,
                                size_t numEdges,
                                size_t previousEdge,
                                size_t numClosingEdges,
                                std::vector<size_t>& edges,
                                std::vector<size_t>& nodes,
                                std::vector<size_t>& sortedEdges,
                                std::vector<size_t>& sortedNodes,
                                std::vector<Point>& nodalValues);

        /// @brief Checks if a triangle has an acute angle (checktriangle)
        /// @param[in] faceNodes
        /// @param[in] nodes
        /// @returns If triangle is okay
        [[nodiscard]] bool CheckTriangle(const std::vector<size_t>& faceNodes, const std::vector<Point>& nodes) const;
    };
} // namespace meshkernel
