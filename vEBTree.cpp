#include <iostream>
#include <cmath>
#include <vector>
#include <climits>
#include <chrono>

class VEBTree {
public:
    int u;
    int min, max;
    VEBTree *summary;
    std::vector<VEBTree *> cluster;

    VEBTree(int size) : u(size), min(-1), max(-1), summary(nullptr) {
        if (u <= 2) {
            summary = nullptr;
            cluster = {};
        } else {
            int sqrt_u = upperSqrt(u);
            summary = new VEBTree(sqrt_u);
            cluster.resize(sqrt_u, nullptr);
            for (int i = 0; i < sqrt_u; i++)
                cluster[i] = new VEBTree(sqrt_u);
        }
    }

    // determines the cluster index containing the value x
    int high(int x) const {
        return x / lowerSqrt(u);
    }

    // determines the position of x within that cluster from high(x)
    int low(int x) const {
        return x % lowerSqrt(u);
    }

    int index(int x, int y) const {
        return x * lowerSqrt(u) + y;
    }

    // insert value x into the vEB tree
    void insert(int x) {
        if (min == -1) {
            min = max = x;
        } else {
            if (x < min) {
                std::swap(x, min);
            }
            if (u > 2) {
                int h = high(x);
                int l = low(x);
                if (cluster[h]->min == -1) {
                    summary->insert(h);
                    cluster[h]->insert(l);
                } else {
                    cluster[h]->insert(l);
                }
            }
            if (x > max) {
                max = x;
            }
        }
    }

    // remove value x from the vEB tree
    void remove(int x) {
        if (min == max) {
            min = max = -1;
        } else if (u == 2) {
            if (x == 0) {
                min = 1;
            } else {
                min = 0;
            }
            max = min;
        } else {
            if (x == min) {
                int first_cluster = summary->min;
                x = index(first_cluster, cluster[first_cluster]->min);
                min = x;
            }
            int h = high(x), l = low(x);
            cluster[h]->remove(l);
            if (cluster[h]->min == -1) {
                summary->remove(h);
                if (x == max) {
                    int summary_max = summary->max;
                    if (summary_max == -1) {
                        max = min;
                    } else {
                        max = index(summary_max, cluster[summary_max]->max);
                    }
                }
            }
            else if (x == max) {
                max = index(h, cluster[h]->max);
            }
        }
    }

    // checks if value x is contained in the vEB tree
    bool contains(int x) const {
        if (x == min || x == max) {
            return true;
        }
        if (u == 2) {
            return false;
        }
        return cluster[high(x)]->contains(low(x));
    }

    // finds the successor for value x
    int successor(int x) const {
        if (u == 2) {
            if (x == 0 && max == 1) {
                return 1;
            } else {
                return -1;
            }
        } else if (min != -1 && x < min) {
            return min;
        } else {
            int h = high(x), l = low(x);
            int maxLow = cluster[h]->max;
            if (maxLow != -1 && l < maxLow) {
                int offset = cluster[h]->successor(l);
                return index(h, offset);
            } else {
                int succCluster = summary->successor(h);
                if (succCluster == -1) {
                    return -1;
                }
                return index(succCluster, cluster[succCluster]->min);
            }
        }
    }

    // finds the predecessor for value x
    int predecessor(int x) const {
        if (u == 2) {
            if (x == 1 && min == 0) {
                return 0;
            } else {
                return -1;
            }
        } else if (max != -1 && x > max) {
            return max;
        } else {
            int h = high(x), l = low(x);
            int minLow = cluster[h]->min;
            if (minLow != -1 && l > minLow) {
                int offset = cluster[h]->predecessor(l);
                return index(h, offset);
            } else {
                int predCluster = summary->predecessor(h);
                if (predCluster == -1) {
                    if (min != -1 && x > min) {
                        return min;
                    } else {
                        return -1;
                    }
                }
                return index(predCluster, cluster[predCluster]->max);
            }
        }
    }

    // returns the minimum value in the vEB tree. Runs in constant time
    int minimum() const {
        return min;
    }

     // returns the maximum value in the vEB tree. Runs in constant time
    int maximum() const {
        return max;
    }

