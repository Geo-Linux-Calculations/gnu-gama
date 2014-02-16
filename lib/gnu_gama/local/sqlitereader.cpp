/*
    GNU Gama C++ library
    Copyright (C) 2011  Vaclav Petras <vaclav.petras@fsv.cvut.cz>
                  2013, 2014  Ales Cepek <cepek@gnu.org>

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

#ifdef   GNU_GAMA_LOCAL_SQLITE_READER

#include <gnu_gama/local/sqlitereader.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/xml/dataobject.h>
#include <gnu_gama/intfloat.h>

#include <string>
#include <sstream>

#include <sqlite3.h>

/**
  \internal
  \file sqlitereader.cpp
  \brief Implementation of #GNU_gama::local::sqlite_db::SqliteReader.
  */

namespace {
  const char* T_gamalite_database_not_open =
    "database not open"; ///< error message, used in #GNU_gama::local::sqlite_db::SqliteReader::SqliteReader
  const char* T_gamalite_invalid_column_value =
    "invalid column value"; ///< error message, used in callbacks' to indicate bad value of database field
  const char* T_gamalite_conversion_to_double_failed =
    "conversion to double failed"; ///< error message, used in conversion function when no better message can be used
  const char* T_gamalite_conversion_to_integer_failed =
    "conversion to integer failed"; ///< \copydoc T_gamalite_conversion_to_double_failed()
  const char* T_gamalite_unknown_exception_in_callback =
    "unknown exception in SqliteReader's callback"; ///< error message, used in callbacks' catch(...)
  const char* T_gamalite_stand_point_cluster_with_multi_dir_sets =
    "StandPoint cluster with multiple directions sets"; ///< error message, used in #sqlite_db_readObservations
  const char* T_gamalite_configuration_not_found =
    "configuration not found"; ///< error message, used in #GNU_gama::local::sqlite_db::SqliteReader::retrieve
}

