#include "shift_register.h"

namespace blocks {

template <typename T>
ShiftRegister<T>::ShiftRegister(size_t size)
    : size_(size), position_(0), data_(size) {}

template <typename T> void ShiftRegister<T>::push(T new_data) {
    ++position_;
    position_ %= size_;
    data_[position_] = new_data;
}

template <typename T> T ShiftRegister<T>::at(size_t index) const {
    static_assert(index < size_);
    size_t data_index = 0;
    if (index <= position_) {
        data_index = position_ - index;
    } else {
        data_index = position_ + size_ - index;
    }
    return data_[data_index];
}

} // namespace blocks
