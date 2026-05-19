#include <cstddef>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class FrequencySorter {
public:
    FrequencySorter(std::string_view data, int nthreads = 1)
        : data_(data), nthreads_(nthreads) {}

    std::vector<std::pair<std::string_view, int>> GetSortedVector();

    FrequencySorter() = delete;
    FrequencySorter(const FrequencySorter &fs) = delete;
    FrequencySorter &operator=(const FrequencySorter &fs) = delete;
    FrequencySorter(FrequencySorter &&fs) = delete;
    FrequencySorter &operator=(FrequencySorter &&fs) = delete;

private:
    void Sort();
    void CollectWords(size_t from, size_t to);

    std::string_view data_;
    bool is_sorted_ = false;
    int nthreads_ = 1;
    std::mutex mtx_;
    std::unordered_map<std::string, int> map_;
    std::vector<std::pair<std::string_view, int>> sorted_;
};