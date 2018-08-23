#include <utility>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <cassert>

#include "math_utils.h"
#include "multipartite_graphs.h"

typedef NMultipartiteGraphs::TEdge TEdge;
typedef std::vector<INT> TGraph;
typedef std::unordered_set<TEdge> TEdgeSet;


template<class TContainer, class TKey>
bool in_container(const TKey& key, const TContainer& container) {
    return container.find(key) != container.end();
}

INT compute_i4(const TGraph& graph) {
    INT ans = 0;
    for (size_t i = 0; i != graph.size() - 1; ++i) {
        for (size_t j = i + 1; j != graph.size(); ++j) {
            ans += c_n_2(graph[i]) * c_n_2(graph[j]);
        }
    }
    return ans;
}


bool is_adjanced(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second) {
    return (first.First == second.First) ||
           (first.First == second.Second) ||
           (first.Second == second.First) ||
           (first.Second == second.Second);
}


bool in_same_component(const NMultipartiteGraphs::TVertex& first, const NMultipartiteGraphs::TVertex& second) {
    return first.ComponentId == second.ComponentId;
}


bool only_one_common_component(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second) {
    int number_of_common_component = 0;
    if (in_same_component(first.First, second.First))
        ++number_of_common_component;
    if (in_same_component(first.First, second.Second))
        ++number_of_common_component;
    if (in_same_component(first.Second, second.First))
        ++number_of_common_component;
    if (in_same_component(first.Second, second.Second))
        ++number_of_common_component;
    return (number_of_common_component == 1);
}


bool is_xi2_subgraph(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second) {
    return is_adjanced(first, second) &&
           only_one_common_component(first, second);
}


bool is_triangle(const NMultipartiteGraphs::TEdge& first, const NMultipartiteGraphs::TEdge& second, const NMultipartiteGraphs::TEdge& third) {
    return is_xi2_subgraph(first, second) &&
           is_xi2_subgraph(second, third) &&
           is_xi2_subgraph(third, first);
}


TEdge build_up_to_triangle(const TEdge& first, const TEdge& second) {
    NMultipartiteGraphs::TVertex target1, target2;
    if (in_same_component(first.First, second.First)) {
        target1 = first.Second;
        target2 = second.Second;
    }

    if (in_same_component(first.First, second.Second)) {
        target1 = first.Second;
        target2 = second.First;
    }

    if (in_same_component(first.Second, second.First)) {
        target1 = first.First;
        target2 = second.Second;
    }

    if (in_same_component(first.Second, second.Second)) {
        target1 = first.First;
        target2 = second.First;
    }
    return TEdge{target1, target2};
}


template <class TContainer>
INT compute_xi1(const TGraph& graph, const TContainer& edges) {
   INT ans = 0;
   INT total_vertix = sum_collection(graph, 0);
   for (const auto& edge: edges) {
        auto first_component = edge.First.ComponentId;
        auto second_component = edge.Second.ComponentId;
        ans += total_vertix - graph[first_component] - graph[second_component];
   }
   return ans;
}

template<class TContainer>
INT count_xi_2_and_2xi_3(const TContainer& container) {
    INT xi_3 = 0;
    INT xi_2 = 0;

    typename TContainer::const_iterator iter1;
    for (iter1 = container.cbegin(); iter1 != container.cend(); ++iter1) {
        for (auto iter2 = iter1; iter2 != container.cend(); ++iter2) {
            if (is_xi2_subgraph(*iter1, *iter2)) {
                TEdge additional_edge = build_up_to_triangle(*iter1, *iter2);
                if (container.find(additional_edge) != container.end()) {
                    ++xi_3;
                }
                ++xi_2;
            }
        }
    }

    xi_2 = xi_2 - xi_3;
    xi_3 /= 3;

    return 2 * xi_3 + xi_2;
}


template <class TContainer>
INT compute_i3(const TGraph& graph, const TContainer& edges) {
    return sigma3<INT>(graph) - compute_xi1(graph, edges) +
           count_xi_2_and_2xi_3(edges);
}


