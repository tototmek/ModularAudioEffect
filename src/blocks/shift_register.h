#ifndef BLOCKS_SHIFT_REGISTER_H
#define BLOCKS_SHIFT_REGISTER_H
#include <cassert>
#include <vector>

namespace blocks {

template <typename T> class ShiftRegister {
  public:
    ShiftRegister(size_t size) : size_(size), position_(0), data_(size) {}

    void push(T new_data) {
        ++position_;
        position_ %= size_;
        data_[position_] = new_data;
    }

    T at(size_t index) const {
        assert(index < size_);
        size_t data_index = 0;
        if (index <= position_) {
            data_index = position_ - index;
        } else {
            data_index = position_ + size_ - index;
        }
        return data_[data_index];
    }

  private:
    size_t size_;
    size_t position_;
    std::vector<T> data_;
};

} // namespace blocks

#endif // BLOCKS_SHIFT_REGISTER_H
