#include <string_view>
#include <string>

class FrequencySorter {
public:
    FrequencySorter(std::string_view data) : raw_data_(data) {}

    std::string GetSorted();

    FrequencySorter() = delete;
    FrequencySorter(const FrequencySorter &fs) = delete;
    FrequencySorter &operator=(const FrequencySorter &fs) = delete;
    FrequencySorter(FrequencySorter &&fs) = delete;
    FrequencySorter &operator=(FrequencySorter &&fs) = delete;

private:
    void Sort();

    std::string_view raw_data_;
    std::string sorted_data_;
    bool is_sorted_ = false;
};