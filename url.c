#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define URL_LENGTH 6

static const char allLetters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

// Creates a random short URL
void generate_short_url(char *short_url) {
    for (int a = 0; a < URL_LENGTH; a++) {
        short_url[a] = allLetters[rand() % (sizeof(allLetters) - 1)];
    }
    short_url[URL_LENGTH] = '\0'; // end the string
}

// Add URL to the database and return the short URL
char *add_url_to_table(sqlite3 *db, const char *long_url) {
    char sql_query[200];
    char *short_url = malloc(URL_LENGTH + 1); // Allocate memory for the short URL
    if (short_url == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    generate_short_url(short_url);

    // Use parameterized query to avoid SQL injection
    const char *insert_sql = "INSERT INTO urltable (fullurl, shorturl) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        free(short_url);
        return NULL;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, long_url, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, short_url, -1, SQLITE_STATIC);

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        free(short_url);
        return NULL;
    }

    sqlite3_finalize(stmt);
    printf("URL added successfully\n");
    return short_url; // Return the dynamically allocated short URL
}

// Get the long URL from the database
char *get_long_url(sqlite3 *db, const char *short_url) {
    const char *select_sql = "SELECT fullurl FROM urltable WHERE shorturl = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Bind the short_url parameter
    sqlite3_bind_text(stmt, 1, short_url, -1, SQLITE_STATIC);

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char *long_url = (const char *)sqlite3_column_text(stmt, 0);
        char *result = strdup(long_url); // Allocate memory for the result
        sqlite3_finalize(stmt);
        return result;
    } else {
        fprintf(stderr, "No matching URL found for short URL: %s\n", short_url);
        sqlite3_finalize(stmt);
        return NULL;
    }
}

// Get the short URL from the database
char *get_short_url(sqlite3 *db, const char *long_url) {
    const char *select_sql = "SELECT shorturl FROM urltable WHERE fullurl = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Bind the long_url parameter
    sqlite3_bind_text(stmt, 1, long_url, -1, SQLITE_STATIC);

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char *short_url = (const char *)sqlite3_column_text(stmt, 0);
        char *result = strdup(short_url); // Allocate memory for the result
        sqlite3_finalize(stmt);
        return result;
    } else {
        fprintf(stderr, "No matching URL found for long URL: %s\n", long_url);
        sqlite3_finalize(stmt);
        return NULL;
    }
}

// Create database and tables
void database_init(sqlite3 **db) {
    int rc = sqlite3_open("urlSHORT.db", db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(*db));
        exit(1);
    } else {
        printf("Database opened successfully\n");
    }

    const char *drop_table_sql = "DROP TABLE IF EXISTS urltable;";
    const char *create_table_sql = "CREATE TABLE IF NOT EXISTS urltable ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "fullurl TEXT NOT NULL,"
                                   "shorturl TEXT NOT NULL);";

    rc = sqlite3_exec(*db, drop_table_sql, 0, 0, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error cleaning database: %s\n", sqlite3_errmsg(*db));
    } else {
        printf("Database cleaned\n");
    }

    rc = sqlite3_exec(*db, create_table_sql, 0, 0, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creating table: %s\n", sqlite3_errmsg(*db));
    } else {
        printf("Table created\n");
    }
}

int main() {
    sqlite3 *urlDatabase;
    database_init(&urlDatabase);

    char long_url[] = "testytesttest.com";
    char *short_url = add_url_to_table(urlDatabase, long_url);
    if (short_url == NULL) {
        fprintf(stderr, "Failed to add URL to table\n");
        sqlite3_close(urlDatabase);
        return 1;
    }

    char *urltestone = get_long_url(urlDatabase, short_url);
    char *urltesttwo = get_short_url(urlDatabase, long_url);

    if (urltestone && urltesttwo) {
        printf("Long URL is: %s\nShort URL is: %s\n", urltestone, urltesttwo);
    }

    // frees memory and closes database
    free(short_url);
    free(urltestone);
    free(urltesttwo);
    sqlite3_close(urlDatabase);
    return 0;
}
