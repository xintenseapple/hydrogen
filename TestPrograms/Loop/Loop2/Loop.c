int main() {
  unsigned int a = 0;
  unsigned int* i;
  for (i = &a; *i < 12; i++) {
    if (*i == 12) {
      (*i)++;
    }
  }
  return 11-(*i);
}
