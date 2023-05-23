void main() {
  int x = 345;
  float f = 3.14;
  int y = 1;

  print(x);
  {
    print(x);
    int x = 1;
    print(x);
  }
  print(x);
}