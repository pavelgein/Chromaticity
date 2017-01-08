#include <iostream>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <cassert>

typedef unsigned INT;

struct TNode {
    size_t component;
    size_t id;
    bool operator==(const TNode& other) const {
        return (component == other.component) && (id == other.id);
    }
    bool operator<(const TNode& other) const {
        return (component < other.component) &&
               (component == other.component || id < other.id);
    }
};


std::ostream& operator<<(std::ostream& os, const TNode& node) {
    std::stringstream ss;
    ss << "TNode(" << node.component << ", " << node.id << ")";
    os << ss.str();
    return os;
}


struct TEdge {
    TNode first;
    TNode second;
    bool operator==(const TEdge& other) const {
        return ((first == other.first) && (second == other.second)) ||
               ((first == other.second) && (second == other.first));
    }
};

std::ostream& operator<<(std::ostream& os, const TEdge edge) {
    std::stringstream ss;
    ss << "TEdge(" << edge.first << ", " << edge.second << ")";
    os << ss.str();
    return os;
}

typedef std::vector<INT>  TGraph;
typedef std::unordered_set<TEdge> TEdgeSet;


bool is_adjanced(const TEdge& first, const TEdge& second) {
    return (first.first == second.first) ||
           (first.first == second.second) ||
           (first.second == second.first) ||
           (first.second == second.second);
}


bool in_same_component(const TNode& first, const TNode& second) {
    return first.component == second.component;
}


bool only_one_common_component(const TEdge& first, const TEdge& second) {
    int number_of_common_component = 0;
    if (in_same_component(first.first, second.first))
        ++number_of_common_component;
    if (in_same_component(first.first, second.second))
        ++number_of_common_component;
    if (in_same_component(first.second, second.first))
        ++number_of_common_component;
    if (in_same_component(first.second, second.second))
        ++number_of_common_component;
    return (number_of_common_component == 1);
}


bool is_xi2_subgraph(const TEdge& first, const TEdge& second) {
    return is_adjanced(first, second) &&
           only_one_common_component(first, second);
}


bool is_triangle(const TEdge& first, const TEdge& second, const TEdge& third) {
    return is_xi2_subgraph(first, second) &&
           is_xi2_subgraph(second, third) &&
           is_xi2_subgraph(third, first);
}


TEdge build_up_to_triangle(const TEdge& first, const TEdge& second) {
    TNode target1, target2;
    if (in_same_component(first.first, second.first)) {
        target1 = first.second;
        target2 = second.second;
    }

    if (in_same_component(first.first, second.second)) {
        target1 = first.second;
        target2 = second.first;
    }

    if (in_same_component(first.second, second.first)) {
        target1 = first.first;
        target2 = second.second;
    }

    if (in_same_component(first.second, second.second)) {
        target1 = first.first;
        target2 = second.first;
    }
    return TEdge{target1, target2};
}

INT c_n_2(INT n) {
    return n * (n - 1) / 2;
}

template<typename T, class TContainer>
T sum_collection(const TContainer& container, T init_value) {
    T sum = init_value;
    for (const auto& x: container) {
        sum += x;
    }
    return sum;
}

template<typename T, class TContainer>
T sigma2(const TContainer& container) {
    T ans = 0;
    for (auto iter1 = container.begin(); iter1 != container.end(); ++iter1) {
        for (auto iter2 = iter1; iter2 != container.end(); ++iter2) {
            if (iter1 == iter2) {
                continue;
            }
            ans += (*iter1) * (*iter2);
        }
    }
    return ans;
}

template<typename T, class TContainer>
T sigma3(const TContainer& container) {
    T ans = 0;
    for (auto iter1 = container.begin(); iter1 != container.end(); ++iter1) {
        for (auto iter2 = iter1; iter2 != container.end(); ++iter2) {
            if (iter1 == iter2) {
                continue;
            }
            for (auto iter3 = iter2; iter3 != container.end(); ++iter3) {
                if (iter3 == iter1 || iter3 == iter2) {
                    continue;
                }
                ans += (*iter1) * (*iter2) * (*iter3);
            }
        }
    }
    return ans;
}


template <class TContainer>
INT compute_xi1(const TGraph& graph, const TContainer& edges) {
   INT ans = 0;
   INT total_vertix = sum_collection(graph, 0);
   for (const auto& edge: edges) {
        auto first_component = edge.first.component;
        auto second_component = edge.second.component;
        ans += total_vertix - graph[first_component] - graph[second_component];
   }
   return ans;
}


