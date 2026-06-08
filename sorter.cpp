#include "sorter.h"
#include <algorithm>
#include <cstddef>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

static constexpr size_t CHAR_RANGE = 256;

constexpr std::array<bool, CHAR_RANGE> MakeIsDelim() {
    std::array<bool, CHAR_RANGE> t{};
    for (size_t i = 0; i < CHAR_RANGE; ++i) {
        auto c = static_cast<unsigned char>(i);

        t[i] = !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
    }
    return t;
}

constexpr std::array<char, CHAR_RANGE> MakeToLower() {
    std::array<char, CHAR_RANGE> t{};
    for (size_t i = 0; i < CHAR_RANGE; ++i) {
        auto c = static_cast<unsigned char>(i);
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

constexpr bool isDelimiter(const char c) {
    return IsDelim.at(static_cast<unsigned char>(c));
}

const std::vector<std::pair<std::string_view, int>> &
FrequencySorter::GetSortedVector() {
    if (!is_sorted_) {
        Sort();
        is_sorted_ = true;
    }
    return sorted_;
}

std::string_view FrequencySorter::HandleWord(std::string &data, size_t &iter,
                                             size_t data_size) {
    size_t begin = iter;

    while (iter < data_size && !isDelimiter(data[iter])) {
        data_[iter] = ToLower.at(static_cast<unsigned char>(data_[iter]));
        ++iter;
    }
    const size_t len = iter - begin;

    auto str = std::string_view(&data_[begin], len);

    while (iter < data_size && isDelimiter(data[iter])) {
        ++iter;
    }
    return str;
}

void FrequencySorter::CollectWords(
    size_t from, size_t to, std::unordered_map<std::string_view, int> &map) {
    size_t i = from;

    while (isDelimiter(data_[i])) {
        ++i;
    }

    while (i < to) {
        map[HandleWord(data_, i, data_.size())]++;
    }
}

void FrequencySorter::Sort() {
    if (data_.empty()) {
        return;
    }
    std::vector<std::thread> threads;
    int area_size = data_.size() / nthreads_;
    size_t from = 0;
    std::unordered_map<std::string_view, int> global_map;
    std::vector<std::unordered_map<std::string_view, int>> local_maps(
        nthreads_);

    for (auto &map : local_maps) {
        map.reserve(area_size / 5); // оценка количества уникальных слов
    }

    for (int t = 0; t < nthreads_; ++t) {
        size_t to = (t == nthreads_ - 1)
                        ? data_.size()
                        : std::min(data_.size(), from + area_size);

        while (to < data_.size() && !isDelimiter(data_[to])) {
            ++to;
        }

        threads.emplace_back(&FrequencySorter::CollectWords, this, from, to,
                             std::ref(local_maps[t]));

        from = to;
    }

    for (auto &th : threads) {
        th.join();
    }

    if (nthreads_ == 1) {
        global_map = std::move(local_maps[0]);
    } else {
        global_map.swap(*local_maps.begin());
        local_maps.erase(local_maps.begin());
        for (auto &lm : local_maps) {
            for (auto &[word, cnt] : lm) {
                global_map[word] += cnt;
            }
        }
    }

    std::vector<std::pair<std::string_view, int>> sorted(global_map.begin(),
                                                         global_map.end());

    // std::execution::par can be used
    std::sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    });
    sorted_ = std::move(sorted);
}
