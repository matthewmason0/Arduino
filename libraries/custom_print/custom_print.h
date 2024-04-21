#ifndef CUSTOM_PRINT_H
#define CUSTOM_PRINT_H

void print() {}

template <typename T, typename... Types>
void print(T arg, Types... others)
{
    Serial.print(arg);
    print(others...);
}

void println() { Serial.println(); }

template <typename T, typename... Types>
void println(T arg, Types... others)
{
    Serial.print(arg);
    println(others...);
}

#endif // CUSTOM_PRINT_H
