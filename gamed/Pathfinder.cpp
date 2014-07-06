#include "stdafx.h"
#include "Pathfinder.h"
#include "ObjectManager.h"
/************************************************************************
Pathfinder, does the following things:
- Read mesh and generate graph on init
- Find the face of start and goal and add them to the graph
- Run A* over all edge midpoints and connect a path
- Run Funnel Algorithm over edges of path
- Run Line of Sight over path to straighten it even more

Possible future Optimizations:
- Fit start and goal into polygon
(find closest edge point, and connect)
- Check dynamic objects for collision and
adjust the waypoints accordingly
- Use BSP Tree for faster ray tracing
- Increase each edge size until intersection
with navigation mesh for perfect funnel
************************************************************************/

#define AGENT_SIZE 10.0f //optional, should be bigger than zero for LOS simplification

/**
*   \brief Fast square root
**/
float inline __declspec(naked) __fastcall fsqrt(float x) {
    _asm {
        fld dword ptr[esp + 4]
        fsqrt
        ret 4
    }
}

float fInvSqrt(float x) {
    float xhalf = 0.5f * x;
    int i = *(int *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

/**
*   \brief CounterClockWise
*
*   Checks if three points are counterclockwise oriented
*
*	Positive is a is counterclockwise to b and c
*	Equal if a, b and c are in one line
*	Negative if a is clockwise to b and c
*
*   \param a - first point to check if counterclockwise to the others
*	\param b - line segment start to test against
*	\param c - line segment end to test against
*   \return An int
*
**/
static inline float ccw(const Vector3f &a, const Vector3f &b, const Vector3f &c) {
    return (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z);
}

static inline bool isLineSegmentsIntersection(const Vector3f &A, const Vector3f &B, const Vector3f &C, const Vector3f &D) {
    return (ccw(A, C, D) < 0) != (ccw(B, C, D) < 0) && (ccw(A, B, C) < 0) != (ccw(A, B, D) < 0);
}

Vector2f LineIntersection(const Vector2f &a1, const Vector2f &b1, const Vector2f &a2, const Vector2f &b2) {
    float x1 = a1.x;
    float y1 = a1.y;
    float x2 = b1.x;
    float y2 = b1.y;
    float x3 = a2.x;
    float y3 = a2.y;
    float x4 = b2.x;
    float y4 = b2.y;
    float r = x1 * y2 - y1 * x2;
    float s = x3 * y4 - y3 * x4;
    float u = x3 - x4;
    float v = x1 - x2;
    float k = y3 - y4;
    float l = y1 - y2;
    Vector2f result(r * u - v * s, r * k - l * s);
    float divisor = v * k - l * u;
    if(divisor != 0) {
        result /= divisor;
    }
    return result;
}

static inline bool isPointInsideTriangle(const Vector3f &p, Vector3f *tri) {
    const bool a = ccw(tri[0], tri[1], p) > 0;
    return (ccw(tri[0], tri[2], p) > 0) != a && (ccw(tri[1], tri[2], p) > 0) == a;
}


bool LineCircleIntersection(Vector2f E, Vector2f L, Vector2f C, float r) {
    auto d = L - E;
    auto f = E - C;
    float a = d.lengthSq();
    float b = 2 * (f.x * d.x + f.y * d.y);
    float c = f.lengthSq() - r * r;
    float dis = b * b - 4 * a * c;
    if(dis < 0) {
        return false;
    }
    dis = fsqrt(dis);
    float t1 = (-b - dis) / (2 * a);
    float t2 = (-b + dis) / (2 * a);
    return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

std::pair<Vector2f, Vector2f> TangentPointsOnCircle(Vector2f Circle, float radius, Vector2f startPoint, unsigned int *numPoints = NULL) {
    std::pair < Vector2f, Vector2f> result;
    Vector2f n = (startPoint - Circle) / radius;
    float d = n.lengthSq();
    if(d == 1) {
        result.first = startPoint;
        result.second = startPoint;
        if(numPoints) {
            *numPoints = 1;
        }
        return result;
    }
    if(d < 1) {
        if(numPoints) {
            *numPoints = 0;
        }
        return result;
    }
    auto D = n.y * fsqrt(d - 1);
    Vector2f tx((n.x - D) / d, (n.x + D) / d);
    if(n.y == 0) {
        D = radius * fsqrt(1 - tx.x * tx.x);
        result.first.y = Circle.y + D;
        result.second.y = Circle.y - D;
    } else {
        result.first.y = Circle.y + radius * (1 - tx.x * n.x) / n.y;
        result.second.y = Circle.y + radius * (1 - tx.y * n.x) / n.y;
    }
    result.first.x = Circle.x + radius * tx.x; //restore scale and position
    result.second.x = Circle.x + radius * tx.y;
    if(numPoints) {
        *numPoints = 2;
    }
    return result;
}

Vector2f ClosestLineCircleIntersection(Vector2f circle, float radius, Vector2f point1, Vector2f point2) {
    float dx, dy, A, B, C, det, t;
    Vector2f result;
    dx = point2.x - point1.x;
    dy = point2.y - point1.y;
    A = dx * dx + dy * dy;
    B = 2 * (dx * (point1.x - circle.x) + dy * (point1.y - circle.y));
    C = (point1.x - circle.x) * (point1.x - circle.x) + (point1.y - circle.y) * (point1.y - circle.y) - radius * radius;
    det = B * B - 4 * A * C;
    if(det == 0) {
        // One solution.
        t = -B / (2 * A);
        result = Vector2f(point1.x + t * dx, point1.y + t * dy);
    } else if(!((A <= 0.0000001) || (det < 0))) {
        // Two solutions.
        t = (float)((-B + fsqrt(det)) / (2 * A));
        auto first = Vector2f(point1.x + t * dx, point1.y + t * dy);
        t = (float)((-B - fsqrt(det)) / (2 * A));
        auto second = Vector2f(point1.y + t * dx, point1.y + t * dy);
        if((first - point1).lengthSq() < (second - point1).lengthSq()) {
            result = first;
        } else {
            result = second;
        }
    }
    return result;
}

float GetY(Vector3f *tri, const Vector2f &point) {
    float det = (tri[1].z - tri[2].z) * (tri[0].x - tri[2].x) + (tri[2].x - tri[1].x) * (tri[0].z - tri[2].z);
    if(det > 0.001) {
        float l1 = ((tri[1].z - tri[2].z) * (point.x - tri[2].x) + (tri[2].x - tri[1].x) * (point.y - tri[2].z)) / det;
        float l2 = ((tri[2].z - tri[0].z) * (point.x - tri[2].x) + (tri[0].x - tri[2].x) * (point.y - tri[2].z)) / det;
        float l3 = 1.0f - l1 - l2;
        return l1 * tri[0].y + l2 * tri[1].y + l3 * tri[2].y;
    } else {
        return (tri[0].y + tri[1].y + tri[2].y) / 3;
    }
}

static inline std::pair<int, int> GetCommonSideIndices(Vector3f *tri1, Vector3f *tri2) {
    int result[2];
    for(int i = 0, c = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(tri1[i] == tri2[j]) {
                result[c++] = i;
                if(c > 1) {
                    return std::pair<int, int>(result[0], result[1]);
                }
            }
        }
    }
    return std::pair<int, int>(-1, -1);
}

template <class Graph, class CostType, class LocMap> class distance_heuristic : public boost::astar_heuristic < Graph, CostType > {
        public:
        typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
        distance_heuristic(LocMap l, Vertex goal) : m_location(l), m_goal(goal) {}
        CostType operator()(Vertex u) {
            auto &p = m_location[u].mid;
            auto &g = m_location[m_goal].mid;
            CostType dx = p.x - g.x;
            CostType dy = p.y - g.y;
            CostType dz = p.z - g.z;
            return fsqrt(dx * dx + dy * dy + dz * dz);
        }
        private:
        LocMap m_location;
        Vertex m_goal;
};

struct found_goal {};
template <class Vertex> class astar_goal_visitor : public boost::default_astar_visitor {
        public:
        astar_goal_visitor(Vertex goal) : m_goal(goal) {}
        template <class Graph>
        void examine_vertex(Vertex u, Graph &g) {
            if(u == m_goal) {
                throw found_goal();
            }
        }
        private:
        Vertex m_goal;
};

Pathfinder::Pathfinder(AiPathReader reader) : reader(reader) {
    portals.reserve(reader.nodes.size() * 3);
    navNodeIndexBuffer.resize(reader.nodes.size()); //says which portals are in triangle
    for(int firstI = 0; firstI < reader.nodes.size(); firstI++) {
        auto first = reader.nodes[firstI];
        int verticeUsageCount[3] = { 0 };
        for(int i = 0; i < 3; i++) {
            auto secondI = first.adjacency[i];
            if(secondI != 0xFFFF) {
                auto second = reader.nodes[secondI];
                NavPortal p;
                p.navNodes[0] = firstI;
                p.navNodes[1] = secondI;
                auto sides = GetCommonSideIndices(first.triangle, second.triangle);
                if(sides.first == -1 || sides.second == -1) {
                    continue;
                }
                verticeUsageCount[sides.first]++;
                verticeUsageCount[sides.second]++;
                if(secondI < firstI) {  //only add each edge once (every edge is in two faces)
                    p.p1 = first.triangle[sides.first];
                    p.p2 = first.triangle[sides.second];
                    p.mid = (p.p1 + p.p2) / 2.0f; //precomputed
                    portals.push_back(p);
                    navNodeIndexBuffer[firstI].portals.push_back(portals.size() - 1);
                    navNodeIndexBuffer[secondI].portals.push_back(portals.size() - 1);
                }
            }
        }
        //Add Outline Segments to list
        for(int triVertI = 0; triVertI < 3; triVertI++) {
            int triVert2I = (triVertI + 1) % 3;
            if(verticeUsageCount[triVertI] < 2 && verticeUsageCount[triVert2I] < 2) {
                auto triVert1 = first.triangle[triVertI], triVert2 = first.triangle[triVert2I];
                if(triVert1.x > triVert2.x) {
                    std::swap(triVert1, triVert2);
                }
                polyOutlineSegments.push_back(NavLineSegment(triVert1, triVert2, firstI)); //todo: sort for x values for faster line segment checks
            }
        }
    }
    //Sort outline segments for fast line sweep
    polyOutlineSegments.sort([](NavLineSegment i, NavLineSegment j) {
        return i.first.x < j.first.x;
    });
    //create index buffer for nodes (tells what portals are in face)
    for(int firstI = 0; firstI < portals.size(); firstI++) {
        auto portal = &portals[firstI];
        int j = 0;
        for(int i = 0; i < 2; i++) {
            for(auto otherI : navNodeIndexBuffer[portal->navNodes[i]].portals) {
                if(otherI != firstI) {
                    portal->adjacency[j++] = otherI;
                }
            }
        }
        for(; j < 4; j++) {
            portal->adjacency[j] = 0xFFFF;
        }
    }
    //create boost adjacency graph with weight
    portalGraph = adjacency_graph(portals.size());
    WeightMap portalWeights = boost::get(boost::edge_weight, portalGraph);
    for(int firstI = 0; firstI < portals.size(); firstI++) {
        auto &first = portals[firstI];
        for(int i = 0; i < 4; i++) {
            auto secondI = first.adjacency[i];
            if(secondI != 0xFFFF) {
                auto &second = portals[secondI];
                cost distance = (first.mid - second.mid).length();
                adjacency_graph::edge_descriptor e;
                bool inserted;
                //
                boost::tie(e, inserted) = add_edge(firstI, secondI, portalGraph);
                portalWeights[e] = distance;
            }
        }
    }
}


Pathfinder::~Pathfinder() {
}

static std::list<Vector3f> Funnel(const std::list<NavPortal *> &nodes) {
    struct Portal {
        typedef std::list<NavPortal *>::const_iterator index_t;
        index_t index;
        Vector3f position;
        Portal(index_t index, Vector3f position) : index(index), position(position) { }
    };
    std::list<Vector3f> result;
    if(nodes.size() >= 1) {
        result.push_back(nodes.front()->mid);
    }
    if(nodes.size() <= 1) {
        return result;
    }
    Portal::index_t i = nodes.cbegin();
    Vector3f apex = (*i)->mid;
    Portal left(i, apex), right(i, apex);
    for(++i; i != nodes.cend(); ++i) {
        //Preparation, get right and left point in current portal
        Vector3f rightPoint = (*i)->p1, leftPoint = (*i)->p2;
        if(ccw(leftPoint, rightPoint, (*std::prev(i))->mid) > 0) {
            std::swap(leftPoint, rightPoint);
        }
        //Agent size (make portal smaller)
        #ifdef AGENT_SIZE
        Vector3f rightToLeft = (leftPoint - rightPoint);
        float len = rightToLeft.length();
        if(len > 2 * AGENT_SIZE) {
            rightToLeft /= len;
            rightPoint += rightToLeft * AGENT_SIZE;
            leftPoint -= rightToLeft * AGENT_SIZE;
        }
        #endif
        //Funnel
        if(ccw(apex, right.position, rightPoint) >= 0) {
            if(apex == right.position || ccw(apex, left.position, rightPoint) < 0) {
                right = Portal(i, rightPoint);
            } else { //choose left
                apex = left.position, right = left;
                result.push_back(left.position);
                i = left.index;
                continue;
            }
        }
        if(ccw(apex, left.position, leftPoint) <= 0) {
            if(apex == left.position || ccw(apex, right.position, leftPoint) > 0) {
                left = Portal(i, leftPoint);
            } else { //choose right
                apex = right.position, left = right;
                result.push_back(right.position);
                i = right.index;
                continue;
            }
        }
    }
    result.push_back((*std::prev(i))->mid);
    return result;
}

static int getFaceIndexOf(const Vector3f &pos, std::vector<NavNode> &faces) {
    for(int i = 0; i < faces.size(); i++) {
        Vector3f *tri = faces[i].triangle;
        if(isPointInsideTriangle(pos, tri)) {
            return i;
        }
    }
    return -1;
}

Vector2f getClosestPointToSegment(NavLineSegment &segment, Vector2f &point) {
    float &cx = point.x;
    float &cy = point.y;
    float &ax = segment.first.x;
    float &ay = segment.first.z;
    float &bx = segment.second.x;
    float &by = segment.second.z;
    float bxMax = bx - ax;
    float byMay = by - ay;
    float rL = ((cx - ax) * (bx - ax) + (cy - ay) * (by - ay)) / (bxMax * bxMax + byMay * byMay);
    float rS = rL < 0 ? 0 : (rL > 1 ? 1 : rL);
    return Vector2f(ax + rS * (bx - ax), ay + rS * (by - ay));
}

Vector3f Pathfinder::Clamp(Vector2f pos, unsigned short *nodeIndex /*= nullptr*/) {
    Vector3f result(pos.x, 0, pos.y);
    int nodeI = getFaceIndexOf(result, reader.nodes);
    if(nodeI == -1) {
        float closestDistSqr = std::numeric_limits<float>::infinity();
        Vector2f closestPos;
        for(auto segment : polyOutlineSegments) {
            Vector2f pointInPolygon = getClosestPointToSegment(segment, pos);
            float distSqr = (pointInPolygon - pos).lengthSq();
            if(distSqr < closestDistSqr) {
                closestDistSqr = distSqr;
                nodeI = segment.navNode;
                closestPos = pointInPolygon;
            }
        }
        auto n = closestPos - pos;
        n.normalize();
        pos = closestPos + n; //move inside a little bit
        result = Vector3f(pos.x, 0, pos.y);
    }
    result.y = GetY(reader.nodes[nodeI].triangle, pos);
    if(nodeIndex) {
        *nodeIndex = nodeI;
    }
    return result;
}

std::list<Vector3f> Pathfinder::FindPath(const Vector2f &from2D, const Vector2f &to2D) {
    std::list<Vector3f> result;
    unsigned short startI = -1;
    unsigned short goalI = -1;
    Vector3f from = Clamp(from2D, &startI);
    if(startI == -1) {
        return result; //todo: move inside, if outside, same for goal
    }
    Vector3f to = Clamp(to2D, &goalI);
    if(goalI == -1) {
        return result;
    }
    if(goalI == startI) {
        result.push_back(from);
        result.push_back(to);
        return result;
    }
    if(LineOfSight(from, to)) {  //we check this here, because it's a pretty common thing, and we save the time for the heavy path finding
        result.push_back(from);
        result.push_back(to);
        EvadeObjects(result);
        return result;
    }
    //Add Start and Goal to graph
    std::vector<std::pair<face, face>> dynamicEdges;
    WeightMap portalWeights = boost::get(boost::edge_weight, portalGraph);
    for(int s = 0; s < 2; s++) {
        Vector3f pos = s == 0 ? from : to;
        int fIndex = s == 0 ? startI : goalI;
        NavPortal portal;
        portal.p1 = pos;
        portal.p2 = pos;
        portal.mid = pos;
        portal.navNodes[0] = fIndex;
        portal.navNodes[1] = fIndex;
        int i = 0;
        for(; i < navNodeIndexBuffer[fIndex].portals.size(); i++) {
            auto connection = navNodeIndexBuffer[fIndex].portals[i];
            portal.adjacency[i] = connection;
            //
            auto second = portals[connection];
            cost distance = (pos - second.mid).length();
            adjacency_graph::edge_descriptor e;
            bool inserted;
            //
            auto edge = s == 0 ? std::pair<face, face>(portals.size(), connection) : std::pair<face, face>(connection, portals.size());
            dynamicEdges.push_back(edge);
            boost::tie(e, inserted) = add_edge(edge.first, edge.second, portalGraph);
            portalWeights[e] = distance;
        }
        for(; i < 4; i++) {
            portal.adjacency[i] = 0xFFFF;
        }
        portals.push_back(portal);
    }
    std::vector<adjacency_graph::vertex_descriptor> faceList(num_vertices(portalGraph));
    //std::vector<cost> distanceMap(num_vertices(portalGraph));
    face goal = portals.size() - 1, start = portals.size() - 2;
    try {
        astar_search(
            portalGraph,
            start,
            distance_heuristic<adjacency_graph, cost, NavPortal *>(portals.data(), goal), //in: heuristic to goal every node
            boost::predecessor_map(&faceList[0])	//out: path
            //.distance_map(&distanceMap[0]) //out: distance to nodes
            .visitor(astar_goal_visitor<face>(goal)));	//in: finishing criteria
    } catch(found_goal fg) {
        std::list<NavPortal *> path;
        for(face v = goal;; v = faceList[v]) {
            path.push_front(&portals[v]);
            if(faceList[v] == v) {
                break;
            }
        }
        result = Funnel(path);
        OptimizeLOS(result);
        EvadeObjects(result);
    }
    for(auto dynFace : dynamicEdges) {
        boost::remove_edge(dynFace.first, dynFace.second, portalGraph);
    }
    portals.pop_back(); //remove end portal
    portals.pop_back(); //remove start portal
    return result;
}

bool Pathfinder::LineOfSight(const Vector3f &from, const Vector3f &to) {
    auto smallestX = std::fmin(from.x, to.x);
    auto biggestX = std::fmax(from.x, to.x);
    for(auto lineSegment : polyOutlineSegments) {
        //Line sweep
        if(lineSegment.second.x < smallestX) {
            continue;
        }
        if(lineSegment.first.x > biggestX) {
            break;
        }
        //Intersection
        if(isLineSegmentsIntersection(from, to, lineSegment.first, lineSegment.second)) {
            return false;
        }
    }
    return true;
}

void Pathfinder::OptimizeLOS(std::list<Vector3f> &waypoints) {
    if(waypoints.size() <= 2) {
        return;
    }
    for(auto i = waypoints.begin(); std::next(i) != waypoints.end(); i++) {
        for(auto j = std::prev(waypoints.end()); std::prev(j) != i; j--) {
            if(LineOfSight(*i, *j)) {
                waypoints.erase(std::next(i), j);
                break;
            }
        }
    }
}

void Pathfinder::EvadeObjects(std::list<Vector3f> &waypoints) {
    for(auto i = waypoints.begin(); std::next(i) != waypoints.end(); i++) {
        auto j = std::next(i);
        auto i2 = Vector2f(i->x, i->z), j2 = Vector2f(j->x, j->z);
        NavLineSegment s(*i, *j, 0);
        for(auto tower : CObjectManager::GetTowers()) {
            if(tower) {
                auto pos = tower->GetPos();
                auto range = 88.4 + 50; // todo: read from inibin
                if(LineCircleIntersection(i2, j2, pos, range)) {
                    unsigned int numPtsI = 0, numPtsJ = 0;
                    auto ptsI = TangentPointsOnCircle(pos, range, i2, &numPtsI);
                    auto ptsJ = TangentPointsOnCircle(pos, range, j2, &numPtsJ);
                    if(numPtsI == 2 && numPtsJ == 2) {
                        auto dist1 = (getClosestPointToSegment(s, ptsI.first) - ptsI.first).lengthSq();
                        auto dist2 = (getClosestPointToSegment(s, ptsI.second) - ptsI.second).lengthSq();
                        if(dist1 > dist2) {
                            std::swap(ptsI.first, ptsI.second);
                        }
                        int sideI = ccw(*i, Vector3f(pos.x, i->y, pos.y), Vector3f(ptsI.first.x, i->y, ptsI.first.y)) > 0;
                        int sideJ = ccw(*i, Vector3f(pos.x, i->y, pos.y), Vector3f(ptsJ.first.x, i->y, ptsJ.first.y)) > 0;
                        if(sideI != sideJ) {
                            std::swap(ptsJ.first, ptsJ.second);
                        }
                        auto intersection = LineIntersection(i2, ptsI.first, j2, ptsJ.first);
                        Vector3f intersection3D(intersection.x, i->y, intersection.y);
                        if(getFaceIndexOf(intersection3D, reader.nodes) != -1) {
                            waypoints.insert(j, intersection3D);
                            i++;
                            continue;
                        } else {
                            intersection = LineIntersection(i2, ptsI.second, j2, ptsJ.second);
                            intersection3D = Vector3f(intersection.x, i->y, intersection.y);
                            if(getFaceIndexOf(intersection3D, reader.nodes) != -1) {
                                waypoints.insert(j, intersection3D);
                                i++;
                                continue;
                            } else {
                                numPtsI = 0;
                                numPtsJ = 0;
                            }
                        }
                    }
                    if(numPtsJ <= 0) {
                        auto newDest = ClosestLineCircleIntersection(pos, range, j2, i2);
                        auto n = (i2 - j2);
                        n.normalize();
                        j->x = newDest.x + n.x;
                        j->z = newDest.y + n.y;
                        continue;
                    }
                }
            }
        }
    }
}