extern "C" {
    /**
      \brief Reads configuration information from table \c gnu_gama_local_configurations.

      Reads from table means: parameter \a argv points to values selected from that table.

      \param data pointer to \c struct #GNU_gama::local::sqlite_db::ReaderData
      \param argc number of strings in \a argv
      \param argv strings with column values (\c NULL pointer means \c NULL value)

      Last parameter is unused.

      Callback can not throw any exception because it interfaces with C library.
      If callback threw an exception, program would crash.
      Program compiled by GCC writes this message:
      \verbatim
      terminate called after throwing an instance of 'GNU_gama::Exception::string'
        what():  from callback
      \endverbatim
      Note that \c what() message is available only if exception is derived from \c std::exception.
      Callbacks should have it's body enclosed by try-catch block.
      \code
    int readSomething(void* data, int argc, char** argv, char**) {
            ReaderData* d = static_cast<ReaderData*>(data);
            try {
                    // ... callback's code
                    return 0;
                }
            catch (GNU_gama::Exception::base& e) { d->exception = e.clone(); }
            catch (std::exception& e) { d->exception = new GNU_gama::Exception::string(e.what()); }
            catch (...) { d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback); }
            return 1;
        }
      \endcode

      We can be quite sure that callback's corresponding statement is ok. But we can not control the field values.
      SQLite database doesn't enforce the field type declared in CREATE statement.
      And there is no guarantee that database we are reading from is created with the GNU Gama official database schema.
      The conclusion is the fields have to checked in callback.
      The critical test of \c NULL values is done in the beginning (we can also check the expected number of fields).
      Note that only some fields could be supposed to have non-\c NULL values.
      \code
      try {
             if (argc == 7 && argv[0] && argv[1]) {
                   // it is sure that argv[0-6] is ok
                   // and std::string s(argv[0-1]) doesn't throw exception
                   // ... callback's code
                   return 0;
                } else {
                   throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
                }
          }
      \endcode

      \note If \c std::string \c const \c char \c * ctor parameter is \c NULL pointer,
      std::logic_error is thrown.
      With GCC the \c what() message is <tt>"basic_string::_S_construct NULL not valid"</tt>.

      \sa #SqliteReaderCallbackType
      */
    int sqlite_db_readConfigurationInfo(void* data, int argc, char** argv, char**);

    /**
      \brief Reads configuration description from table \c gnu_gama_local_descriptions.
      #GNU_gama::local::sqlite_db::ReaderData::configurationId has to be set before calling this function.

     \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readConfigurationText(void* data, int argc, char** argv, char**);

    /**
      \brief Reads points from table \c gnu_gama_local_points.


      \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readPoints           (void* data, int argc, char** argv, char**);

    /**
      \brief Reads clusters from table \c gnu_gama_local_clusters.

      \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readClusters         (void* data, int argc, char** argv, char**);

    /**
      \brief Reads observations from table \c gnu_gama_local_obs.

      \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readObservations     (void* data, int argc, char** argv, char**);

    /**
      \brief Reads vectors from table \c gnu_gama_local_vectors.

      \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readVectors          (void* data, int argc, char** argv, char**);

    /**
      \brief Reads HeightDifferences from table \c gnu_gama_local_vectors.

      \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readCoordinates      (void* data, int argc, char** argv, char**);

    /**
      \brief Reads vectors from table \c gnu_gama_local_vectors.

      \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readHeightDifferences(void* data, int argc, char** argv, char**);

    /**
      \brief Reads covariance matrix from table \c gnu_gama_local_covmat.

      \sa sqlite_db_readConfigurationInfo()
      */
    int sqlite_db_readCovarianceMatrix (void* data, int argc, char** argv, char**);

    /**
        \brief The type for a callback function of function \c sqlite3_exec.

        Header file \c <sqlite3.h> contains similar typedef but it is deprecated.
        It has to by declared in <tt>extern "C"</tt> block because it is pointer to function with C linkage.
        Unfortunately if functions is declared in <tt>extern "C"</tt> block,
        unnamed namespace can't be used because it is ignored. Function is global.
        Keyword \c static can be used to declare function visible only in one file (translation unit) in C.
        This works also with <tt>extern "C"</tt> functions with GCC, but it is not sure that this works with other compilers.

        \warning GCC does't warn if you use pointer to C function when pointer to C++ is expected and vice versa.

       Next rows cite SQLite C API documentation <http://www.sqlite.org/capi3ref.html#sqlite3_exec>.

       The 4th argument to \c sqlite3_exec() is relayed through to the 1st argument of each callback invocation.

       The 2nd argument to the \c sqlite3_exec() callback function is the number of columns in the result.
       The 3rd argument to the \c sqlite3_exec() callback is an array of pointers to strings obtained as if from \c sqlite3_column_text(),
       one for each column.
       If an element of a result row is \c NULL then the corresponding string pointer for the \c sqlite3_exec() callback is a \c NULL pointer.
       The 4th argument to the \c sqlite3_exec() callback is an array of pointers to strings
       where each entry represents the name of corresponding result column as obtained from \c sqlite3_column_name().

       If an \c sqlite3_exec() callback returns non-zero,
       the \c sqlite3_exec() routine returns SQLITE_ABORT without invoking
       the callback again and without running any subsequent SQL statements.
      */

  typedef int (*SqliteReaderCallbackType)(void*, int, char**, char**);
}

namespace GNU_gama { namespace local { namespace sqlite_db {

/**
   \internal
   \brief #GNU_gama::local::sqlite_db::SqliteReader class private data

   Contains all private data. In file sqlitereader.h is forward declaration of this struct.
   But declaration is only available in this file (translation unit).
   All members are public. Functions especially (<tt>extern "C"</tt>) callbacks can easy manipulate with this members.
   This is no OOP violation because we can think about functions in this file as ReaderData member functions.
   Functions outside this file can't access this structure because they know only forward declaration
   and #GNU_gama::local::sqlite_db::SqliteReader has declared pointer to this struct private of course.
   However, there are some problems with callbacks visibility
   (see #SqliteReaderCallbackType or #sqlite_db_readConfigurationInfo for details).

   Callbacks #sqlite_db_readObservations, ... and #sqlite_db_readCovarianceMatrix have to share data between it's invocations.
   So they needs access to the same \c StandPoint, \c Vectors, etc.
   They also needs access to #exception.
   This is the reason why ReaderData contains pointer to \c StandPoint etc.
   Pointers #currentStandPoint, #currentVectors, #currentCoordinates, #currentHeightDifferences
   and #currentCovarianceMatrix temporary points to objects which are in use at the moment
   by the #exec() caller and corresponding callback invocations.
   When processing of one object is finished, this pointer should by set to \c NULL pointer.
  */
struct ReaderData
{
    /**
       It sets all member variables. Pointers are set to \c NULL pointers.
       Strings are initialised by \c "" to satisfied GCC \c -Weffc++ warning options.
      */

