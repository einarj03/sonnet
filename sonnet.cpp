#include <iostream>
#include <cstring>
#include <cctype>
#include <cassert>
#include <map>
#include <string>
#include <fstream>

using namespace std;

#include "sonnet.h"

/* PRE-SUPPLIED HELPER FUNCTIONS START HERE */

/* NOTE: It is much more important to understand the interface to and
   the "black-box" operation of these functions (in terms of inputs and
   outputs) than it is to understand the details of their inner working. */

/* get_word(...) retrieves a word from the input string input_line
   based on its word number. If the word number is valid, the function
   places an uppercase version of the word in the output parameter
   output_word, and the function returns true. Otherwise the function
   returns false. */

bool get_word(const char *input_line, int word_number, char *output_word) {
  char *output_start = output_word;
  int words = 0;

  if (word_number < 1) {
    *output_word = '\0';
    return false;
  }
  
  do {
    while (*input_line && !isalnum(*input_line))
      input_line++;

    if (*input_line == '\0')
      break;

    output_word = output_start;
    while (*input_line && (isalnum(*input_line) || *input_line=='\'')) {
      *output_word = toupper(*input_line);
      output_word++;
      input_line++;
    }
    *output_word = '\0';

    if (++words == word_number)
      return true;

  } while (*input_line);

  *output_start = '\0';
  return false;
}

/* char rhyming_letter(const char *ending) generates the rhyme scheme
   letter (starting with 'a') that corresponds to a given line ending
   (specified as the parameter). The function remembers its state
   between calls using an internal lookup table, such that subsequents
   calls with different endings will generate new letters.  The state
   can be reset (e.g. to start issuing rhyme scheme letters for a new
   poem) by calling rhyming_letter(RESET). */

char rhyming_letter(const char *ending) {

  // the next rhyming letter to be issued (persists between calls)
  static char next = 'a';
  // the table which maps endings to letters (persists between calls)
  static map<string, char> lookup;

  // providing a way to reset the table between poems
  if (ending == RESET) {
    lookup.clear();
    next = 'a';
    return '\0';
  }

  string end(ending);

  // if the ending doesn't exist, add it, and issue a new letter
  if (lookup.count(end) == 0) {
    lookup[end]=next;
    assert(next <= 'z');
    return next++;
  }

  // otherwise return the letter corresponding to the existing ending
  return lookup[end];
}

/* START WRITING YOUR FUNCTION BODIES HERE */
int count_words(const char* line) {
  int count = 0;
  char last_word[MAX_LENGTH];

  while (get_word(line, count+1, last_word))
    ++count;
  
  return count;
}

bool find_phonetic_ending(const char* word, char* phonetic_ending) {
  ifstream in;
  in.open("dictionary.txt");
  char dict_word[MAX_LENGTH];
  char phonetic[MAX_LENGTH];
  bool found = false;

  while(!found && in >> ws >> dict_word) {
    if (strcmp(dict_word, word) == 0)
      found = true;
    
    in.getline(phonetic, MAX_LENGTH);
  }
  in.close();

  if (found) {
    int word_count = count_words(phonetic);
    int vowel_pos = 1;
    char next_phoneme[MAX_LENGTH];
    for (int i = 1; i <= word_count; ++i) {
      get_word(phonetic, i, next_phoneme);
      if (word_contains_vowel(next_phoneme))
        vowel_pos = i;
    }

    strcpy(phonetic_ending, "");
    for (int i = vowel_pos; i <= count_words(phonetic); ++i) {
      get_word(phonetic, i, next_phoneme);
      strcat(phonetic_ending, next_phoneme);
    }
    
    return true;
  }

  return false;
}

bool word_contains_vowel(const char* word) {
  int len = strlen(word);
  for (int i = 0; i < len; ++i) {
    if (word[i] == 'A' || word[i] == 'E' || word[i] == 'I' || word[i] == 'O' || word[i] == 'U')
      return true;
  }

  return false;
}

bool find_rhyme_scheme(const char* filename, char* scheme) {
  ifstream in;
  in.open(filename);

  if (in.fail())
    return false;

  char line[MAX_LENGTH];
  char last_word[MAX_LENGTH];
  char ending[MAX_LENGTH];
  int word_count;
  int count = 0;
  rhyming_letter(RESET);
  strcpy(scheme, "");

  in.getline(line, MAX_LENGTH);
  while (in.good()) {
    word_count = count_words(line);
    get_word(line, word_count, last_word);
    find_phonetic_ending(last_word, ending);

    scheme[count] = rhyming_letter(ending);
    ++count;

    in.getline(line, MAX_LENGTH);
  }
  in.close();
  
  return true;
}

const char* identify_sonnet(const char* filename) {
  char scheme[MAX_LENGTH];

  if (find_rhyme_scheme(filename, scheme)) {
    if (strcmp(scheme, "ababcdcdefefgg") == 0)
      return "Shakespearean";

    if (strcmp(scheme, "abbaabbacdcdcd") == 0)
      return "Petrarchan";

    if (strcmp(scheme, "ababbcbccdcdee") == 0)
      return "Spenserian";
  }

  return "Unknown";
}