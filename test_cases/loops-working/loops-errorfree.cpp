void main() {
  int i;
  for (i = 0; i < 10; i = i + 1) {
    int b = 10;
  }

  int x = 0;

  while (x < 20) {
    x = x + 1;
  }

  x = 0;

  do {
    x = x + 1;
  } while (x < 20);

  switch (x) {
    case 1:
      x = 10;
      break;

    default:
      break;
  }
}