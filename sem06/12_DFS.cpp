#include <atomic>
#include <vector>
#include <iostream>
#include <omp.h>

/* Граф - список смежности */
using MyGraph = std::vector<std::vector<int>>;

struct DfsResult {
    std::vector<int> parent; 
    std::vector<int> order;  //допустим, мы хотим наши вершины пронумеровать
};

/* u - вершина для обработки, p - родительская для неё */ 
void dfs_task(const MyGraph& g, int u, int p,
                    std::vector<std::atomic_bool>& visited,
                    std::vector<int>& parent,
                    std::vector<int>& order,
                    int& counter)
{
    if (visited[u].exchange(true)) return;

    parent[u] = p;
    int my_order;

    /* Атомарно прибавили 1 к порядку, в order записали что там лежало. То же самое, что и fetch_add */
    #pragma omp atomic capture
    my_order = counter++;

    order[u] = my_order;

    for (int v : g[u]) {
        if (v == p) continue;

        #pragma omp task firstprivate(v, u) shared(g, visited, parent, order, counter)
        dfs_task(g, v, u, visited, parent, order, counter);
    }
}

DfsResult parallel_dfs(const MyGraph& g, int s)
{
    std::vector<std::atomic_bool> visited(g.size());
    for (auto& x : visited) x.store(false);

    DfsResult result;
    result.parent.assign(g.size(), -1);
    result.order.assign(g.size(), -1);

    int counter = 0;

    #pragma omp parallel
    #pragma omp single nowait
    {
        #pragma omp taskgroup
        {
            dfs_task(g, s, -1, visited, result.parent, result.order, counter);
        }
    }

    return result;
}

int main()
{
    MyGraph g = {
        {1,3},   // 0
        {0,2,4}, // 1
        {1,5},   // 2
        {0,4},   // 3
        {1,3,5}, // 4
        {2,4,6}, // 5
        {5}      // 6
    };

    int s = 0;
    auto r = parallel_dfs(g, s);

    std::cout << "parent / order:\n";
    for (int i = 0; i < (int)g.size(); ++i) {
        std::cout << i << " : " << r.parent[i] << " , " << r.order[i] << "\n";
    }

    return 0;
}