int main() {
  unsigned int i;
  for (i = 0; i < 13; i++) {
    if (i == 12) {
      i++;
    }
  }
  return 11-i;
}
