void main() {
  int x = 345;
  int z;

  print(x);
  {
    print(x);
    int x = 1;
    print(x);
  }
  print(x);
}