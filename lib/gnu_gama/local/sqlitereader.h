/*
    GNU Gama C++ library
    Copyright (C) 2011  Vaclav Petras <vaclav.petras@fsv.cvut.cz>

    This file is part of the GNU Gama C++ library.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef  GNU_GAMA_LOCAL_SQLITE_READER

#ifndef SQLITEREADER_H
#define SQLITEREADER_H

#include <string>

#include "gnu_gama/exception.h"

/**
  \file sqlitereader.h
  \brief #GNU_gama::local::sqlite_db::SqliteReader header file.
  */

namespace GNU_gama {

    namespace Exception {

    /**
      \brief Exception class for #GNU_gama::local::sqlite_db::SqliteReader
      */
    class sqlitexc : public GNU_gama::Exception::string
    {
    public:
        /** \param message sqlite database error message or SqliteReader message */
        sqlitexc(const std::string& message)
            : string(message)
            { }

        /**
          Clones an exception.
          \internal
          The way as it is used in callback functions:
          \code
            // ... try block
            catch (GNU_gama::Exception::base& e)
                {
                    d->exception = e.clone();
                }
            return 1;
          \endcode
          */
        virtual sqlitexc* clone() const { return new sqlitexc(*this); }

        /**
          Rethrows an exception polymorphically.
          \internal
          The way as it is used in function \c exec in file sqlitereader.cpp:
          \code
            if (readerData->exception != 0)
                {
                    readerData->exception->raise();
                }
          \endcode
          */
        virtual void raise() const { throw *this; }
    };

    } // namespace Exception

namespace local {

    class LocalNetwork;

namespace sqlite_db {

struct ReaderData;

/**
  \brief Reads LocalNetwork from SQLite 3 database.
  */
class SqliteReader
{
public:
    /**
      \brief Opens a database connection.
      \param fileName name of database file
      */
    explicit SqliteReader(const std::string &fileName);

    /**
      \brief Closes a database connection.
      */
    ~SqliteReader();

    /** \brief Reads configuration \a configuration from database.

        If \a lnet is a \c NULL pointer new \c LocalNetwork is created.
        Type of network depends on algorithm fetched from database.

        \throws #GNU_gama::Exception::sqlitexc
      */
    void retrieve(LocalNetwork*& lnet, const std::string& configuration);

private:
    /** disabled copy constructor */
    SqliteReader(const SqliteReader&);
    /** disabled assignment operator */
    SqliteReader& operator= (const SqliteReader&);

    ReaderData *readerData; ///< pointer to private data
};

} // namespace sqlite_db
} // namespace local
} // namespace GNU_gama

#endif // SQLITEREADER_H
#endif // GNU_GAMA_LOCAL_SQLITE_READER
