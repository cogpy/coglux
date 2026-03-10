// cog/lux/expression_node.hpp — Expression Node Graph for MetaHuman DNA
// Typed node graph for AU activation routing and expression composition
// Header-only, C++11, zero external dependencies
// SPDX-License-Identifier: MIT
//
// Models the expression pipeline as a typed node graph where:
// - Nodes represent expression sources (endocrine, cognitive, chaos, aesthetic)
// - Edges carry AU activation values with typed connections
// - Supports graph traversal for expression debugging and visualization
//
#ifndef COG_LUX_EXPRESSION_NODE_HPP
#define COG_LUX_EXPRESSION_NODE_HPP

#include "../core/core.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

namespace cog { namespace lux {

// ─────────────────────────────────────────────────────────────────────────────
// Expression Source Types
// ─────────────────────────────────────────────────────────────────────────────
enum class ExprSourceType : uint8_t {
    ENDOCRINE,    // Hormone-driven AU activation
    COGNITIVE,    // Cognitive mode preset
    CHAOTIC,      // Lorenz attractor micro-expression
    AESTHETIC,    // SuperHotGirl aesthetic bias
    VALENCE,      // Valence-arousal mapping
    COMPOSITE,    // Named composite expression
    BLEND,        // Blended output
};

inline const char* source_type_name(ExprSourceType t) {
    static const char* names[] = {
        "Endocrine", "Cognitive", "Chaotic", "Aesthetic",
        "Valence", "Composite", "Blend"
    };
    return names[static_cast<int>(t)];
}

// ─────────────────────────────────────────────────────────────────────────────
// ExpressionNode — A node in the expression routing graph
// ─────────────────────────────────────────────────────────────────────────────
struct ExpressionNode {
    uint32_t id;
    ExprSourceType type;
    std::string label;
    float weight;  // Contribution weight [0, 1]
    bool active;

    // AU activations produced by this node
    std::unordered_map<uint8_t, float> au_activations;

    ExpressionNode()
        : id(0), type(ExprSourceType::BLEND), weight(1.0f), active(true) {}

    ExpressionNode(uint32_t id_, ExprSourceType t, const std::string& lbl, float w = 1.0f)
        : id(id_), type(t), label(lbl), weight(w), active(true) {}

    void set_au(uint8_t au_idx, float value) {
        au_activations[au_idx] = value;
    }