template <class TContainer>
INT compute_i4_2parts(const TGraph& graph, const TContainer& edges) {
    INT ans = 0;
    for (size_t comp1 = 0; comp1 != graph.size() - 1; ++comp1) {
        size_t comp1_size = graph[comp1];
        for (INT ind11 = 0; ind11 != comp1_size - 1; ++ind11) {
            NMultipartiteGraphs::TVertex node11{comp1, ind11};
            for (INT ind12 = ind11 + 1; ind12 != comp1_size; ++ind12) {
                NMultipartiteGraphs::TVertex node12{comp1, ind12};

                for (size_t comp2 = comp1 + 1; comp2 != graph.size(); ++comp2) {
                    size_t comp2_size = graph[comp2];
                    for (INT ind21 = 0; ind21 != comp2_size - 1; ++ind21) {
                        NMultipartiteGraphs::TVertex node21{comp2, ind21};
                        TEdge edge1{node11, node21};
                        TEdge edge2{node12, node21};
                        if (in_container(edge1, edges) ||
                            in_container(edge2, edges)) {
                                continue;
                            }

                        for (INT ind22 = ind21 + 1;
                             ind22 != comp2_size;
                             ++ind22) {
                            NMultipartiteGraphs::TVertex node22{comp2, ind22};
                            TEdge edge3{node11, node22};
                            TEdge edge4{node12, node22};
                            if (!in_container(edge3, edges) &&
                                !in_container(edge4, edges)) {
                                    ans += 1;
                                }
                        }
                    }
                }
            }
        }
    }
    return ans;
}

template<class TContainer>
INT compute_i4_3parts(const TGraph& graph, const TContainer& edges) {
    INT ans = 0;
    if (graph.size() < 3) {
        return 0;
    }
    for (size_t comp1 = 0; comp1 != graph.size() - 1; ++comp1) {
        INT comp1_size = graph[comp1];
        for (INT ind1 = 0; ind1 != comp1_size; ++ind1) {
            NMultipartiteGraphs::TVertex node1 = NMultipartiteGraphs::TVertex{comp1, ind1};
            for (size_t comp2 = 0; comp2 != graph.size(); ++comp2) {
                if (comp2 == comp1) {
                    continue;
                }
                INT comp2_size = graph[comp2];
                for (INT ind21 = 0; ind21 != comp2_size - 1; ++ind21) {
                    NMultipartiteGraphs::TVertex node21{comp2, ind21};
                    TEdge edge1{node1, node21};
                    if (in_container(edge1, edges)) {
                        continue;
                    }

                    for (INT ind22 = ind21 + 1; ind22 != comp2_size; ++ind22) {
                        NMultipartiteGraphs::TVertex node22{comp2, ind22};
                        TEdge edge2{node1, node22};
                        if (in_container(edge2, edges)) {
                            continue;
                        }

                        for (size_t comp3 = comp1 + 1; comp3 != graph.size(); ++comp3) {
                            if (comp3 == comp2 || comp3 == comp1) {
                                continue;
                            }
                            INT comp3_size = graph[comp3];
                            for (INT ind3 = 0; ind3 != comp3_size; ++ind3) {
                                NMultipartiteGraphs::TVertex node3{comp3, ind3};
                                TEdge edge3{node3, node21};
                                TEdge edge4{node3, node22};
                                TEdge edge5{node1, node3};

                                if (!in_container(edge3, edges) &&
                                    !in_container(edge4, edges) &&
                                    in_container(edge5, edges)) {
                                        ++ans;
                                    }
                            }
                        }
                    }
                }
            }
        }
    }
    return ans;
}

template<class TContainer>
INT compute_i4(const TGraph& graph, const TContainer& edges) {
    return compute_i4_2parts(graph, edges) +
           compute_i4_3parts(graph, edges);
}


std::ostream& operator<<(std::ostream& outp, const TGraph& graph) {
    outp << "Graph(";
    for (auto iter = graph.begin(); iter != graph.end(); ++iter) {
        if (iter != graph.begin()) {
            outp << ",";
        }
        outp << *iter;
    }
    outp << ")";
    return outp;
}



template<class TContainer>
std::ostream& print_collection(std::ostream& outp, const TContainer& container) {
    for (const auto& x: container) {
        outp << x << " ";
    }
    return outp;
}

bool is_same(const TGraph& first, const TGraph& second) {
    if (first.size() != second.size()) {
        return false;
    }

    for (size_t i = 0; i != first.size(); ++i) {
        if (first[i] != second[i]) {
            return false;
        }
    }
    return true;
}


std::vector<TEdge> generate_all_edges(INT comp1, INT comp2, size_t comp1id,
        size_t comp2id) {
    std::vector<TEdge> ans(comp1 * comp2);
    for (INT i = 0; i != comp1; ++i) {
        NMultipartiteGraphs::TVertex node1{comp1id, i};
        for (INT j = 0; j != comp2; ++j) {
            NMultipartiteGraphs::TVertex node2{comp2id, j};
            ans[i * comp2 + j] = TEdge{node1, node2};
        }
    }
    return ans;
}

bool next_combination(std::vector<size_t>& combination, size_t n) {
    size_t k = combination.size();
    for (size_t i = k; i-- > 0; )
        if (combination[i] < n - k + i + 1) {
            ++combination[i];
            for (size_t j = i + 1; j < k; ++j)
                combination[j] = combination[j - 1 ] + 1;
            return true;
        }
    return false;
}


