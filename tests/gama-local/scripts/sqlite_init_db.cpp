#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

sqlite3    *database;
int         error = 0;

int read_sql_file(const char*);

int main(int argc, const char* argv[])
{
  std::string dbname = argv[1];
  int open = sqlite3_open_v2(dbname.c_str(), &database,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if (open != SQLITE_OK)
    {
      std::cout << "DB open error " << open << "\n";
      return 1;
    }

  for (int i=2; i<argc && !error; i++) read_sql_file(argv[i]);

  int close = sqlite3_close/*_v2*/(database);
  if (close != SQLITE_OK)
    {
      std::cout << "DB close error " << close << "\n";
      return 1;
    }

  return error;
}

int read_sql_file(const char* file_name)
{
  std::ifstream finp(file_name);
  if(!finp) return error=1;

  std::string sinp;

  finp.seekg(0, std::ios::end);
  sinp.resize(finp.tellg());
  finp.seekg(0, std::ios::beg);

  finp.unsetf(std::ios_base::skipws);
  sinp.assign(std::istream_iterator<char>(finp),
              std::istream_iterator<char>());

  const char* tail = 0;
  const char* sql  = sinp.c_str();
  const char* stop = sql + sinp.length();

  do
    {
      sqlite3_stmt *statement;
      int prepare = sqlite3_prepare_v2(database, sql, -1, &statement, &tail);
      if (prepare != SQLITE_OK) {
        std::cout << "DB prepare error " << prepare << "\n";
        return error=1;
      }

      int step;
      for (;;) {
        step = sqlite3_step(statement);
        if (step != SQLITE_ROW) break;

        // select processing ... sqlite3_column_int(statement, 0)
      }

      int final = sqlite3_finalize(statement);
      if (final != SQLITE_OK) {
        std::cout << "DB finalize error " << final << "\n";
        return error=1;
      }

      sql = tail;
    }
  while (tail && tail < stop);

  return 0;
}