    float get_au(uint8_t au_idx) const {
        auto it = au_activations.find(au_idx);
        return (it != au_activations.end()) ? it->second : 0.0f;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// ExpressionEdge — Typed connection between expression nodes
// ─────────────────────────────────────────────────────────────────────────────
struct ExpressionEdge {
    uint32_t from;
    uint32_t to;
    float blend_weight;
    std::string channel; // "additive", "multiplicative", "override"

    ExpressionEdge()
        : from(0), to(0), blend_weight(1.0f), channel("additive") {}

    ExpressionEdge(uint32_t f, uint32_t t, float w = 1.0f,
                   const std::string& ch = "additive")
        : from(f), to(t), blend_weight(w), channel(ch) {}
};

// ─────────────────────────────────────────────────────────────────────────────
// ExpressionNodeGraph — Complete expression routing graph
// ─────────────────────────────────────────────────────────────────────────────
class ExpressionNodeGraph {
public:
    ExpressionNodeGraph() : next_id_(1) {}

    // Add a node, return its ID
    uint32_t add_node(ExprSourceType type, const std::string& label,
                      float weight = 1.0f) {
        uint32_t id = next_id_++;
        nodes_[id] = ExpressionNode(id, type, label, weight);
        return id;
    }

    // Add an edge
    void add_edge(uint32_t from, uint32_t to, float weight = 1.0f,
                  const std::string& channel = "additive") {
        edges_.push_back(ExpressionEdge(from, to, weight, channel));
    }

    // Get node
    ExpressionNode* get_node(uint32_t id) {
        auto it = nodes_.find(id);
        return (it != nodes_.end()) ? &it->second : nullptr;
    }

    const ExpressionNode* get_node(uint32_t id) const {
        auto it = nodes_.find(id);
        return (it != nodes_.end()) ? &it->second : nullptr;
    }

    // Build the standard meta-echo-dna expression routing graph
    void build_standard() {
        uint32_t endo = add_node(ExprSourceType::ENDOCRINE, "EndocrineMapper", 1.0f);
        uint32_t cog  = add_node(ExprSourceType::COGNITIVE, "CognitivePreset", 0.8f);
        uint32_t val  = add_node(ExprSourceType::VALENCE, "ValenceArousal", 0.7f);
        uint32_t chaos = add_node(ExprSourceType::CHAOTIC, "LorenzMicro", 0.15f);
        uint32_t aesth = add_node(ExprSourceType::AESTHETIC, "SuperHotGirl", 0.6f);
        uint32_t blend = add_node(ExprSourceType::BLEND, "FinalBlend", 1.0f);

        add_edge(endo, blend, 1.0f, "additive");
        add_edge(cog, blend, 0.8f, "additive");
        add_edge(val, blend, 0.7f, "additive");
        add_edge(chaos, blend, 0.15f, "additive");
        add_edge(aesth, blend, 1.0f, "multiplicative");
    }

    // Evaluate the graph: blend all source nodes into the blend node
    void evaluate() {
        // Find blend nodes
        for (auto& kv : nodes_) {
            if (kv.second.type == ExprSourceType::BLEND) {
                evaluate_blend(kv.first);
            }
        }
    }

    // Export as DOT format
    std::string to_dot() const {
        std::ostringstream ss;
        ss << "digraph ExpressionRouting {\n";
        ss << "  rankdir=LR;\n";
        ss << "  node [shape=box, style=rounded];\n";

        for (const auto& kv : nodes_) {
            const auto& n = kv.second;
            std::string color = "white";
            switch (n.type) {
            case ExprSourceType::ENDOCRINE: color = "#FFE0E0"; break;
            case ExprSourceType::COGNITIVE: color = "#E0E0FF"; break;
            case ExprSourceType::CHAOTIC:   color = "#E0FFE0"; break;
            case ExprSourceType::AESTHETIC:  color = "#FFFFE0"; break;
            case ExprSourceType::BLEND:     color = "#E0E0E0"; break;
            default: break;
            }
            ss << "  n" << n.id << " [label=\"" << n.label
               << "\\n(" << source_type_name(n.type) << ")"
               << "\\nw=" << n.weight
               << "\", fillcolor=\"" << color << "\", style=filled];\n";
        }

        for (const auto& e : edges_) {
            ss << "  n" << e.from << " -> n" << e.to
               << " [label=\"" << e.channel << "\\n" << e.blend_weight << "\"];\n";
        }

        ss << "}\n";
        return ss.str();
    }

    size_t node_count() const { return nodes_.size(); }
    size_t edge_count() const { return edges_.size(); }

private:
    uint32_t next_id_;
    std::unordered_map<uint32_t, ExpressionNode> nodes_;
    std::vector<ExpressionEdge> edges_;

    void evaluate_blend(uint32_t blend_id) {
        auto* blend = get_node(blend_id);
        if (!blend) return;
        blend->au_activations.clear();

        for (const auto& edge : edges_) {
            if (edge.to != blend_id) continue;
            const auto* src = get_node(edge.from);
            if (!src || !src->active) continue;

            for (const auto& au : src->au_activations) {
                float contribution = au.second * edge.blend_weight * src->weight;
                if (edge.channel == "additive") {
                    blend->au_activations[au.first] += contribution;
                } else if (edge.channel == "multiplicative") {
                    if (blend->au_activations.count(au.first)) {
                        blend->au_activations[au.first] *= (1.0f + contribution);
                    }
                }
            }
        }

        // Clamp all AU values to [0, 1]
        for (auto& au : blend->au_activations) {
            au.second = std::max(0.0f, std::min(1.0f, au.second));
        }
    }
};

}} // namespace cog::lux

#endif // COG_LUX_EXPRESSION_NODE_HPP
