// cog/lux/lux.hpp — Cognitive Node Graph Framework
// Typed nodes, edges, traversal, pattern matching, DOT export
// Header-only, C++11, zero external dependencies
// SPDX-License-Identifier: MIT
#ifndef COG_LUX_HPP
#define COG_LUX_HPP

#include "../core/core.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <functional>
#include <sstream>
#include <algorithm>
#include <memory>

namespace cog { namespace lux {

// ─────────────────────────────────────────────────────────────────────────────
// Node & Edge Types
// ─────────────────────────────────────────────────────────────────────────────
enum class NodeType : uint16_t {
    GENERIC     = 0,
    CONCEPT     = 1,
    PREDICATE   = 2,
    ACTION      = 3,
    SENSOR      = 4,
    MOTOR       = 5,
    MEMORY      = 6,
    GOAL        = 7,
    SCHEMA      = 8,
    VARIABLE    = 9,
    CONSTANT    = 10,
    PROCESS     = 11,
    AGENT       = 12,
    RESOURCE    = 13,
    EVENT       = 14,
    STATE       = 15,
    NODE_TYPE_COUNT = 16
};

enum class EdgeType : uint16_t {
    GENERIC     = 0,
    DIRECTED    = 1,
    UNDIRECTED  = 2,
    INHERITANCE = 3,
    SIMILARITY  = 4,
    CAUSAL      = 5,
    TEMPORAL    = 6,
    SPATIAL     = 7,
    PART_OF     = 8,
    HAS_A       = 9,
    DEPENDS_ON  = 10,
    INHIBITS    = 11,
    EXCITES     = 12,
    FLOW        = 13,
    EDGE_TYPE_COUNT = 14
};

inline const char* node_type_name(NodeType t) {
    static const char* names[] = {
        "Generic","Concept","Predicate","Action","Sensor","Motor",
        "Memory","Goal","Schema","Variable","Constant","Process",
        "Agent","Resource","Event","State"
    };
    auto idx = static_cast<uint16_t>(t);
    return (idx < 16) ? names[idx] : "Unknown";
}

inline const char* edge_type_name(EdgeType t) {
    static const char* names[] = {
        "Generic","Directed","Undirected","Inheritance","Similarity",
        "Causal","Temporal","Spatial","PartOf","HasA","DependsOn",
        "Inhibits","Excites","Flow"
    };
    auto idx = static_cast<uint16_t>(t);
    return (idx < 14) ? names[idx] : "Unknown";
}

// ─────────────────────────────────────────────────────────────────────────────
// Property — Key-value property for nodes and edges
// ─────────────────────────────────────────────────────────────────────────────
struct Property {
    enum ValueType { STRING, INT, FLOAT, BOOL };
    ValueType vtype;
    std::string sval;
    int64_t ival;
    double fval;
    bool bval;

    Property() : vtype(STRING), ival(0), fval(0), bval(false) {}
    explicit Property(const std::string& s) : vtype(STRING), sval(s), ival(0), fval(0), bval(false) {}
    explicit Property(int64_t i) : vtype(INT), ival(i), fval(0), bval(false) {}
    explicit Property(double f) : vtype(FLOAT), ival(0), fval(f), bval(false) {}
    explicit Property(bool b) : vtype(BOOL), ival(0), fval(0), bval(b) {}

    std::string to_string() const {
        switch (vtype) {
            case STRING: return sval;
            case INT:    return std::to_string(ival);
            case FLOAT:  { std::ostringstream os; os << fval; return os.str(); }
            case BOOL:   return bval ? "true" : "false";
        }
        return "";
    }
};

using PropertyMap = std::unordered_map<std::string, Property>;

// ─────────────────────────────────────────────────────────────────────────────
// CogNode — Typed node with properties
// ─────────────────────────────────────────────────────────────────────────────
struct CogNode {
    uint32_t    id;
    NodeType    type;
    std::string name;
    TruthValue  tv;
    AttentionValue av;
    PropertyMap props;

    CogNode() : id(0), type(NodeType::GENERIC) {}
    CogNode(uint32_t id, NodeType type, const std::string& name)
        : id(id), type(type), name(name) {}

    void set(const std::string& key, const Property& val) { props[key] = val; }

