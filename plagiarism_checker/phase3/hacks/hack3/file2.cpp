#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Structure to represent an edge
struct Edge {
    int u, v, weight;
    // Define equality operator
    bool operator==(const Edge& other) const {
        return u == other.u && v == other.v && weight == other.weight;
    }
};

// Comparator to sort edges by weight in ascending order
bool compareAscending(const Edge& a, const Edge& b) {
    return a.weight < b.weight;
}

// Disjoint Set (Union-Find) class for connectivity checks
class UnionFind {
private:
    vector<int> parent, rank;

public:
    // Constructor to initialize the Union-Find structure
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++) {
            parent[i] = i; // Each node is its own parent initially
        }
    }

    // Find function with path compression
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // Union function with rank-based merging
    void unionSets(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX != rootY) {
            if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            } else if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            } else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }

    // Check if two nodes are in the same set
    bool isConnected(int x, int y) {
        return find(x) == find(y);
    }
};


// Kruskal's Algorithm for MST
vector<Edge> kruskalMST(int n, vector<Edge>& edges) {
    // Sort edges in ascending order by weight
    sort(edges.begin(), edges.end(), compareAscending);

    // Initialize Union-Find for connectivity
    UnionFind uf(n);

    // Store the edges of the Minimum Spanning Tree
    vector<Edge> mst;

    for (const Edge& edge : edges) {
        // If the edge does not form a cycle
        if (uf.find(edge.u) != uf.find(edge.v)) {
            mst.push_back(edge);           // Include the edge in the MST
            uf.unionSets(edge.u, edge.v); // Union the two sets
        }

        // Stop when we have n - 1 edges in the MST
        if (mst.size() == n - 1) {
            break;
        }
    }

    return mst;
}

class Solution {
public:
    int minCostConnectPoints(vector<vector<int>>& points) {
        int npoints = points.size();

        vector<Edge> edges;

        for (int i = 0; i < npoints - 1; i++){
            int x1 = points[i][0];
            int y1 = points[i][1];
            for (int j = i; j < npoints; j++){
                int x2 = points[j][0];
                int y2 = points[j][1];
                int distance = abs(x1 - x2) + abs(y1 - y2);
                Edge edge = {i, j, distance};
                edges.push_back(edge);
            }
        }

        vector<Edge> minimum_edges = kruskalMST(npoints, edges);

        int total_cost = 0;
        for (Edge & edge : minimum_edges){
            total_cost += edge.weight;
        }

        return total_cost;
    }
};