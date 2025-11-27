// clang-format off
// https://en.cppreference.com/w/cpp/coroutine/generator.html
#include <generator>
#include <iostream>

template<typename T>
struct Tree
{
  T value;
  Tree *left{}, *right{};

  std::generator<const T&> traverse_inorder() const
  {
    if (left)
      co_yield std::ranges::elements_of(left->traverse_inorder());

    co_yield value;

    if (right)
      co_yield std::ranges::elements_of(right->traverse_inorder());
  }
};

int main()
{
  Tree<char> tree[]
      {
          {'D', tree + 1, tree + 2},
          //                            │
          //            ┌───────────────┴────────────────┐
          //            │                                │
          {'B', tree + 3, tree + 4},       {'F', tree + 5, tree + 6},
          //            │                                │
          //  ┌─────────┴─────────────┐      ┌───────────┴─────────────┐
          //  │                       │      │                         │
          {'A'},                  {'C'}, {'E'},                    {'G'}
      };

  for (char x : tree->traverse_inorder())
    std::cout << x << ' ';
  std::cout << '\n';
}
