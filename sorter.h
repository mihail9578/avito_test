#include <array>
#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class FrequencySorter {
public:
    FrequencySorter(std::string data, int nthreads = 1)
        : data_(std::move(data)), nthreads_(nthreads) {
        if (nthreads_ < 1) {
            throw std::runtime_error("Threads number should be > 0");
        }
    }

    const std::vector<std::pair<std::string_view, int>> &GetSortedVector();
    ~FrequencySorter() = default;

    FrequencySorter() = delete;
    FrequencySorter(const FrequencySorter &fs) = delete;
    FrequencySorter &operator=(const FrequencySorter &fs) = delete;
    FrequencySorter(FrequencySorter &&fs) = delete;
    FrequencySorter &operator=(FrequencySorter &&fs) = delete;

private:
    void Sort();
    void CollectWords(size_t from, size_t to,
                      std::unordered_map<std::string_view, int> &map);

    std::string_view HandleWord(std::string &data, size_t &iter,
                                size_t data_size);

    std::string data_;
    bool is_sorted_ = false;
    int nthreads_ = 1;
    std::vector<std::pair<std::string_view, int>> sorted_;
};
