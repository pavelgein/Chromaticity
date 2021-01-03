#pragma once

#include "local_types.h"

#include <cstddef>
#include <ostream>
#include <functional>

namespace NMultipartiteGraphs {
    struct TVertex {
        size_t ComponentId;
        INT VertexId;

        TVertex(size_t ComponentId, INT VertexId);

        TVertex();

        bool operator==(const TVertex& other) const;
        bool operator!=(const TVertex& other) const;
    };

    struct TEdge {
        TVertex First;
        TVertex Second;

        TEdge();

        TEdge(TVertex first, TVertex second);

        bool operator==(const TEdge& other) const;

        bool operator!=(const TEdge& other) const {
            return !(*this == other);
        }
    };


    class IGraph {
    public:
        virtual INT VerticesCount() const = 0;

        virtual INT I2Invariant() const = 0;

        virtual INT I3Invariant() const = 0;

        virtual INT I4Invariant() const = 0;

        virtual INT PtInvariant() const = 0;

        virtual ~IGraph() = default;
    };

    template<typename TNumber>
    using TInvariant = std::function<TNumber(const IGraph& graph)>;
}

std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TEdge& edge);
std::ostream& operator<<(std::ostream& os, const NMultipartiteGraphs::TVertex& node);

namespace std {
template<>
struct hash<NMultipartiteGraphs::TVertex> {
    size_t operator()(const NMultipartiteGraphs::TVertex& node) const {
        return static_cast<size_t>(node.ComponentId << 4) ^ (node.VertexId);
    }
};


template<>
struct hash<NMultipartiteGraphs::TEdge> {
    size_t operator()(const NMultipartiteGraphs::TEdge& edge) const {
        return std::hash<NMultipartiteGraphs::TVertex>()(edge.First) ^
               std::hash<NMultipartiteGraphs::TVertex>()(edge.Second);
    }
};
}