  ReaderData() : lnet(0),
                 exception(0), sqlite3Handle(0), configurationId(""),
                 currentStandPoint(0), currentVectors(0),
                 currentCoordinates(0), currentHeightDifferences(0),
                 currentCovarianceMatrix(0)
  {
  }

  GNU_gama::local::LocalNetwork* lnet; ///< pointer to network object

  GNU_gama::Exception::base* exception; ///< an exception which was caught in callback or \c NULL if no exception was thrown
  sqlite3*  sqlite3Handle; ///< pointer to \c struct \c sqlite3
  std::string configurationId; ///< configuration id in database

  /** provides access to same stand point for callback ::readObservations and caller of #exec() function */
  GNU_gama::local::StandPoint*        currentStandPoint;
  GNU_gama::local::Vectors*           currentVectors;
  GNU_gama::local::Coordinates*       currentCoordinates;
  GNU_gama::local::HeightDifferences* currentHeightDifferences;
  /** provides access to same covariance matrix for callback ::readCovarianceMatrix and caller of #exec() function */
  GNU_gama::local::CovMat*            currentCovarianceMatrix;

private:
  /** disabled copy constructor */
  ReaderData(const ReaderData&);
  /** disabled assignment operator */
  ReaderData& operator= (const ReaderData&);
};

}}} // namespace GNU_gama local sqlite_db


using namespace GNU_gama::local::sqlite_db;


SqliteReader::SqliteReader(const std::string& fileName)
  : readerData(new ReaderData)
{
  int resCode = sqlite3_open(fileName.c_str(), &readerData->sqlite3Handle);

  if (resCode) {
    delete readerData;
    throw GNU_gama::Exception::sqlitexc(T_gamalite_database_not_open);
  }
}

/**
  If function \c sqlite3_close returns another value then \c SQLITE_OK
  (there were some error), no action is performed.
  */
SqliteReader::~SqliteReader()
{
  int resCode = sqlite3_close(readerData->sqlite3Handle);
  if (resCode == SQLITE_OK)
    {
      readerData->sqlite3Handle = 0;
    }

  if (readerData->exception) {
    delete readerData->exception;
    readerData->exception = 0;
  }
  delete readerData;
}

namespace {

  /**
     \internal
     \brief A C++ wrapper around \c sqlite3_exec function.

     Connection to database has to be open. If the \a callback is \c NULL pointer,
     then no callback function is called (result rows are ignored).
     If callback requests query execution abort (by returning non-zero value),
     no other callbacks is called and exception is thrown
     (\c sqlite3_exec returns a value which differs from \c SQLITE_OK, see also <http://www.sqlite.org/capi3ref.html#SQLITE_ABORT>).

     If callback stores pointer to an exception to #GNU_gama::local::sqlite_db::ReaderData::exception
     and callback requests query execution abort, exception will be rethrown.
     If #GNU_gama::local::sqlite_db::ReaderData::exception is \c NULL pointer,
     #GNU_gama::Exception::sqlitexc will be thrown with SQLite error message.

     \param sqlite3Handle pointer to \c struct \c sqlite3
     \param query query string
     \param callback pointer to callback function
     \param readerData pointer to \c struct ReaderData

     \throws #GNU_gama::Exception::sqlitexc if error occurs when reading from database
     \throws GNU_gama::Exception::base if is error occurred by something another -- it depends on callback
     It can also throw any other exception derived from this class.

     Callback functions are expected to handle exceptions like this:
     \code
            \\ ... try block
            catch (GNU_gama::Exception::base& e)
                {
                    d->exception = e.clone();
                }
            catch (std::exception& e)
                {
                    d->exception = new GNU_gama::Exception::string(e.what());
                }
            catch (...)
                {
                    d->exception = new GNU_gama::Exception::string("unknown");
                }
            return 1;
      \endcode

      \sa #GNU_gama::local::sqlite_db::ReaderData, #SqliteReaderCallbackType, #GNU_gama::Exception::sqlitexc
      */
  void exec(sqlite3 *sqlite3Handle, const std::string& query, SqliteReaderCallbackType callback, ReaderData* readerData)
  {
    char * errorMsg = 0;
    int rc = 0;
    rc = sqlite3_exec(sqlite3Handle, query.c_str(), callback, readerData, &errorMsg);
    if (rc != SQLITE_OK)
      {
        if (readerData->exception != 0)
          {
            readerData->exception->raise();
          }
        else if (errorMsg)
          {
            std::string s = errorMsg;
            sqlite3_free(errorMsg);
            throw GNU_gama::Exception::sqlitexc(s);
          }
      }
  }
} // unnamed namespace


