#ifndef CHECK_MEM_H
#define CHECK_MEM_H
/* This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
 */
void check_mem()
{
    static uint8_t* heapptr;
    static uint8_t* stackptr;
    stackptr = (uint8_t *)malloc(4);  // use stackptr temporarily
    heapptr = stackptr;               // save value of heap pointer
    free(stackptr);                   // free up the memory again (sets stackptr to 0)
    stackptr =  (uint8_t *)(SP);      // save value of stack pointer
    Serial.print(F("heap ")); Serial.println((uint16_t)heapptr - RAMSTART);
    Serial.print(F("stack ")); Serial.println((uint16_t)stackptr - RAMSTART);
}

#endif // CHECK_MEM_H
