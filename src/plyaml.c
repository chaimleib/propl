#include "plist/plist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int plist2yaml(plist_t root);

int plistArray2yaml(plist_t array) {
  uint32_t arraySize = plist_array_get_size(array);
  printf("!!array<%u>\n", arraySize);
  uint32_t i=0;
  plist_t val = NULL;
  for (i=0; i<arraySize; i++) {
    printf("%u:", i);
    val = plist_array_get_item(array, i);
    plist2yaml(val);
  }
  return 0;
}

int plistDict2yaml(plist_t dict) {
  uint32_t dictSize = plist_dict_get_size(dict);
  printf("!!dict<%u>\n", dictSize);
  plist_dict_iter iter = NULL;
  plist_dict_new_iter(dict, &iter);
  char *key = NULL;
  plist_t val = NULL;
  uint32_t i = 0;
  for (i=0; i<dictSize; i++) {
    printf("%u:", i);
    plist_dict_next_item(dict, iter, &key, &val);
    printf("'%s'", key);
    free(key);
    plist2yaml(val);
  }
  return 0;
}

int plistBoolean2yaml(plist_t b) {
  printf("!!bool");
  uint8_t val = 0;
  plist_get_bool_val(b, &val);
  if (val) {
    printf("<TRUE>\n");
  } else {
    printf("<FALSE>\n");
  }
  return 0;
}

int plistString2yaml(plist_t str) {
  printf("!!string");
  char *val = NULL;
  plist_get_string_val(str, &val);
  if (val) {
    printf("<%s>\n", val);
    free(val);
  } else {
    printf("<>\n");
  }
  return 0;
}

int plistUint2yaml(plist_t u) {
  uint64_t val = 0;
  plist_get_uint_val(u, &val);
  printf("!!uint<%llu>\n", val);
  return 0;
}

int plistReal2yaml(plist_t real) {
  double val = 0.0;
  plist_get_real_val(real, &val);
  printf("!!real<%f>\n", val);
  return 0;
}

int plistData2yaml(plist_t data) {
  char *val = NULL;
  uint64_t dataSize = 0;
  plist_get_data_val(data, &val, &dataSize);
  printf("!!data<%llu>\n", dataSize);
  free(val);
  return 0;
}

int plist2yaml(plist_t root) {
  plist_type nodeType = plist_get_node_type(root);
  switch (nodeType) {
    case PLIST_BOOLEAN:
      return plistBoolean2yaml(root);
    case PLIST_UINT:
      return plistUint2yaml(root);
    case PLIST_REAL:
      return plistReal2yaml(root);
    case PLIST_STRING:
      return plistString2yaml(root);
    case PLIST_ARRAY:
      return plistArray2yaml(root);
    case PLIST_DICT:
      return plistDict2yaml(root);
    case PLIST_DATE:
      printf("date\n");
      return 0;
    case PLIST_DATA:
      return plistData2yaml(root);
    case PLIST_KEY:
      printf("key\n");
      return 0;
    case PLIST_UID:
      printf("uid\n");
      return 0;
    case PLIST_NONE:
      printf("no type\n");
      return 0;
    default:
      printf("UNKNOWN\n");
      return 1;
  }
}


int main(int argc, char *argv[]) {
  FILE *plistIn = NULL;
  plist_t root = 0;
  char *fpathIn;
  int sizeIn = 0;
  struct stat *filestats = (struct stat *) malloc(sizeof(struct stat));
  char * bufIn = 0;
  
  if (argc != 2) {
    printf("No input file provided\n");
    return 1;
  }
  
  fpathIn = argv[1];
  plistIn = fopen(fpathIn, "rb");
  
  if (!plistIn) {
    printf("Could not open file\n");
    return 2;
  }
  
  stat(fpathIn, filestats);
  sizeIn = filestats->st_size;
  bufIn = (char *) malloc(sizeof(char) * (sizeIn + 1));
  fread(bufIn, sizeof(char), sizeIn, plistIn);
  fclose(plistIn);
  
  plist_from_bin(bufIn, sizeIn, &root);
  if (!root) {
    printf("Could not read binary plist\n");
    return 3;
  }
  
  return plist2yaml(root);
}