void SqliteReader::retrieve(LocalNetwork*& locnet, const std::string& configuration)
{
  // at this point we do not know if locnet is defined yet
  readerData->lnet = locnet;

  // configuration info
  std::string query("select conf_id, "
                    "       algorithm, sigma_apr, conf_pr, tol_abs, sigma_act,"
                    "       update_cc, axes_xy, angles, epoch, ang_units, "
                    "       latitude, ellipsoid, cov_band "
                    "  from gnu_gama_local_configurations "
                    " where conf_name = '" + configuration + "'");
  exec(readerData->sqlite3Handle, query, sqlite_db_readConfigurationInfo, readerData);

  if (readerData->configurationId.empty())
    throw GNU_gama::Exception::sqlitexc(T_gamalite_configuration_not_found);

  locnet = readerData->lnet; // now the pointer must be defined externally or in the callback

  // configuration description
  query = "select text from gnu_gama_local_descriptions "
    " where conf_id = " + readerData->configurationId +
    " order by indx asc";
  exec(readerData->sqlite3Handle, query, sqlite_db_readConfigurationText, readerData);

  // points
  query = "select id, x, y, z, txy, tz "
          "  from gnu_gama_local_points where conf_id = " + readerData->configurationId;
  exec(readerData->sqlite3Handle, query, sqlite_db_readPoints, readerData);

  // changed cluster to ccluster
  query = "select ccluster, dim, band, tag "
         "  from gnu_gama_local_clusters where conf_id = " + readerData->configurationId;
  exec(readerData->sqlite3Handle, query, sqlite_db_readClusters, readerData);
}


namespace {
  /**
     \brief Converts string to double.

     Parameter \a s can not be \c NULL pointer.
     If conversion fails, exception is thrown.

     \param s string to convert
     \param m error message if conversion fails

     \throws #GNU_gama::Exception::sqlitexc
  */
  double ToDouble(const char* s, const std::string& m = T_gamalite_conversion_to_double_failed)
  {
    std::string ss = s;
    if (!GNU_gama::IsFloat(ss))
      throw GNU_gama::Exception::sqlitexc(m);
    std::istringstream istr(ss);
    double d;
    istr >> d;
    return  d;
  }

  /**
     \brief Converts string to integer

     \copydetails ToDouble()
  */
  int ToInteger(const char* s, const std::string& m = T_gamalite_conversion_to_integer_failed)
  {
    std::string ss = s;
    if (!GNU_gama::IsInteger(ss)) throw GNU_gama::Exception::sqlitexc(m);
    std::istringstream istr(ss);
    int n;
    istr >> n;
    return  n;
  }
} // unnamed namespace