    // helper function that prints out the entire tree in alphanumeric form
    void print(std::string prefix = "") const {
        std::cout << prefix << "u=" << u << " min=" << min << " max=" << max << std::endl;
        if (u > 2) {
            std::cout << prefix << "Summary:" << std::endl;
            summary->print(prefix + "  ");
            for (int i = 0; i < cluster.size(); i++) {
                if (cluster[i]->min != -1) {
                    std::cout << prefix << "Cluster[" << i << "]:" << std::endl;
                    cluster[i] -> print(prefix + "  ");
                }
            }
        }
    }

private:
    // used for primary functions

    int upperSqrt(int u) const {
        return (int)pow(2, ceil(log2(u) / 2));
    }

    int lowerSqrt(int u) const {
        return (int)pow(2, floor(log2(u) / 2));
    }
};

double loglog(int u) {
    return log(log(u) / log(2)) / log(2);
}

// section #1 tests the primary O(log(log(U))) functions
//      -> Insert/Remove, Successor/Predecessor, etc.
void testSectionOne(std::vector<int> sizes, std::vector<int> nums) {
    for (int u : sizes) {
        VEBTree tree(u);

        const int trials = 5;
        long long total_insert_time = 0;
        long long total_successor_time = 0;

        for (int t = 0; t < trials; t++) {
            VEBTree tempTree(u);

            auto start = std::chrono::steady_clock::now();
            for (int x : nums) {
                tempTree.insert(x);
            }
            auto end = std::chrono::steady_clock::now();
            total_insert_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::steady_clock::now();
            for (int x : nums) {
                tempTree.successor(x);
            }
            end = std::chrono::steady_clock::now();
            total_successor_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }

        double avg_insert_time = total_insert_time / (double)(nums.size() * trials);      
        double avg_successor_time = total_successor_time / (double)(nums.size() * trials); 

        std::cout << "U = " << u << std::endl;
        std::cout << "log(log(U)) = " << loglog(u) << " (theoretical)" << std::endl;
        std::cout << "Avg Insert Time: " << avg_insert_time << " ns/op" << std::endl;
        std::cout << "Avg Successor Time: " << avg_successor_time << " ns/op" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
}

// section #2 tests the O(1) constant time functions
//      -> minimum/maximum
void testSectionTwo(std::vector<int> sizes, std::vector<int> nums) {
    for (int u : sizes) {
        const int trials = 5;
        long long total_min_time = 0;
        long long total_max_time = 0;

        for (int t = 0; t < trials; t++) {
            VEBTree tempTree(u);
            for (int x : nums)
                tempTree.insert(x);

            auto start = std::chrono::steady_clock::now();
            for (int i = 0; i < nums.size(); i++) {
                tempTree.minimum();
            }
            auto end = std::chrono::steady_clock::now();
            total_min_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::steady_clock::now();
            for (int i = 0; i < nums.size(); i++) {
                tempTree.maximum();
            }
            end = std::chrono::steady_clock::now();
            total_max_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }

        double avg_min_time = total_min_time / (double)(nums.size() * trials);
        double avg_max_time = total_max_time / (double)(nums.size() * trials);

        std::cout << "U = " << u << std::endl;
        std::cout << "log(log(U)) = " << loglog(u) << " (theoretical)" << std::endl;
        std::cout << "Avg Min Time: " << avg_min_time << " ns/op" << std::endl;
        std::cout << "Avg Max Time: " << avg_max_time << " ns/op" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
}

int main() {
    // different universe U values to run tests for (k = {8, 10, 12, 14, 16, 18})
    std::vector<int> sizes = {
        256,
        1024,
        4096,
        16384,
        65536,
        262144};

    // set N to test the vEB tree functions on
    std::vector<int> nums = {3, 5, 2, 8, 12, 15};

    std::cout << "Testing Section 1" << std::endl;
    testSectionOne(sizes, nums);
    std::cout << "Testing Section 2" << std::endl;
    testSectionTwo(sizes, nums);
    std::cout << "Done Testing" << std::endl;

    return 0;
}
