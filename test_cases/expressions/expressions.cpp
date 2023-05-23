void main() {
  int x = 345;

  print(x);
  {
    print(x);
    int x = 1;
    print(x);
  }
  print(x);
}