/////////////// callbacks - extern "C" ///////////////////
int sqlite_db_readConfigurationInfo(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);

  try {
    if (argc == 14 && argv[0])
      {
        d->configurationId = argv[0];

        if (argv[1])
          d->lnet->set_algorithm(argv[1]);

        using namespace GNU_gama::local;

        // create a new local newtwork if not defined
        if (d->lnet == 0)
          {
            d->lnet = new LocalNetwork;
          }

        d->lnet->apriori_m_0(ToDouble(argv[2]));
        d->lnet->conf_pr(ToDouble(argv[3]));
        d->lnet->tol_abs(ToDouble(argv[4]));

        if (std::string(argv[5]) == "apriori")
          d->lnet->set_m_0_apriori();
        else
          d->lnet->set_m_0_aposteriori();

        d->lnet->update_constrained_coordinates((std::string(argv[6]) == "yes"));

        std::string val = argv[7];
        LocalCoordinateSystem::CS& lcs = d->lnet->PD.local_coordinate_system;
        if      (val == "ne") lcs = LocalCoordinateSystem::NE;
        else if (val == "sw") lcs = LocalCoordinateSystem::SW;
        else if (val == "es") lcs = LocalCoordinateSystem::ES;
        else if (val == "wn") lcs = LocalCoordinateSystem::WN;
        else if (val == "en") lcs = LocalCoordinateSystem::EN;
        else if (val == "nw") lcs = LocalCoordinateSystem::NW;
        else if (val == "se") lcs = LocalCoordinateSystem::SE;
        else if (val == "ws") lcs = LocalCoordinateSystem::WS;
        else lcs = LocalCoordinateSystem::NE;

        // d->lnet->PD.right_handed_angles = (std::string(argv[8]) == "right-handed");
	if (std::string(argv[8]) == "right-handed")
	  d->lnet->PD.setAngularObservations_Righthanded();
	else
	  d->lnet->PD.setAngularObservations_Lefthanded();

        if (argv[9])
          d->lnet->set_epoch(ToDouble(argv[9]));

        if (std::string(argv[10]) == "400")
          d->lnet->set_gons();
        else
          d->lnet->degrees();

        using namespace std;
        if (argv[11])
          d->lnet->set_latitude(atoi(argv[11]) * M_PI / 200);

        if (argv[12])
          d->lnet->set_ellipsoid(argv[12]);

        d->lnet->set_xml_covband(atoi(argv[13]));

        return 0;
      }
    else
      {
        throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
      }
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}

int sqlite_db_readConfigurationText(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc == 1 && argv[0])
      {
        d->lnet->description += argv[0];
        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}

int sqlite_db_readPoints(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc == 6 && argv[0])
      {
        GNU_gama::local::LocalPoint p;
        if (argv[1] && argv[2])
          p.set_xy(ToDouble(argv[1]), ToDouble(argv[2]));
        if (argv[3])
          p.set_z(ToDouble(argv[3]));
        if (argv[4])
          {
            std::string txy = argv[4];
            if      (txy == "fixed")       p.set_fixed_xy();
            else if (txy == "adjusted")    p.set_free_xy();
            else if (txy == "constrained") p.set_constrained_xy();
          }
        if (argv[5])
          {
            std::string tz = argv[5];
            if      (tz == "fixed")       p.set_fixed_z();
            else if (tz == "adjusted")    p.set_free_z();
            else if (tz == "constrained") p.set_constrained_z();
          }
        std::string pid = argv[0];
        d->lnet->PD[pid] = p;

        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}

int sqlite_db_readClusters(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc ==4 && argv[0] && argv[1] && argv[2] && argv[3])
      {
        std::string currentClusterId = argv[0];
        std::string tag = argv[3];

        GNU_gama::Cluster<GNU_gama::local::Observation>* c = 0;

        if (tag == "obs")
          {
            d->currentStandPoint = new GNU_gama::local::StandPoint(&d->lnet->OD);

            std::string query =
              "select indx, tag, from_id, to_id, to_id2, val, stdev, "
              "       from_dh, to_dh, to_dh2, dist, rejected "
              "  from gnu_gama_local_obs "
              " where conf_id = " + d->configurationId +
              "   and ccluster = " + currentClusterId ;
            exec(d->sqlite3Handle, query.c_str(), sqlite_db_readObservations, d);

            c = d->currentStandPoint;
            d->currentStandPoint = 0;
          }
        else if (tag == "vectors")
          {
            d->currentVectors = new GNU_gama::local::Vectors(&d->lnet->OD);

            std::string query =
              "select indx, from_id, to_id, dx, dy, dz, "
              "       from_dh, to_dh, rejected "
              "  from gnu_gama_local_vectors "
              " where conf_id = " + d->configurationId +
              "   and ccluster = " + currentClusterId ;
            exec(d->sqlite3Handle, query.c_str(), sqlite_db_readVectors, d);

            c = d->currentVectors;
            d->currentVectors = 0;
          }
        else if (tag == "coordinates")
          {
            d->currentCoordinates = new GNU_gama::local::Coordinates(&d->lnet->OD);

            std::string query =
              "select indx, id, x, y, z, rejected "
              "  from gnu_gama_local_coordinates "
              " where conf_id = " + d->configurationId +
              "   and ccluster = " + currentClusterId ;
            exec(d->sqlite3Handle, query.c_str(), sqlite_db_readCoordinates, d);

            c = d->currentCoordinates;
            d->currentCoordinates = 0;
          }
        else if (tag == "height-differences")
          {
            d->currentHeightDifferences = new GNU_gama::local::HeightDifferences(&d->lnet->OD);

            // clusters HeightDifferences share the same table with clusters StandPoint
            std::string query =
              "select indx, tag, from_id, to_id, to_id2, val, stdev, "
              "       from_dh, to_dh, to_dh2, dist, rejected "
              "  from gnu_gama_local_obs "
              " where conf_id = " + d->configurationId +
              "   and ccluster = " + currentClusterId ;
            exec(d->sqlite3Handle, query.c_str(), sqlite_db_readHeightDifferences, d);

            c = d->currentHeightDifferences;
            d->currentHeightDifferences = 0;
          }

        d->lnet->OD.clusters.push_back(c);

        int dim = ToInteger(argv[1]); // index is unsigned
        int band = ToInteger(argv[2]);

        c->covariance_matrix.reset(dim, band);
        d->currentCovarianceMatrix = &c->covariance_matrix;

        std::string query =
          "select rind, cind, val "
          "  from gnu_gama_local_covmat "
          " where conf_id = " + d->configurationId +
          "   and ccluster = " + currentClusterId ;
        exec(d->sqlite3Handle, query.c_str(), sqlite_db_readCovarianceMatrix, d);

        d->currentCovarianceMatrix = 0;
        c->update();

        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}

