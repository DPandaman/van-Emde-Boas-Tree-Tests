#include <iostream>
#include <cmath>
#include <vector>
#include <climits>
#include <chrono>

using namespace std;
using namespace chrono;

class VEBTree
{
public:
    int u;
    int min, max;
    VEBTree *summary;
    vector<VEBTree *> cluster;

    VEBTree(int size) : u(size), min(-1), max(-1), summary(nullptr)
    {
        if (u <= 2)
        {
            summary = nullptr;
            cluster = {};
        }
        else
        {
            int sqrt_u = upperSqrt(u);
            summary = new VEBTree(sqrt_u);
            cluster.resize(sqrt_u, nullptr);
            for (int i = 0; i < sqrt_u; i++)
                cluster[i] = new VEBTree(sqrt_u);
        }
    }

    int high(int x) const
    {
        return x / lowerSqrt(u);
    }

    int low(int x) const
    {
        return x % lowerSqrt(u);
    }

    int index(int x, int y) const
    {
        return x * lowerSqrt(u) + y;
    }

    void insert(int x)
    {
        if (min == -1)
        {
            min = max = x;
        }
        else
        {
            if (x < min)
                swap(x, min);
            if (u > 2)
            {
                int h = high(x);
                int l = low(x);
                if (cluster[h]->min == -1)
                {
                    summary->insert(h);
                    cluster[h]->insert(l);
                }
                else
                {
                    cluster[h]->insert(l);
                }
            }
            if (x > max)
                max = x;
        }
    }

    void remove(int x)
    {
        if (min == max)
        {
            min = max = -1;
        }
        else if (u == 2)
        {
            if (x == 0)
                min = 1;
            else
                min = 0;
            max = min;
        }
        else
        {
            if (x == min)
            {
                int first_cluster = summary->min;
                x = index(first_cluster, cluster[first_cluster]->min);
                min = x;
            }
            int h = high(x), l = low(x);
            cluster[h]->remove(l);
            if (cluster[h]->min == -1)
            {
                summary->remove(h);
                if (x == max)
                {
                    int summary_max = summary->max;
                    if (summary_max == -1)
                        max = min;
                    else
                        max = index(summary_max, cluster[summary_max]->max);
                }
            }
            else if (x == max)
            {
                max = index(h, cluster[h]->max);
            }
        }
    }

    bool contains(int x) const
    {
        if (x == min || x == max)
            return true;
        if (u == 2)
            return false;
        return cluster[high(x)]->contains(low(x));
    }

    int successor(int x) const
    {
        if (u == 2)
        {
            if (x == 0 && max == 1)
                return 1;
            else
                return -1;
        }
        else if (min != -1 && x < min)
        {
            return min;
        }
        else
        {
            int h = high(x), l = low(x);
            int maxLow = cluster[h]->max;
            if (maxLow != -1 && l < maxLow)
            {
                int offset = cluster[h]->successor(l);
                return index(h, offset);
            }
            else
            {
                int succCluster = summary->successor(h);
                if (succCluster == -1)
                    return -1;
                return index(succCluster, cluster[succCluster]->min);
            }
        }
    }

    int predecessor(int x) const
    {
        if (u == 2)
        {
            if (x == 1 && min == 0)
                return 0;
            else
                return -1;
        }
        else if (max != -1 && x > max)
        {
            return max;
        }
        else
        {
            int h = high(x), l = low(x);
            int minLow = cluster[h]->min;
            if (minLow != -1 && l > minLow)
            {
                int offset = cluster[h]->predecessor(l);
                return index(h, offset);
            }
            else
            {
                int predCluster = summary->predecessor(h);
                if (predCluster == -1)
                {
                    if (min != -1 && x > min)
                        return min;
                    else
                        return -1;
                }
                return index(predCluster, cluster[predCluster]->max);
            }
        }
    }

    int minimum() const
    {
        return min;
    }

    int maximum() const
    {
        return max;
    }

