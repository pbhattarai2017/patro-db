#define _XOPEN_SOURCE
#include <locale.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <errno.h>

int die(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void make_calendar(int mitiYear, int mitiMonth, int mitiDay, int totalDays,
                   int firstWeekDay);

int main(int argc, char *argv[]) {
  MYSQL *connection;
  MYSQL_RES *result;
  MYSQL_ROW row;
  int dateBufferSize = 12;
  int queryStringSize = 128;
  char dateBuffer[dateBufferSize];
  char queryString[queryStringSize];
  char mitiBufferSize = 12;
  char mitiBuffer[mitiBufferSize];
  int mitiYear, mitiMonth, totalDays, mitiDay;
  int totalColumns;
  char numberBuffer[8];
  unsigned long *field_lengths;

  // use strptime(3) to parse time
  connection = mysql_init(NULL);
  connection != NULL || die("failed to initialize the connection");

  mysql_real_connect(connection, "localhost", "prakash", "secretkey", "miti",
                     3306, NULL, 0) == connection ||
      die("failed to connect to the database");
  // printf("argc = %d", argc);
  errno = 22;
  if (argc == 2 || argc > 3) {
    die("Syntax: miti <year> <month>");
  } else if (argc == 3) {
    mitiYear = atoi(argv[1]);
    mitiMonth = atoi(argv[2]);
    if (mitiYear < 1975 || mitiYear > 2100) {
      die("Invalid <year> argument\nValid range: 1975 BS - 2100 BS");
    }
    if (mitiMonth < 1 || mitiMonth > 12) {
      die("Invalid <month> argument");
    }
    snprintf(mitiBuffer, mitiBufferSize, "%d/%d/%d", mitiYear, mitiMonth, 1);
  } else {
    /**
     * @brief use current date
     *
     */
    errno = 0;
    time_t T = time(NULL);
    struct tm currentDate = *localtime(&T);
    snprintf(dateBuffer, dateBufferSize, "%d-%d-%d", currentDate.tm_year + 1900,
             currentDate.tm_mon + 1, currentDate.tm_mday);
    snprintf(queryString, queryStringSize,
             "select miti from miti where date = '%s'", dateBuffer);

    mysql_query(connection, queryString) == 0 ||
        die("error occured while executing query");

    result = mysql_use_result(connection);
    result != NULL || die("error occured while fetchin result");

    /**
     * @brief fetch today's miti from database
     *
     */
    totalColumns = mysql_num_fields(result);
    mitiBuffer[0] = '\0';
    while (row = mysql_fetch_row(result)) {
      field_lengths = mysql_fetch_lengths(result);
      for (int i = 0; i < totalColumns; i++) {
        strncpy(mitiBuffer, row[i] ? row[i] : "NULL", (int)field_lengths[i]);
        mitiBuffer[(int)field_lengths[i]] = '\0';
        // printf("MITI:%s\n", mitiBuffer);
      }
    }
  }
  if (mitiBuffer[0] != '\0') {
    sscanf(mitiBuffer, "%d/%d/%d", &mitiYear, &mitiMonth, &mitiDay);
    // printf("%d,%d,%d\n", mitiYear, mitiMonth, mitiDay);
    snprintf(
        queryString, queryStringSize,
        "select days, start_date from summary where (year, month) = (%d, %d)",
        mitiYear, mitiMonth);
    mysql_query(connection, queryString) == 0 ||
        die("error occured while executing query");
    result = mysql_use_result(connection);
    result != NULL || die("error occured while fetching result");
    /**
     * fetch total # of days & start_date of miti
     */
    // totalColumns = mysql_num_fields(result);
    while (row = mysql_fetch_row(result)) {
      field_lengths = mysql_fetch_lengths(result);
      // printf("exec...");
      strncpy(numberBuffer, row[0] ? row[0] : "NULL", (int)field_lengths[0]);
      numberBuffer[(int)field_lengths[0]] = '\0';
      strncpy(dateBuffer, row[1] ? row[1] : "NULL", (int)field_lengths[1]);
      dateBuffer[(int)field_lengths[1]] = '\0';
    }
    totalDays = atoi(numberBuffer);
    struct tm start_date;
    memset(&start_date, 0, sizeof(struct tm));
    strptime(dateBuffer, "%Y-%m-%d", &start_date) != NULL ||
        die("error parsing start_date");
    // printf("WEEK #: %d\n", start_date.tm_wday);
    // printf("totalDays:%d\n", totalDays);
    // printf("start_date: %s\n", dateBuffer);
    if (totalDays >= 28 && totalDays <= 32) {
      make_calendar(mitiYear, mitiMonth, mitiDay, totalDays,
                    start_date.tm_wday + 1);
    } else {
      errno = 33;
      die("total number of days in a month is invalid");
    }
  } else {
    errno = 121;
    die("miti couldn't be fetched from database");
  }
  mysql_free_result(result);
  mysql_close(connection);
  return 0;
}

void make_calendar(int mitiYear, int mitiMonth, int mitiDay, int totalDays,
                   int firstWeekDay) {
  char headline[20];
  char *weekDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  char *months[] = {"Baishakh", "Jetha",   "Asar",  "Saun", "Bhadau", "Asoj",
                    "Kartik",   "Mangsir", "Pusha", "Magh", "Fagun",  "Chaita"};

  // print headline
  printf("           %d %s\n", mitiYear, months[mitiMonth - 1]);
  printf("---------------------------------\n");
  // print weekdays
  // printf(" ");
  for (int i = 0; i < 7; i++) {
    if (i == 6) {
      printf("\033[1;31m");
    }
    printf("%s  ", weekDays[i]);
    printf("\033[0m");
  }
  printf("\n");

  // print first week (partial)
  int currentDay = 1;
  // adding 35 (multiples of 7) is equivalent to itself (mod 7)
  // int firstWeekDay = ((todayWeekDay + 35) - (mitiDay - 1)) % 7;
  for (int i = 0; i < 7; i++) {
    if (i + 1 >= firstWeekDay) {
      if (i == 6) {
        // red color in saturday
        printf("\033[1;31m");
      }
	  if (currentDay == mitiDay) {
        // yellow color today
        printf("\033[1;43m");
      }
      if (currentDay < 10) {
        printf(" ");
      }
      printf("%d", currentDay++);
      printf("\033[0m");
	  printf("   ");
    } else {
      printf("     ");
    }
  }
  printf("\n");
  // write remaining weeks
  for (int w = 1; w <= 5; w++) {
    for (int i = 0; i < 7; i++) {
      if (currentDay > totalDays) {
        if (i > 0) {
          printf("\n");
        }
        return;
      }
      if (i == 6) {
        // red color in saturday
        printf("\033[1;31m");
      }
	  if (currentDay == mitiDay) {
        // yellow color today
        printf("\033[1;43m");
      }
      if (currentDay < 10) {
        printf(" ");
      }
      printf("%d", currentDay++);
      printf("\033[0m");
	  printf("   ");
    }
    printf("\n");
  }
}