int sqlite_db_readObservations(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc ==12 && argv[1] && argv[2] && argv[3] && argv[5] && argv[11])
      {
        //    0    1       2      3       4     5      6      7       8      9      10     11
        //  indx, tag, from_id, to_id, to_id2, val, stdev, from_dh, to_dh, to_dh2, dist, rejected
        // 1 2 3 5 11  tested for NULL on input
        // 0 4 6 7 8 9 10  not tested
        // indx = argv[0] shouldn't be NULL but gama doesn't use it

        std::string dir_from;
        std::string tag  = argv[1];
        std::string from = argv[2];
        std::string to   = argv[3];
        double      val  = ToDouble(argv[5]);

        GNU_gama::local::Observation* obs = 0;

        if (tag == "direction")
          {
            obs = new GNU_gama::local::Direction(from, to, val);
            if (dir_from.empty())
              {
                d->currentStandPoint->station = dir_from = from;
              }
            if (dir_from != from)
              {
                throw GNU_gama::Exception::sqlitexc(T_gamalite_stand_point_cluster_with_multi_dir_sets);
              }
          }
        else if (tag == "distance")
          {
            obs = new GNU_gama::local::Distance  (from, to, val);
          }
        else if (tag == "angle" && argv[4])
          {
            GNU_gama::local::Angle*
              ang = new GNU_gama::local::Angle (from, to, argv[4], val);
            if (argv[9]) ang->set_fs_dh(ToDouble(argv[9]));
            obs = ang;
          }
        else if (tag == "s-distance")
          {
            obs = new GNU_gama::local::S_Distance(from, to, val);
          }
        else if (tag == "z-angle")
          {
            obs = new GNU_gama::local::Z_Angle   (from, to, val);
          }
        else if (tag == "azimuth")
          {
            obs = new GNU_gama::local::Azimuth   (from, to, val);
          }
        else if (tag == "dh")
          {
            GNU_gama::local::H_Diff*
              dh = new GNU_gama::local::H_Diff    (from, to, val);
            if (argv[10]) dh->set_dist(ToDouble(argv[10]));
            obs = dh;
          }

        if (argv[7]) obs->set_from_dh(ToDouble(argv[7]));
        if (argv[8]) obs->set_to_dh  (ToDouble(argv[8]));

        if(d->currentStandPoint)
          d->currentStandPoint->observation_list.push_back(obs);

        int rejected = ToInteger(argv[11]);
        if (rejected) obs->set_passive();

        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}


