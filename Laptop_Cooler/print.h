#ifndef PRINT_H
#define PRINT_H

// template<typename T, typename... Types>
// void print(T arg, Types... others) {
//   Serial.print(arg);
//   print(others...);
// }

void print() {}

template <typename T, typename... Types>
void print(T arg, Types... others)
{
  Serial.print(arg);
  print(others...);
}

#endif