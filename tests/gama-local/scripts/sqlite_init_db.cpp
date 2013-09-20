#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

sqlite3    *dbHandle;
const char *pzTail;
int         error = 0;

void read_sql_file(const char*);

int main(int argc, const char* argv[])
{
  std::string dbname = argv[1];

  int resOpen = sqlite3_open_v2(dbname.c_str(), &dbHandle,
                                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                0);
  if (resOpen != SQLITE_OK)
    {
      std::cout << "DB open error " << resOpen << "\n";
      return 1;
    }

  for (int i=2; i<argc && !error; i++) read_sql_file(argv[i]);

  int resClose = sqlite3_close/*_v2*/(dbHandle);
  if (resClose != SQLITE_OK)
    {
      std::cout << "DB close error " << resClose << "\n";
      return 1;
    }

  return error;
}

void read_sql_file(const char* file_name)
{
  //std::cout << "reading file " << file_name << "\n";
  std::ifstream file(file_name);

  while(file)
    {
      char c;
      bool comment = false, empty = true;;
      std::string statement;
      while (file.get(c))
        {
          if (c == '/' && file.peek() == '*') {
            comment = true;  file.get(c); continue;
          }
          if (c == '*' && file.peek() == '/') {
            comment = false; file.get(c); continue;
          }
          if (!comment && c == ';') break;

          if (!comment) {
            statement += c;
            if (!std::isspace(c)) empty = false;
          }
        }
      if (empty) continue;

      sqlite3_stmt *ppStmt;
      int resPrepare = sqlite3_prepare_v2(
             dbHandle,           /* Database handle */
             statement.c_str(),  /* SQL statement, UTF-8 encoded */
             statement.length(), /* Maximum length of zSql in bytes. */
             &ppStmt,            /* OUT: Statement handle */
             &pzTail);           /* OUT: Pointer to unused portion of zSql */
      if (resPrepare != SQLITE_OK)
        {
          std::cout << "DB prepare error " << resPrepare << "\n";
          error = 1;
          return;
        }

      int resStep;
      do  
        {
          resStep = sqlite3_step(ppStmt);
        }
      while (resStep == SQLITE_ROW);

      int resFinal = sqlite3_finalize(ppStmt);
      if (resFinal != SQLITE_OK)
        {
          std::cout << "DB finalize error " << resFinal << "\n";
          error = 1;
          return;
        }
    }
}
