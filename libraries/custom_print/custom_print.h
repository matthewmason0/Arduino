#ifndef CUSTOM_PRINT_H
#define CUSTOM_PRINT_H

void print() {}

template <typename T, typename... Types>
void print(T arg, Types... others)
{
#ifndef DISABLE_PRINT
    Serial.print(arg);
    print(others...);
#endif
}

void println() { Serial.println(); }

template <typename T, typename... Types>
void println(T arg, Types... others)
{
#ifndef DISABLE_PRINT
    Serial.print(arg);
    println(others...);
#endif
}

#endif // CUSTOM_PRINT_H