    void print(string prefix = "") const
    {
        cout << prefix << "u=" << u << " min=" << min << " max=" << max << endl;
        if (u > 2)
        {
            cout << prefix << "Summary:" << endl;
            summary->print(prefix + "  ");
            for (int i = 0; i < cluster.size(); ++i)
            {
                if (cluster[i]->min != -1)
                {
                    cout << prefix << "Cluster[" << i << "]:" << endl;
                    cluster[i]->print(prefix + "  ");
                }
            }
        }
    }

private:
    int upperSqrt(int u) const
    {
        return (int)pow(2, ceil(log2(u) / 2));
    }

    int lowerSqrt(int u) const
    {
        return (int)pow(2, floor(log2(u) / 2));
    }
};

// Function to calculate log(log(U)) for visualization
double loglog(int u)
{
    return log(log(u) / log(2)) / log(2);
}

int main()
{
    // Comment out and switch Section 1 and Section 2 depending on what you want to test.
    //      -> Section 1 tests insert, successor, etc.
    //      -> Section 2 tests minimum and maximum
    // Defintely can be optimized to include in one singular loop, but I am too lazy and tired right now

    vector<int> sizes = {
        256,
        1024,
        4096,
        16384,
        65536,
        262144};
    vector<int> nums = {3, 5, 2, 8, 12, 15};

    // START SECTION 1

    for (int u : sizes)
    {
        VEBTree tree(u);

        // Warm-up & multiple trials
        const int trials = 5;
        long long total_insert_time = 0;
        long long total_successor_time = 0;

        for (int t = 0; t < trials; ++t)
        {
            VEBTree tempTree(u);

            auto start = steady_clock::now();
            for (int x : nums)
                tempTree.insert(x);
            auto end = steady_clock::now();
            total_insert_time += duration_cast<nanoseconds>(end - start).count();

            start = steady_clock::now();
            for (int x : nums)
                tempTree.successor(x);
            end = steady_clock::now();
            total_successor_time += duration_cast<nanoseconds>(end - start).count();
        }

        double avg_insert_time = total_insert_time / (double)(nums.size() * trials);       // nanoseconds per operation
        double avg_successor_time = total_successor_time / (double)(nums.size() * trials); // nanoseconds per operation

        cout << "U = " << u << endl;
        cout << "log(log(U)) = " << loglog(u) << " (theoretical)" << endl;
        cout << "Avg Insert Time: " << avg_insert_time << " ns/op" << endl;
        cout << "Avg Successor Time: " << avg_successor_time << " ns/op" << endl;
        cout << "----------------------------------------" << endl;
    }

    // END SECTION 1

    // START SECTION 2

    /* for (int u : sizes)
    {
        const int trials = 5;
        long long total_min_time = 0;
        long long total_max_time = 0;

        for (int t = 0; t < trials; ++t)
        {
            VEBTree tempTree(u);
            for (int x : nums)
                tempTree.insert(x);

            auto start = steady_clock::now();
            for (int i = 0; i < nums.size(); ++i)
                tempTree.minimum();
            auto end = steady_clock::now();
            total_min_time += duration_cast<nanoseconds>(end - start).count();

            start = steady_clock::now();
            for (int i = 0; i < nums.size(); ++i)
                tempTree.maximum();
            end = steady_clock::now();
            total_max_time += duration_cast<nanoseconds>(end - start).count();
        }

        double avg_min_time = total_min_time / (double)(nums.size() * trials);
        double avg_max_time = total_max_time / (double)(nums.size() * trials);

        cout << "U = " << u << endl;
        cout << "log(log(U)) = " << loglog(u) << " (theoretical)" << endl;
        cout << "Avg Min Time: " << avg_min_time << " ns/op" << endl;
        cout << "Avg Max Time: " << avg_max_time << " ns/op" << endl;
        cout << "----------------------------------------" << endl;
    } */

    // END SECTION 2

    return 0;
}
