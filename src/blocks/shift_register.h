#ifndef BLOCKS_SHIFT_REGISTER_H
#define BLOCKS_SHIFT_REGISTER_H
#include <vector>

namespace blocks {

template <typename T> class ShiftRegister {
  public:
    ShiftRegister(size_t size);
    void push(T new_data);
    T at(size_t index) const;

  private:
    size_t size_;
    size_t position_;
    std::vector<T> data_;
};

} // namespace blocks

#endif // BLOCKS_SHIFT_REGISTER_H
