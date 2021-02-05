static constexpr int fan = 5;
static constexpr int peak = 2;

String input = "";

int value = 75;
int offset = 0;
bool increasing = true;

void setup()
{
    pinMode(fan, OUTPUT);
    analogWrite(fan, value);

    Serial.begin(9600);
}

void loop()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            value = input.toInt();
            input = "";
        }
        else
            input += c;
    }
    analogWrite(fan, value + offset);
    // if (increasing && offset == peak)
    //     increasing = false;
    // else if (!increasing && offset == -peak)
    //     increasing = true;
    // offset += increasing ? 1 : -1;
    // delay(50);
}