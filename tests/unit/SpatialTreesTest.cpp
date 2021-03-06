#include <chrono>
#include <gtest/gtest.h>
#include <random>

#include <MeshKernel/Entities.hpp>
#include <MeshKernel/RTree.hpp>

TEST(RTree, RTreeRemovePoint)
{
    const int n = 4; // x
    const int m = 4; // y

    std::vector<meshkernel::Point> nodes(n * m);
    std::size_t nodeIndex = 0;
    for (auto j = 0; j < m; ++j)
    {
        for (auto i = 0; i < n; ++i)
        {
            nodes[nodeIndex] = {(double)i, (double)j};
            nodeIndex++;
        }
    }

    meshkernel::RTree rtree;
    rtree.BuildTree(nodes);

    rtree.DeleteNode(0);

    ASSERT_EQ(rtree.Size(), 15);
}

TEST(RTree, PerformanceTestBuildAndSearchRTree)
{
    const int n = 10; // x
    const int m = 10; // y
    std::vector<meshkernel::Point> nodes(n * m);
    std::size_t nodeIndex = 0;
    for (auto j = 0; j < m; ++j)
    {
        for (auto i = 0; i < n; ++i)
        {
            nodes[nodeIndex] = {(double)i, (double)j};
            nodeIndex++;
        }
    }

    auto start(std::chrono::steady_clock::now());
    meshkernel::RTree rtree;
    rtree.BuildTree(nodes);
    auto end = std::chrono::steady_clock::now();
    double elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    std::cout << "Elapsed time build " << nodes.size() << " mesh nodes RTree: " << elapsedTime << " s " << std::endl;

    start = std::chrono::steady_clock::now();
    for (auto i = 0; i < nodes.size(); ++i)
    {
        rtree.NearestNeighborsOnSquaredDistance(nodes[i], 1e-8);
        ASSERT_EQ(rtree.GetQueryResultSize(), 1);
    }
    end = std::chrono::steady_clock::now();
    elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    std::cout << "Elapsed time search " << nodes.size() << " nodes in RTree: " << elapsedTime << " s " << std::endl;
}

TEST(RTree, FindNodesInSquare)
{
    const int n = 4; // x
    const int m = 4; // y

    std::vector<meshkernel::Point> nodes(n * m);
    std::size_t nodeIndex = 0;
    for (auto j = 0; j < m; ++j)
    {
        for (auto i = 0; i < n; ++i)
        {
            nodes[nodeIndex] = {(double)i, (double)j};
            nodeIndex++;
        }
    }

    meshkernel::RTree rtree;
    rtree.BuildTree(nodes);

    // large search size, node found
    std::vector<meshkernel::Point> pointToSearch(1, {(n - 1.0) / 2.0, (n - 1.0) / 2.0});
    double squaredDistance = 0.708 * 0.708;
    rtree.NearestNeighborsOnSquaredDistance(pointToSearch[0], squaredDistance);
    ASSERT_EQ(rtree.GetQueryResultSize(), 4);

    // smaller search size, node not found
    squaredDistance = 0.700 * 0.700;
    rtree.NearestNeighborsOnSquaredDistance(pointToSearch[0], squaredDistance);
    ASSERT_EQ(rtree.GetQueryResultSize(), 0);
}
