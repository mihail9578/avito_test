#include "sorter.h"
#include <algorithm>
#include <cstddef>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>


std::vector<std::pair<std::string_view, int>>
FrequencySorter::GetSortedVector() {
    if (!is_sorted_) {
        Sort();
        is_sorted_ = true;
    }
    return sorted_;
}

bool isDelimiter(char c) {
    if (c >= 'a' && c <= 'z') {
        return false;
    }

    if (c >= 'A' && c <= 'Z') {
        return false;
    }

    return true;
}

std::string HandleWord(const std::string_view& data, size_t &iter) {
    int begin = iter;
    while (!isDelimiter(data[iter])) {
        iter++;
    }
    std::string str(data, begin, iter - begin);

    for (auto &c : str) {
        c = (c >= 'A' && c <= 'Z') ? c - ('A' - 'a') : c;
    }

    while (iter < data.size() && isDelimiter(data[iter])) {
        iter++;
    }
    return str;
}

void FrequencySorter::CollectWords(size_t from, size_t to) {
    std::unordered_map<std::string, int> map;
    size_t i = from;
    while (i < to) {
        map[HandleWord(data_, i)]++;
    }

    std::lock_guard lk(mtx_);
    for (const auto &el : map) {
        map_[el.first] += el.second;
    }
}

void FrequencySorter::Sort() {
    std::vector<std::thread> threads;
    int area_size = data_.size() / nthreads_;
    size_t from = 0;
    size_t to = area_size;

    while(from < data_.size() && isDelimiter(data_[from])) from++;

    while (to < data_.size()) {
        while (to < data_.size() &&
               !(isDelimiter(data_[to]) && !isDelimiter(data_[to + 1])))
            to++;
        threads.emplace_back(&FrequencySorter::CollectWords, this, from, to);

        from = ++to;
        to = to + area_size;
    }

    if (from < data_.size()) {
        threads.emplace_back(&FrequencySorter::CollectWords, this, from,
                             data_.size());
    }

    for (auto &th : threads) {
        th.join();
    }

    std::vector<std::pair<std::string_view, int>> sorted(map_.begin(),
                                                         map_.end());

    // std::execution::par can be used
    std::sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) {
        if (a.second != b.second)
            return a.second > b.second;
        return a.first < b.first;
    });
    sorted_ = std::move(sorted);
}