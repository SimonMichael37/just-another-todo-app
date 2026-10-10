#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LINE 1024
#define MAX_FIELDS 2

void print_line(int line_number, char *line) {
  printf("%d. ", line_number);

  char *columns[MAX_FIELDS];
  int col_count = 0;

  char *token = strtok(line, ",");
  while (token != NULL && col_count < MAX_FIELDS) {
    columns[col_count] = token;
    col_count++;

    token = strtok(NULL, ",");
  }

  printf("%s ", columns[0]);

  bool completed = atoi( columns[1] );
  if (completed)
    printf("[x]");
  else
    printf("[ ]");

  printf("\n");
}

void todo_list(void) {
  FILE *file = fopen("todo.csv", "r");
  if (!file) {
    perror("Failed to open file");
    return;
  }

  char line[1024];
  int index = 1;

  while ( fgets(line, sizeof(line), file) ) {
    line[strcspn(line, "\r\n")] = '\0';

    print_line(index, line);
    index++;
  }
}

void todo_delete(int target_line) {
  FILE *file = fopen("todo.csv", "r");
  FILE *tmp_file = fopen("todo.csv.tmp", "w");

  char buffer[1024];
  int current_line = 1;
  bool found = false;

  while ( fgets(buffer, sizeof(buffer), file) ) {
    if (current_line == target_line) {
      found = true;
    } else {
      fputs(buffer, tmp_file);
    }

    current_line++;
  }

  fclose(file);
  fclose(tmp_file);

  if (!found) {
    printf("Line No %d not found in file.\n", target_line);
    remove("todo.csv.tmp");
  }
  else {
    printf("Line No %d deleted.\n", target_line);
    remove("todo.csv");
    rename("todo.csv.tmp", "todo.csv");
  }
}

void todo_clear(void) {
  FILE *file = fopen("todo.csv", "r");
  FILE *tmp_file = fopen("todo.csv.tmp", "w");

  char line[1024];
  char tmp_line[1024];
  int current_line = 1;
  int lines_deleted = 0;

  while ( fgets(line, sizeof(line), file) ) {
    for (int i = 0; i < 1024; i++) {
      tmp_line[i] = line[i];
    }

    tmp_line[ strcspn(tmp_line, "\r\n") ] = '\0';

    char *columns[MAX_FIELDS];
    int col_count = 0;

    char *token = strtok(tmp_line, ",");
    while (token != NULL && col_count < MAX_FIELDS) {
      columns[col_count] = token;
      col_count++;

      token = strtok(NULL, ",");
    }

    bool completed = atoi( columns[1] );

    if (completed) {
      lines_deleted++;
    } else {
      fputs(line, tmp_file);
    }

    current_line++;
  }

  fclose(file);
  fclose(tmp_file);

  if (lines_deleted <= 0) {
    printf("Zero tasks deleted\n");
    remove("todo.csv.tmp");
  } else {
    printf("%d tasks deleted\n", lines_deleted);
    remove("todo.csv");
    rename("todo.csv.tmp", "todo.csv");
  }
}

void todo_mark(int target_line) {
  FILE *file = fopen("todo.csv", "r");
  FILE *tmp_file = fopen("todo.csv.tmp", "w");

  char line[1024];
  char tmp_line[1024];
  int current_line = 1;
  bool found = false;

  while ( fgets(line, sizeof(line), file) ) {
    for (int i = 0; i < 1024; i++) {
      tmp_line[i] = line[i];
    }

    if (current_line == target_line) {
      tmp_line[ strcspn(tmp_line, "\r\n") ] = '\0';

      char *columns[MAX_FIELDS];
      int col_count = 0;

      char *token = strtok(tmp_line, ",");
      while (token != NULL && col_count < MAX_FIELDS) {
        columns[col_count] = token;
        col_count++;

        token = strtok(NULL, ",");
      }

      strcpy(columns[1], "1");

      found = true;

      fputs(columns[0], tmp_file);
      fputc(',', tmp_file);
      fputs(columns[1], tmp_file);
      fputc('\n', tmp_file);
    } else {
      fputs(line, tmp_file);
    }

    current_line++;
  }

  fclose(file);
  fclose(tmp_file);

  if (!found) {
    printf("Task not found at Line No %d \n", target_line);

    remove("todo.csv.tmp");
  } else {
    printf("Task at Line No %d marked as completed\n", target_line);

    remove("todo.csv");
    rename("todo.csv.tmp", "todo.csv");
  }
}

void todo_add(char *task) {
  task[strcspn(task, "\r\n")] = '\0';

  FILE *file = fopen("todo.csv", "a");
  if (!file) {
    perror("Error opening file in append mode");
    return;
  }

  fprintf(file, "%s,0\n", task);

  fclose(file);
}

void print_help(void) {
  printf(" --- Available Commands --- \n");
  printf("1) list - Lists tasks.\n");
  printf("2) clear - Lists tasks.\n");
  printf("3) delete [Line No] - Lists tasks.\n");
  printf("4) mark [Line No] - Lists tasks.\n");
  printf("5) add \"[Task]\" - Lists tasks.\n");
  printf("6) help - Display available commands\n");
}

int main(int argc, char** argv) {
  if (argc < 2) {
    print_help();
    return 0;
  }

  if (strncmp(argv[1], "list", 8) == 0) {
    todo_list();
  } else if (strncmp(argv[1], "clear", 8) == 0) {
    todo_clear();
  } else if (strncmp(argv[1], "delete", 8) == 0) {
    if (argc < 3) {
      printf("Please also include line no\n");
    } else {
      int line_no = atoi(argv[2]);
      todo_delete(line_no);
    }
  } else if (strncmp(argv[1], "mark", 8) == 0) {
    if (argc < 3) {
      printf("Please also include line no\n");
    } else {
      int line_no = atoi(argv[2]);
      todo_mark(line_no);
    }
  } else if (strncmp(argv[1], "add", 8) == 0) {
    if (argc < 3) {
      printf("Please also include task\n");
    } else {
      todo_add(argv[2]);
    }
  } else if (strncmp(argv[1], "help", 8) == 0) {
    print_help();
  } else {
    printf("Invalid command. Use the 'help' command to see the available commands\n");
  } 

  return 0;
}
