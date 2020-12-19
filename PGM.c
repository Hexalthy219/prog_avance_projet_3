#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>

#include "PGM.h"

PGM* createImageFromFile(const char* filename)
{
  FILE* file = fopen(filename, "r");
  if(!file)
    return NULL;

  // File encoding
  char magicNumber[3];
  if (fscanf(file, "%2s", magicNumber) != 1)
  {
    fclose(file);
    return NULL;
  }

  PGMType type;
  if (strcmp(magicNumber, "P2") == 0)
    type = ASCII;
  else if (strcmp(magicNumber, "P5") == 0)
    type = BINARY;
  else
  {
    fclose(file);
    return NULL;
  }

  // Skip comments
  char nextChar = fgetc(file);
  while (nextChar != EOF && !isdigit(nextChar))
    nextChar = fgetc(file);
  fseek(file, -1, SEEK_CUR);

  // read width
  size_t width = 0;
  if (fscanf(file, "%lu", &width) != 1)
  {
    fclose(file);
    return NULL;
  }

  // read height
  size_t height = 0;
  if (fscanf(file, "%lu", &height) != 1)
  {
    fclose(file);
    return NULL;
  }

  // read max value
  uint16_t maxValue = 0;
  if (fscanf(file, "%" SCNu16, &maxValue) != 1)
  {
    fclose(file);
    return NULL;
  }

  // create image
  PGM* res = createEmptyImage(width, height, maxValue);
  if (res == NULL)
  {
    fclose(file);
    return NULL;
  }
  res->type = type;

  // skip space in binary format
  if (res->type == BINARY)
    fgetc(file);

  // fill image
  for (size_t i = 0; i < res->height; ++i)
    for (size_t j = 0; j < res->width; ++j)
    {
      int value = -1;
      if (res->type == BINARY)
        value = fgetc(file);
      else
        fscanf(file, "%d", &value);

      if (value < 0)
      {
        freeImage(res);
        fclose(file);
        return NULL;
      }
      res->array[i][j] = (uint16_t)value;
    }

  fclose(file);
  return res;
}

int saveImageToFile(const PGM* image, const char* filename)
{
  if (image == NULL)
    return -1;

  FILE* file = fopen(filename, "w");
  if (file == NULL)
  {
    return -1;
  }

  fprintf(file, image->type == BINARY ? "P5\n" : "P2\n");
  fprintf(file, "%lu %lu\n", image->width, image->height);
  fprintf(file, "%u\n", image->maxValue);

  for (size_t i = 0; i < image->height; ++i)
  {
    for (size_t j = 0; j < image->width; ++j)
    {
      if (image->type == BINARY)
        image->maxValue > 256 ? fputc((uint16_t)image->array[i][j], file)
                              : fputc((uint8_t)image->array[i][j], file);
      else
        fprintf(file, "%u ", image->array[i][j]);
    }

    if (image->type == ASCII)
      fprintf(file, "\n");
  }

  fclose(file);
  return 0;
}

PGM* createEmptyImage(size_t width, size_t height, size_t numLevels)
{
  PGM* res = malloc(sizeof(PGM));
  if (res == NULL)
    return NULL;

  res->type = ASCII;
  res->width = width;
  res->height = height;
  res->maxValue = numLevels;

  res->array = malloc(height * sizeof(uint16_t*));
  if (res->array == NULL)
  {
    free(res);
    return NULL;
  }

  for (size_t i = 0; i < height; ++i)
  {
    res->array[i] = malloc(width * sizeof(uint16_t));
    if (res->array[i] == NULL)
    {
      for (size_t j = 0; j < i; ++j)
        free(res->array[j]);
      free(res->array);
      free(res);
      return NULL;
    }

    for (size_t j = 0; j < width; ++j)
      res->array[i][j] = 0;
  }

  return res;
}

void freeImage(PGM* image)
{
  if (image == NULL)
    return;
  for (size_t i = 0; i < image->height; ++i)
    free(image->array[i]);
  free(image->array);
  free(image);
  return;
}

