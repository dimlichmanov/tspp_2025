#include <atomic>
#include <vector>
#include <iostream>
#include <omp.h>

/* Граф - список смежности */
using MyGraph = std::vector<std::vector<int>>;

std::vector<int> bfs_task(const MyGraph& g, int s)
{
    std::vector<int> distances(n, -1);
    std::vector<std::atomic_bool> visited(g.size());

    for (auto& x : visited) x.store(false);

    std::vector<int> frontier, next;
    frontier.push_back(s);
    visited[s].store(true);
    distances[s] = 0;

    #pragma omp parallel
    #pragma omp single
    {
        while (!frontier.empty()) {
            next.clear();

            #pragma omp taskgroup
            {
                 /* Из каждой вершины делаем свой task */
                for (int i = 0; i < (int)frontier.size(); ++i) {
                    int u = frontier[i];
                   
                    #pragma omp task firstprivate(u) shared(next, distances, visited, g)
                    {
                        for (int v : g[u]) {
                            /* Вспоминаем TSL - атомарно читаем значение и кладём туда true */
                            if (!visited[v].exchange(true)) {
                                distances[v] = distances[u] + 1;
                                #pragma omp critical(next_push) 
                                {
                                    /* Фронт должны обновлять в критической секции*/
                                    next.push_back(v);
                                }
                            }
                        }
                    }
                }
            }
            frontier.swap(next); 
        }
    }

    return distances;
}

int main() {
    MyGraph g = {
        {1,3},   // 0      
        {0,2,4}, // 1  
        {1,5},   // 2 
        {0,4},   // 3
        {1,3,5}, // 4
        {2,4}    // 5   
    };

    int s = 0;
    auto d = bfs_task(g, s);

    std::cout << "dist from " << s << ":\n";
    for (int i = 0; i < (int)d.size(); i++)
        std::cout << i << ": " << d[i] << "\n";
}