int sqlite_db_readVectors(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc ==9 && argv[1] && argv[2] && argv[3] && argv[4] && argv[5] && argv[8])
      {
        //  0     1        2      3   4   5   6        7      8
        //  indx, from_id, to_id, dx, dy, dz, from_dh, to_dh, rejected

        std::string from = argv[1];
        std::string to   = argv[2];
        double      dx   = ToDouble(argv[3]);
        double      dy   = ToDouble(argv[4]);
        double      dz   = ToDouble(argv[5]);

        GNU_gama::local::Xdiff* xdiff = new GNU_gama::local::Xdiff(from, to, dx);
        GNU_gama::local::Ydiff* ydiff = new GNU_gama::local::Ydiff(from, to, dy);
        GNU_gama::local::Zdiff* zdiff = new GNU_gama::local::Zdiff(from, to, dz);

        if (argv[6])
          {
            double from_dh = ToDouble(argv[7]);
            xdiff->set_from_dh(from_dh);
            ydiff->set_from_dh(from_dh);
            zdiff->set_from_dh(from_dh);
          }

        if (argv[7])
          {
            double to_dh = ToDouble(argv[8]);
            xdiff->set_to_dh(to_dh);
            ydiff->set_to_dh(to_dh);
            zdiff->set_to_dh(to_dh);
          }

        if (int rejected = ToInteger(argv[8]))
          {
            xdiff->set_passive();
            ydiff->set_passive();
            zdiff->set_passive();
          }

        d->currentVectors->observation_list.push_back(xdiff);
        d->currentVectors->observation_list.push_back(ydiff);
        d->currentVectors->observation_list.push_back(zdiff);

        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}


int sqlite_db_readCoordinates(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc ==6 && argv[1])
      {
        //  0     1   2  3  4  5
        //  indx, id, x, y, z, rejected

        std::string id = argv[1];

        int reject = 0;
        if (argv[5]) reject = ToInteger(argv[5]);

        GNU_gama::local::X* x = 0;
        GNU_gama::local::Y* y = 0;
        GNU_gama::local::Z* z = 0;

        if (argv[2] && argv[3])
          {
            x = new GNU_gama::local::X(id, ToDouble(argv[2]));
            y = new GNU_gama::local::Y(id, ToDouble(argv[3]));
            if (reject)
              {
                x->set_passive();
                y->set_passive();
             }
          }

        if (argv[4])
          {
            z = new GNU_gama::local::Z(id, ToDouble(argv[4]));
            if (reject)
              {
                z->set_passive();
              }
          }

        if (x) d->currentCoordinates->observation_list.push_back(x);
        if (y) d->currentCoordinates->observation_list.push_back(y);
        if (z) d->currentCoordinates->observation_list.push_back(z);

        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}


int sqlite_db_readHeightDifferences(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc ==12 && argv[1] && argv[2] && argv[3] && argv[5] && argv[11])
      {
        //  0     1    2        3      4       5    6      7        8      9       10    11
        //  indx, tag, from_id, to_id, to_id2, val, stdev, from_dh, to_dh, to_dh2, dist, rejected
        // 1 2 3 5 10 11  tested for NULL on input
        // 0 4 6 7 8 9  not tested
        // indx = argv[0] shouldn't be NULL but gama doesn't use it

        std::string   tag  = argv[1];
        std::string   from = argv[2];
        std::string   to   = argv[3];
        double        val  = ToDouble(argv[5]);
        double        dist = 0;
        if (argv[10]) dist = ToDouble(argv[10]);

        GNU_gama::local::H_Diff* hdiff = new GNU_gama::local::H_Diff(from, to, val, dist);

        d->currentHeightDifferences->observation_list.push_back(hdiff);

        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}


int sqlite_db_readCovarianceMatrix(void* data, int argc, char** argv, char**)
{
  ReaderData* d = static_cast<ReaderData*>(data);
  try {
    if (argc == 3 && argv[0] && argv[1] && argv[2])
      {
        int r = ToInteger(argv[0]);
        int c = ToInteger(argv[1]);
        double v = ToDouble (argv[2]);

        (*d->currentCovarianceMatrix)(r,c) = v;

        return 0;
      }
    else
      throw GNU_gama::Exception::sqlitexc(T_gamalite_invalid_column_value);
  }
  catch (GNU_gama::Exception::base& e)
    {
      d->exception = e.clone();
    }
  catch (std::exception& e)
    {
      d->exception = new GNU_gama::Exception::string(e.what());
    }
  catch (...)
    {
      d->exception = new GNU_gama::Exception::string(T_gamalite_unknown_exception_in_callback);
    }
  return 1;
}

/////////////// end of callbacks ///////////////////

#endif  // GNU_GAMA_LOCAL_SQLITE_READER