//bool solve_case(const TGraph& source, const TGraph& target, TEdgeSet current_edges, std::ostream& debug) {
    //return true;
//}

void compare_two_graphs(const TGraph& source, const TGraph target,
    std::ostream& debug=std::cout) {
    debug << "Checking graphs" << std::endl;
    debug << "Source: " << source << std::endl;
    debug << "Target: " << target << std::endl;

    debug << "Checking vertices" << std::endl;
    INT source_vertixes = sum_collection(source, 0);
    INT target_vertixes = sum_collection(target, 0);

    debug << "Source: " << source_vertixes << std::endl;
    debug << "Target: " << target_vertixes << std::endl;
    if (source_vertixes != target_vertixes) {
        debug << "Check failed" << std::endl;
        debug << "Answer: No" << std::endl;
        debug << "Reason: Vertises numbers mismatches" << std::endl;
        return;
    } else {
        debug << "Check passed" << std::endl;
    }

    debug << "Checking edges" << std::endl;
    INT source_edges = sigma2(source);
    INT target_edges = sigma2(target);

    debug << "Source: " << source_edges << std::endl;
    debug << "Target: " << target_edges << std::endl;

    if (source_edges > target_edges) {
        debug << "Check failed" << std::endl;
        debug << "Answer: No" << std::endl;
        debug << "Reason: Source contains more edges" << std::endl;
        return;
    } else if (source_edges == target_edges) {
        if (is_same(source, target)) {
            debug << "Check passed" << std::endl;
            debug << "Answer: Yes" << std::endl;
            debug << "Reason: Graphs are equal" << std::endl;
        } else {
            debug << "Check failed" << std::endl;
            debug << "Answer: No" << std::endl;
            debug << "Reason: Two different multipartie graphs" << std::endl;
        }
        return;
    }

    INT edge_diff = target_edges - source_edges;
    debug << "Difference: " << edge_diff << std::endl;
    assert(source.size() == 3);


    INT source_i3 = sigma3<INT>(source);
    INT target_i3 = sigma3<INT>(target);

    INT source_i4 = compute_i4(source);
    INT target_i4 = compute_i4(target);

    debug << "Source I3: " << source_i3 << std::endl;
    debug << "Target I3: " << target_i3 << std::endl;
    debug << "Source I4: " << source_i4 << std::endl;
    debug << "Target I4: " << target_i4 << std::endl;
    debug << std::flush;

    std::vector<TEdge> edges12 = generate_all_edges(target[0], target[1], 0, 1);
    std::vector<TEdge> edges13 = generate_all_edges(target[0], target[2], 0, 2);
    std::vector<TEdge> edges23 = generate_all_edges(target[1], target[2], 1, 2);

    std::vector<TEdge> all_edges;
    all_edges.reserve(target_edges);
    all_edges.insert(all_edges.end(), edges12.begin(), edges12.end());
    all_edges.insert(all_edges.end(), edges13.begin(), edges13.end());
    all_edges.insert(all_edges.end(), edges23.begin(), edges23.end());
    std::vector<size_t> combination(edge_diff);
    for (size_t ind = 0; ind != edge_diff; ++ind) {
        combination[ind] = ind;
    }

    do {
        TEdgeSet current_edges;
        for (auto x: combination) {
            current_edges.insert(all_edges[x]);
        }

        print_collection(debug, current_edges);
        INT i3_mod = compute_i3(target, current_edges);
        debug << "I3:" << i3_mod << " ";
        if (i3_mod != source_i3) {
            debug << "Answer: No Reason: I3";
        } else {
            INT i4_mod = compute_i4(target, current_edges);
            debug << "I4: " << i4_mod;
            if (i4_mod == source_i4) {
                debug << " Answer: Yes";
            } else {
                debug << " Answer: No Reason: I4";
            }
        }

        debug << " ";
        int c_e12 = 0;
        int c_e13 = 0;
        int c_e23 = 0;
        for (const auto& edge: current_edges) {
            auto firstComponent = edge.First.ComponentId;
            auto secondComponent = edge.Second.ComponentId;
            if (firstComponent > secondComponent) {
                std::swap(firstComponent, secondComponent);
            }

            if (firstComponent == 1) {
                if (secondComponent == 2) {
                    c_e12 += 1;
                } else {
                    c_e13 += 1;
                }
            } else {
                c_e23 += 1;
            }
        }

        debug << "e12=" << c_e12 << ", e23=" << c_e23 << ", e13=" << c_e13;
        debug << std::endl;
    } while (next_combination(combination, target_edges - 1));
}


int main(void) {
    TGraph graph{7, 2, 2};
    TGraph graph2{4, 4, 3};
    compare_two_graphs(graph, graph2);
    return 0;
}
