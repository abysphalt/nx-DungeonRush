#include "storage.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

void readScore(FILE* f, Score* score) {
  fscanf(f, "%d %d %d %d\n", &score->damage, &score->stand, &score->killed,
         &score->got);
  calcScore(score);
}
void writeScore(FILE* f, Score* score) {
  fprintf(f, "%d %d %d %d\n", score->damage, score->stand, score->killed,
          score->got);
}
void destroyRanklist(int n, Score** scores) {
  for (int i = 0; i < n; i++) {
    destroyScore(scores[i]);
  }
}
Score** insertScoreToRanklist(Score* score, int* n, Score** scores) {
  for (int i = 0; i < *n; i++)
    if (scores[i]->rank < score->rank) {
      if (*n < STORAGE_RANKLIST_NUM) {
        scores = realloc(scores, sizeof(Score*) * (++*n));
        scores[*n - 1] = createScore();
      }
      else destroyScore(scores[*n - 1]);
      for (int j = *n - 1; j > i ; j--) scores[j] = scores[j - 1];
      scores[i] = createScore();
      memcpy(scores[i], score, sizeof(Score));
      break;
    }
  return scores;
}

// Function to create a directory (recursively)
int createDirectory(const char *path) {
    return mkdir(path, 0777);  // 0777 sets read, write, and execute permissions for owner, group, and others
}

void writeRanklist(const char* path, int n, Score** scores) {
  // Check and see if the directory exists, if not make it.
  if (access(STORAGE_DIR, F_OK) != 0) {
    if (createDirectory(STORAGE_DIR) != 0) {
      #ifdef DEBUG
        TRACE("Error creating directory");
      #endif
      return;
    }
    #ifdef DEBUG
      TRACE("Directory created: %s\n", STORAGE_DIR);
    #endif
    }
  FILE* f = fopen(path, "w");
  if (f == NULL) {
    #ifdef DEBUG
      TRACE("writeRanklist: Can not create file\n");
    #endif
    return;
  } 
  fprintf(f, "%d\n", n);
  for (int i = 0; i < n; i++) writeScore(f, scores[i]);
  fclose(f);
}
Score** readRanklist(const char* path, int* n) {
  FILE* f = fopen(path, "r");
  if (!f) {
      *n = 1;
      Score** scores = malloc(sizeof(Score*) * (*n));
      scores[0] = createScore();
      memset(scores[0], 0, sizeof(Score));
      return scores;
  }
  fscanf(f, "%d", n);
  Score** scores = malloc(sizeof(Score*) * (*n));
  for (int i = 0; i < *n; i++) {
    scores[i] = createScore();
    readScore(f, scores[i]);
  }
  fclose(f);
  return scores;
}
void updateLocalRanklist(Score* score) {
  int n;
  Score** scores = readRanklist(STORAGE_PATH, &n);
  scores = insertScoreToRanklist(score, &n, scores);
  writeRanklist(STORAGE_PATH, n, scores);
}
