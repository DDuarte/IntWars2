#pragma once
#include "AiPathReader.h"
#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>

struct TrianglePosition {
    LPD3DXMESH mesh;
    unsigned int faceIndex;
    D3DXVECTOR3 position;
};

struct NavNodeToPortal {
    std::vector<unsigned short> portals;
};

struct NavPortal {
    Vector3f p1;
    Vector3f p2;
    Vector3f mid;
    unsigned short navNodes[2];
    unsigned short adjacency[4]; //indices of outgoing portals
};

struct NavLineSegment {
    NavLineSegment(Vector3f first, Vector3f second, unsigned short navNodeIndex) : first(first), second(second), navNode(navNodeIndex) {}
    Vector3f first;
    Vector3f second;
    unsigned short navNode;
};

class Pathfinder {

    public:
        Pathfinder(AiPathReader);
        ~Pathfinder();
        bool LineOfSight(const Vector3f &from, const Vector3f &to);
        std::list<Vector3f> FindPath(const Vector2f &from, const Vector2f &to);
        Vector3f Clamp(Vector2f pos, unsigned short *nodeIndex = nullptr);
    private:
        typedef Vector2f location; //index of triangle
        typedef float cost;
        typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, cost> > adjacency_graph;
        typedef adjacency_graph::vertex_descriptor face;
        typedef boost::property_map<adjacency_graph, boost::edge_weight_t>::type WeightMap;
        AiPathReader reader;
        std::list<NavLineSegment> polyOutlineSegments;
        std::vector<NavNodeToPortal> navNodeIndexBuffer;
        std::vector<NavPortal> portals;
        adjacency_graph portalGraph;
        void OptimizeLOS(std::list<Vector3f> &waypoints);
        void EvadeObjects(std::list<Vector3f> &waypoints);
};
