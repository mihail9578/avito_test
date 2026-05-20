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

std::string HandleWord(const std::string_view &data, size_t &iter) {
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

    while (isDelimiter(data_[i]))
        i++;

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

    if (to <= 0) {
        return;
    }

    while (to <= data_.size()) {
        int word_offset = 0;
        if (!isDelimiter(data_[to - 1])) {
            while (to + word_offset < data_.size() &&
                   !isDelimiter(data_[to + word_offset]))
                word_offset++;
        }
        if (from >= to + word_offset) {
            continue;
        }

        threads.emplace_back(&FrequencySorter::CollectWords, this, from,
                             to + word_offset);

        from = to + word_offset;
        if (data_.size() - (to + area_size) < area_size ||
            to / area_size == nthreads_ - 1) {
            to = data_.size();
        } else {
            to += area_size;
        }
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