    const Property* get(const std::string& key) const {
        auto it = props.find(key);
        return (it != props.end()) ? &it->second : nullptr;
    }

    std::string label() const {
        return std::string(node_type_name(type)) + ":" + name;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// CogEdge — Typed edge with properties
// ─────────────────────────────────────────────────────────────────────────────
struct CogEdge {
    uint32_t    id;
    uint32_t    source;
    uint32_t    target;
    EdgeType    type;
    float       weight;
    PropertyMap props;

    CogEdge() : id(0), source(0), target(0), type(EdgeType::GENERIC), weight(1.0f) {}
    CogEdge(uint32_t id, uint32_t src, uint32_t tgt, EdgeType type, float w = 1.0f)
        : id(id), source(src), target(tgt), type(type), weight(w) {}
};

// ─────────────────────────────────────────────────────────────────────────────
// CogGraph — Adjacency-list graph with typed nodes and edges
// ─────────────────────────────────────────────────────────────────────────────
class CogGraph {
public:
    CogGraph() : next_node_id_(1), next_edge_id_(1) {}

    // Add node
    uint32_t add_node(NodeType type, const std::string& name,
                      const TruthValue& tv = TruthValue(1.0f, 0.0f)) {
        uint32_t id = next_node_id_++;
        CogNode node(id, type, name);
        node.tv = tv;
        nodes_[id] = node;
        adjacency_[id] = {};
        return id;
    }

    // Add edge
    uint32_t add_edge(uint32_t src, uint32_t tgt, EdgeType type, float weight = 1.0f) {
        if (nodes_.find(src) == nodes_.end() || nodes_.find(tgt) == nodes_.end())
            return 0;
        uint32_t id = next_edge_id_++;
        CogEdge edge(id, src, tgt, type, weight);
        edges_[id] = edge;
        adjacency_[src].push_back(id);
        if (type == EdgeType::UNDIRECTED) {
            adjacency_[tgt].push_back(id);
        }
        return id;
    }

    // Get node
    CogNode* node(uint32_t id) {
        auto it = nodes_.find(id);
        return (it != nodes_.end()) ? &it->second : nullptr;
    }
    const CogNode* node(uint32_t id) const {
        auto it = nodes_.find(id);
        return (it != nodes_.end()) ? &it->second : nullptr;
    }

    // Get edge
    CogEdge* edge(uint32_t id) {
        auto it = edges_.find(id);
        return (it != edges_.end()) ? &it->second : nullptr;
    }

    // Get neighbors of a node
    std::vector<uint32_t> neighbors(uint32_t node_id) const {
        std::vector<uint32_t> result;
        auto it = adjacency_.find(node_id);
        if (it == adjacency_.end()) return result;
        for (auto eid : it->second) {
            auto eit = edges_.find(eid);
            if (eit == edges_.end()) continue;
            if (eit->second.source == node_id) result.push_back(eit->second.target);
            else result.push_back(eit->second.source);
        }
        return result;
    }

    // Get edges from a node
    std::vector<uint32_t> edges_from(uint32_t node_id) const {
        auto it = adjacency_.find(node_id);
        if (it != adjacency_.end()) return it->second;
        return {};
    }

    // BFS traversal
    void bfs(uint32_t start, std::function<bool(const CogNode&, int depth)> visitor) const {
        std::unordered_set<uint32_t> visited;
        std::queue<std::pair<uint32_t, int>> q;
        q.push({start, 0});
        visited.insert(start);
        while (!q.empty()) {
            auto front = q.front(); q.pop();
            auto nit = nodes_.find(front.first);
            if (nit == nodes_.end()) continue;
            if (!visitor(nit->second, front.second)) return;
            for (auto nb : neighbors(front.first)) {
                if (visited.find(nb) == visited.end()) {
                    visited.insert(nb);
                    q.push({nb, front.second + 1});
                }
            }
        }
    }

    // DFS traversal
    void dfs(uint32_t start, std::function<bool(const CogNode&, int depth)> visitor) const {
        std::unordered_set<uint32_t> visited;
        std::stack<std::pair<uint32_t, int>> s;
        s.push({start, 0});
        while (!s.empty()) {
            auto top = s.top(); s.pop();
            if (visited.find(top.first) != visited.end()) continue;
            visited.insert(top.first);
            auto nit = nodes_.find(top.first);
            if (nit == nodes_.end()) continue;
            if (!visitor(nit->second, top.second)) return;
            auto nbs = neighbors(top.first);
            for (auto it = nbs.rbegin(); it != nbs.rend(); ++it) {
                if (visited.find(*it) == visited.end()) {
                    s.push({*it, top.second + 1});
                }
            }
        }
    }

    // Pattern match: find all nodes matching type and property constraints
    std::vector<uint32_t> match_nodes(
        NodeType type,
        const std::function<bool(const CogNode&)>& predicate = nullptr) const
    {
        std::vector<uint32_t> results;
        for (auto& kv : nodes_) {
            if (kv.second.type == type) {
                if (!predicate || predicate(kv.second)) {
                    results.push_back(kv.first);
                }
            }
        }
        return results;
    }

    // Find shortest path (BFS-based, unweighted)
    std::vector<uint32_t> shortest_path(uint32_t from, uint32_t to) const {
        std::unordered_map<uint32_t, uint32_t> parent;
        std::queue<uint32_t> q;
        q.push(from);
        parent[from] = from;
        while (!q.empty()) {
            uint32_t cur = q.front(); q.pop();
            if (cur == to) {
                // Reconstruct path
                std::vector<uint32_t> path;
                uint32_t c = to;
                while (c != from) {
                    path.push_back(c);
                    c = parent[c];
                }
                path.push_back(from);
                std::reverse(path.begin(), path.end());
                return path;
            }
            for (auto nb : neighbors(cur)) {
                if (parent.find(nb) == parent.end()) {
                    parent[nb] = cur;
                    q.push(nb);
                }
            }
        }
        return {}; // No path
    }

    // Remove node and all incident edges
    bool remove_node(uint32_t id) {
        auto it = nodes_.find(id);
        if (it == nodes_.end()) return false;
        // Remove incident edges
        auto eit = adjacency_.find(id);
        if (eit != adjacency_.end()) {
            for (auto edge_id : eit->second) {
                edges_.erase(edge_id);
            }
            adjacency_.erase(eit);
        }
        // Remove from other adjacency lists
        for (auto& kv : adjacency_) {
            auto& vec = kv.second;
            vec.erase(std::remove_if(vec.begin(), vec.end(), [&](uint32_t eid) {
                auto e = edges_.find(eid);
                return e != edges_.end() &&
                       (e->second.source == id || e->second.target == id);
            }), vec.end());
        }
        nodes_.erase(it);
        return true;
    }

    // Export to DOT format
    std::string to_dot(const std::string& graph_name = "CogGraph") const {
        std::ostringstream os;
        os << "digraph " << graph_name << " {\n";
        os << "  rankdir=LR;\n";
        os << "  node [shape=box, style=filled, fillcolor=\"#2d2d2d\", "
              "fontcolor=\"#e0e0e0\", color=\"#555555\"];\n";
        os << "  edge [color=\"#888888\", fontcolor=\"#aaaaaa\"];\n";

        for (auto& kv : nodes_) {
            auto& n = kv.second;
            os << "  n" << n.id << " [label=\"" << n.label()
               << "\\n" << n.tv.to_string() << "\"];\n";
        }

        for (auto& kv : edges_) {
            auto& e = kv.second;
            os << "  n" << e.source << " -> n" << e.target
               << " [label=\"" << edge_type_name(e.type)
               << " w=" << e.weight << "\"];\n";
        }

        os << "}\n";
        return os.str();
    }

    size_t node_count() const { return nodes_.size(); }
    size_t edge_count() const { return edges_.size(); }

    template<typename Fn>
    void foreach_node(Fn fn) const { for (auto& kv : nodes_) fn(kv.second); }

    template<typename Fn>
    void foreach_edge(Fn fn) const { for (auto& kv : edges_) fn(kv.second); }

private:
    uint32_t next_node_id_;
    uint32_t next_edge_id_;
    std::unordered_map<uint32_t, CogNode> nodes_;
    std::unordered_map<uint32_t, CogEdge> edges_;
    std::unordered_map<uint32_t, std::vector<uint32_t>> adjacency_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Version & Init
// ─────────────────────────────────────────────────────────────────────────────
inline const char* version() { return "0.1.0"; }

}} // namespace cog::lux

#endif // COG_LUX_HPP