template <class TContainer>
INT compute_i3(const TGraph& graph, const TContainer& edges) {
    return sigma3<INT>(graph) - compute_xi1(graph, edges) +
           count_xi_2_and_2xi_3(edges);
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

template <class TContainer>
INT compute_i4_2parts(const TGraph& graph, const TContainer& edges) {
    INT ans = 0;
    for (size_t comp1 = 0; comp1 != graph.size() - 1; ++comp1) {
        INT comp1_size = graph[comp1];
        for (size_t ind11 = 0; ind11 != comp1_size - 1; ++ind11) {
            TNode node11 = TNode{.component=comp1, .id=ind11};
            for (size_t ind12 = ind11 + 1; ind12 != comp1_size; ++ind12) {
                TNode node12 = TNode{.component=comp1, .id=ind12};

                for (size_t comp2 = comp1 + 1; comp2 != graph.size(); ++comp2) {
                    INT comp2_size = graph[comp2];
                    for (size_t ind21 = 0; ind21 != comp2_size - 1; ++ind21) {
                        TNode node21 = {.component=comp2, .id=ind21};
                        TEdge edge1{node11, node21};
                        TEdge edge2{node12, node21};
                        if (in_container(edge1, edges) ||
                            in_container(edge2, edges)) {
                                continue;
                            }

                        for (size_t ind22 = ind21 + 1;
                             ind22 != comp2_size;
                             ++ind22) {
                            TNode node22 = {.component=comp2, .id=ind22};
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


template<class TContainer, class TKey>
bool in_container(const TKey& key, const TContainer& container) {
    return container.find(key) != container.end();
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
            TNode node1 = TNode{.component=comp1, .id=ind1};
            for (size_t comp2 = 0; comp2 != graph.size(); ++comp2) {
                if (comp2 == comp1) {
                    continue;
                }
                INT comp2_size = graph[comp2];
                for (INT ind21 = 0; ind21 != comp2_size - 1; ++ind21) {
                    TNode node21{.component=comp2, .id=ind21};
                    TEdge edge1{node1, node21};
                    if (in_container(edge1, edges)) {
                        continue;
                    }

                    for (INT ind22 = ind21 + 1; ind22 != comp2_size; ++ind22) {
                        TNode node22{.component=comp2, .id=ind22};
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
                                TNode node3{.component=comp3, .id=ind3};
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

template<class TContainer>
INT count_xi_2_and_2xi_3(const TContainer& container) {
    INT xi_3 = 0;
    INT xi_2 = 0;

    typename TContainer::const_iterator iter1;
    for (iter1 = container.cbegin(); iter1 != container.cend(); ++iter1) {
        for (auto iter2 = iter1; iter2 != container.cend(); ++iter2) {
            if (is_xi2_subgraph(*iter1, *iter2)) {
                // std::cout << "Try!" << std::endl;
                // std::cout << *iter1 << " " << *iter2 << std::endl;
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

    // std::cout << xi_2 << " " << xi_3 << std::endl;

    return 2 * xi_3 + xi_2;
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


namespace std {
    template<>
    struct std::hash<TNode> {
        std::size_t operator()(const TNode& node) const {
            return static_cast<std::size_t>(node.component << 4) ^ (node.id);
        }
    };


    template<>
    struct hash<TEdge> {
        std::size_t operator()(const TEdge& edge) const {
            return std::hash<TNode>()(edge.first) ^
                   std::hash<TNode>()(edge.second);
        }
    };
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
        for (INT j = 0; j != comp2; ++j) {
            TNode node1{.component=comp1id, .id=i};
            TNode node2{.component=comp2id, .id=j};
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
    INT source_edges = sigma2<INT>(source);
    INT target_edges = sigma2<INT>(target);

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
        // print_collection(debug, combination);
        // debug << std::endl;
        TEdgeSet current_edges;
        for (auto x: combination) {
            current_edges.insert(all_edges[x]);
        }

        print_collection(debug, current_edges);
        INT i3_mod = compute_i3(target, current_edges);
        debug << "I3:" << i3_mod;
        if (i3_mod != source_i3) {
            debug << " Answer: No Reason: I3";
        } else {
            INT i4_mod = compute_i4(target, current_edges);
            debug << "I4: " << i4_mod;
            if (i4_mod == source_i4) {
                debug << " Answer: Yes";
            } else {
                debug << " Answer: No Reason: I4";
            }
        }

        debug << std::endl;
    } while (next_combination(combination, target_edges - 1));
}


//int main(int argc, const char **argv) {
int main(void) {
    // TNode first{1, 2}, second{2, 3}, third{3, 1}, fourth{1, 4};
    // TEdge edge{first, second};
    // TEdge edge2{first, third};
    // TEdge edge3{second, third};
    // TEdge edge4{third, fourth};
    // TEdge edge5{third, second};
    // std::cout << is_xi2_subgraph(edge, edge2) << std::endl;
    // std::cout << is_adjanced(edge, edge2) << std::endl;
    // std::cout << only_one_common_component(edge, edge2) << std::endl;
    // std::cout << is_triangle(edge, edge2, edge3) << std::endl;
    // std::cout << is_triangle(edge, edge2, edge4) << std::endl;
    //
    // TEdgeSet edge_set{edge, edge2, edge5};
    // for (const auto& x: edge_set)
    //     std::cout << x << std::endl;
    // std::cout << count_xi_2_and_2xi_3(edge_set) << std::endl;
    //
    TGraph graph{7, 2, 2};
    TGraph graph2{4, 4, 3};
    // TGraph graph2{6, 3, 2};
    // TNode one{0, 1}, two{1, 1};
    // TEdge edge_x{one, two};
    // TEdgeSet edge_set2{edge_x};
    // std::cout << compute_i4_2parts(graph, edge_set2) << std::endl;
    // std::cout << compute_i4_3parts(graph, edge_set2) << std::endl;
    // std::cout << compute_i4(graph, edge_set2) << std::endl;
    compare_two_graphs(graph, graph2);
    return 0;
}
