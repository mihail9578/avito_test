#include "sorter.h"
#include <algorithm>
#include <cstddef>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

constexpr auto MakeIsDelim() {
    std::array<bool, 256> t{};
    for (int i = 0; i < 256; ++i) {
        unsigned char c = static_cast<unsigned char>(i);

        t[i] = !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
    }
    return t;
}

constexpr auto MakeToLower() {
    std::array<char, 256> t{};
    for (int i = 0; i < 256; ++i) {
        unsigned char c = static_cast<unsigned char>(i);
        if (c >= 'A' && c <= 'Z') {
            t[i] = static_cast<char>(c + ('a' - 'A'));
        } else {
            t[i] = c;
        }
    }
    return t;
}

static constexpr auto IsDelim = MakeIsDelim();
static constexpr auto ToLower = MakeToLower();

constexpr std::array<bool, 256> InitDelimiterTable() {
    std::array<bool, 256> table{};
    for (int i = 0; i < 256; ++i) {
        unsigned char ch = static_cast<unsigned char>(i);
        table[i] = !((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
    }
    return table;
}

inline bool isDelimiter(const char c) {
    return IsDelim[static_cast<unsigned char>(c)];
}

std::vector<std::pair<std::string_view, int>>
FrequencySorter::GetSortedVector() {
    if (!is_sorted_) {
        Sort();
        is_sorted_ = true;
    }
    return sorted_;
}

void FrequencySorter::HandleWord(const std::string_view &data, size_t &iter,
                                 std::string &word) {
    word.clear();
    const size_t size = data.size();
    size_t begin = iter;

    while (iter < size && !isDelimiter(data[iter])) {
        ++iter;
    }
    const size_t len = iter - begin;

    word.reserve(len);
    for (size_t i = begin; i < iter; ++i) {
        word.push_back(ToLower[static_cast<unsigned char>(data[i])]);
    }

    while (iter < size && isDelimiter(data[iter])) {
        ++iter;
    }
}

void FrequencySorter::CollectWords(size_t from, size_t to,
                                   std::unordered_map<std::string, int> &map) {
    size_t i = from;

    while (isDelimiter(data_[i]))
        i++;

    std::string buf;
    while (i < to) {
        HandleWord(data_, i, buf);
        map[buf]++;
    }
}

void FrequencySorter::Sort() {
    if (data_.size() == 0) {
        return;
    }
    std::vector<std::thread> threads;
    int area_size = data_.size() / nthreads_;
    size_t from = 0;
    size_t to = area_size;
    std::vector<std::unordered_map<std::string, int>> local_maps(nthreads_);
    int i = 0;
    while (to <= data_.size()) {
        int word_offset = 0;
        if (!isDelimiter(data_[to - 1])) {
            while (to + word_offset < data_.size() &&
                   !isDelimiter(data_[to + word_offset]))
                word_offset++;
        }
        if (from < to + word_offset) {
            threads.emplace_back(&FrequencySorter::CollectWords, this, from,
                                 to + word_offset, std::ref(local_maps[i++]));
        }

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

    for (auto &lm : local_maps) {
        for (auto &[word, cnt] : lm) {
            map_[std::move(word)] += cnt;
        }
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