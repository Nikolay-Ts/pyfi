clang-format --version
git ls-files '*.[ch]' '*.[ch]pp' '*.cc' '*.hh' '*.cxx' '*.hxx' \
| xargs -n50 -P4 clang-format -i --style=file
git diff --quiet  # should be clean