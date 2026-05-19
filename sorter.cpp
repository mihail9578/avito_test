#include "sorter.h"

std::string FrequencySorter::GetSorted() {
    if(!is_sorted_) {
        Sort();
        is_sorted_ = true;
    }
    return sorted_data_;
}

void FrequencySorter::Sort() {

}