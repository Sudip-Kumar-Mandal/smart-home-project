const int sevenSegmentPins[7] = {26, 27, 23, 22, 21, 33, 25};

int numbers[11][7] = {
    {1,1,1,1,1,1,0},  //0
    {0,1,1,0,0,0,0},  //1
    {1,1,0,1,1,0,1},  //2
    {1,1,1,1,0,0,1},  //3
    {0,1,1,0,0,1,1},  //4
    {1,0,1,1,0,1,1},  //5
    {1,0,1,1,1,1,1},  //6
    {1,1,1,0,0,0,0},  //7
    {1,1,1,1,1,1,1},  //8
    {1,1,1,1,0,1,1},  //9
    {0,0,0,0,0,0,0}   //- off
};

void setup_display() {
    for(int i = 0; i < 7; i++) {
        pinMode(sevenSegmentPins[i], OUTPUT);
    }
}

void sevenSegmentDisplay(int num) {
  for(int i=0; i<7; i++) {
    digitalWrite(sevenSegmentPins[i], numbers[num][i]);
  }
}