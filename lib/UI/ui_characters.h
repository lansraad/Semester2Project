// Define custom characters as 5x8 matrices
const unsigned char up_arrow_empty[] = {
  0b00000,
  0b00100,
  0b01010,
  0b01010,
  0b10001,
  0b10001,
  0b11111,
  0b00000
};

const unsigned char up_arrow_full[] = {
  0b00000,
  0b00100,
  0b01110,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b00000
};

const unsigned char down_arrow_empty[] = {
  0b00000,
  0b11111,
  0b10001,
  0b10001,
  0b01010,
  0b01010,
  0b00100,
  0b00000
};

const unsigned char down_arrow_full[] = {
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b01110,
  0b00100,
  0b00000
};