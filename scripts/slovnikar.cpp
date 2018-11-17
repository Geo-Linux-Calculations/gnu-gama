/* Slovnikar (lexicographer) is a simple program to compile `dictionaries'
 * used in the GNU Gama project.
 * ==========================================================================
 *
 * Program reads series of file names from standard input. Input files
 * contain translations of texts in the following format:
 *
 *   <?xml version="1.0" standalone="yes"?>  <!-- add your encoding="..." -->
 *   <entries>
 *   <e id="my_text_id"   en="This is my text"
 *                        cz="Toto je muj text"  />
 *   <e id="your_text_id" en="This is your text"
 *                        cz="Toto je tvuj text" />
 *       <!-- ...... -->
 *   </entries>
 *
 * Program compiles dictionarise and write files language.h and language.cpp
 *
 * To ease creation of language files, special attribute EN is
 * available.  Attribute EN is ignored on input and serves as a kind
 * of comment.
 *
 * ------------------------------------------------------------------------ */

         const char* language[] = { "en", 
                                    "ca", "cz", "du", "es", "fi",
                                    "fr", "hu", "ru", "ua", "zh" };

         const int N = sizeof(language)/sizeof(const char*);

         const char* version = "1.14";

/* ---------------------------------------------------------------------------
 *
 * 1.14  2018-11-17
 *
 *       - <meta content="text/html; charset=UTF-8"... /> added into HTML head
 *
 * 1.13  2017-10-21
 *
 *       - generate include guards withou double underscores
 *
 * 1.12  2011-07-10
 *
 *       - added Chinese translation in GBK encoding (switch "zh")
 *         by 薇 <trackv1@qq.com>
 *
 * 1.11  2011-04-12
 *
 *       - added Spanish (switch "es")
 *
 * 1.10  2010-08-14
 *
 *       - namespace GNU_gama::local
 *
 * 1.09  2006-08-26
 *
 *       - fixed code to avoid some minor warnings
 *
 * 1.08  2005-12-15
 *
 *       - added Ukrainian (switch "ua")
 *
 * 1.07  2005-08-31
 *
 *       - added French (switch "fr")
 *
 * 1.06  2005-08-21
 *
 *       - added Russin (switch "ru")
 *
 * 1.05  2004-05-01
 *
 *       - aplied patches for adding Catalan translation by Diego Berge
 *
 * 1.04  2004-04-12
 *
 *       - added Hungarian translation by Zoltan Faludi
 *
 * 1.03  2003-05-09
 *
 *       - encoding.[h|cpp] moved to gnu_gama/xml (from gamalib)
 *
 * 1.02  2003-03-16
 *
 *       - processing moved from build-dictionaries to Makefile-slovnikar
 *
 * 1.01  2002-11-22
 *
 *       - added Dutch (switch "du")
 *
 * 1.00  2002-09-29
 *
 *       - added Finish (switch "fi")
 *       - removed a bug in processing multiple languages in element <e />
 *
 * 0.05  2002-06-04
 *
 *       - added include <iostrem> (g++ 3.0.4)
 *
 * 0.04  2001-11-28
 *
 *       - all undefined texts set to
 *         "internal error : program must call function set_gama_language()"
 *
 * 0.03  2001.05.07
 *
 *       - added function set_gama_language() for dynamic language setting
 *
 * 0.02  2000.08.07
 *
 *       - language.html review table was added
 *       - language "EN" is ignored (changing "en" to "EN" is used for
 *         commenting out the English texts)
 *
 * 0.01  2000.08.04
 *
 *       - first draft
 *
 * ---------------------------------------------------------------------------
 *
 *   Geodesy and Mapping C++ Library (GNU_gama::local)
 *   Copyright (C) 2000, 2006, 2012  Ales Cepek <cepek@gnu.org>
 *
 *   This file is part of the GNU_gama::local C++ Library.
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library (see COPYING.LIB); if not, write to the
 *   Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ---------------------------------------------------------------------------
 */

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

struct Entry {
  string lang[N];
};

typedef map<string, Entry> Dictionary;

Dictionary dict;

// ---------------------------------------------------------------------------

#include <gnu_gama/xml_expat.h>
#include <gnu_gama/xml/encoding.h>

using namespace GNU_gama;

enum parser_state { START, ENTRIES, ENTRY, END };
int  result = 0;

class Parser {

  XML_Parser expat_parser;
  string     filename;
  string     errString;
  int        errCode;
  int        errLine;
  string     text;

public:

  Parser(string filename);
  ~Parser();

  parser_state state;

  void error(const char*);
};



void startElement(void *userData, const char *cname, const char **atts)
{
  Parser* parser = static_cast<Parser*>(userData);
  const string name(cname);

  if (parser->state == START && name == "entries")
    {
      parser->state = ENTRIES;
    }
  else if (parser->state == ENTRIES && name == "e")
    {
      parser->state = ENTRY;

      string id;
      Entry  entry, dict_entry;
      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);

          if (nam == "id")
            {
              id = val;
              dict_entry = dict[id];
              continue;
            }
          else if (nam == "EN")
            {
              continue;   // "EN" is supposed to be comment attribute
            }

          int lang=N;
          for (int i=0; i<N; i++)
            if (nam == language[i])
              {
                entry.lang[i] = val;
                lang = i;
                break;
              }
          if (lang == N) parser->error("unknown language");
        }

      if (id == "")
        parser->error("id not defined");
      else
        {
          for (int l=0; l<N; l++)
            {
              if (entry.lang[l] == "") continue;

              if (dict_entry.lang[l] != ""  &&
                  dict_entry.lang[l] != entry.lang[l])
                {
                  string txt = id + " / "
                    + string(language[l]) + " = "
                    + entry.lang[l] + " redefined";
                  parser->error(txt.c_str());
                }
              dict_entry.lang[l] = entry.lang[l];
            }

          dict[id] = dict_entry;
        }
    }
  else
    parser->error("unknown tag");
}

void endElement(void *userData, const char * /*cname*/)
{
  Parser* parser = static_cast<Parser*>(userData);

  switch (parser->state)
    {
    case ENTRIES: parser->state = END;     break;
    case ENTRY  : parser->state = ENTRIES; break;
    default     : break;
    }
}

void characterDataHandler(void *userData, const char* s, int len)
{
  Parser* parser = static_cast<Parser*>(userData);

  for (int b=0; b <len; b++)
    if (!isspace(s[b]))
      {
        parser->error("ignored junk");
        return;
      }
}



Parser::Parser(string fn)
{
  expat_parser = XML_ParserCreate(0);

  XML_SetUserData(expat_parser, this);
  XML_SetElementHandler(expat_parser, startElement, endElement);
  XML_SetCharacterDataHandler(expat_parser, characterDataHandler);
  XML_SetUnknownEncodingHandler(expat_parser, UnknownEncodingHandler, 0);

  filename = fn;
  state = START;

  ifstream inp(filename.c_str());
  if (!inp) cerr << "cannot open file " << filename << endl;
  while (getline(inp, text))
    {
      text += "\n";
      int err = XML_Parse(expat_parser, text.c_str(), text.length(), false);
      if (err == 0)
        {
          errString = string(XML_ErrorString(XML_GetErrorCode(expat_parser)));
          errCode   = XML_GetErrorCode(expat_parser);
          errLine   = XML_GetCurrentLineNumber(expat_parser);

          cerr << filename << ":" << errLine << ": "
               << errString << " : " << text << endl;
          return;
        }
    }
  XML_Parse(expat_parser, "", 0, true);
}

Parser::~Parser()
{
  XML_ParserFree(expat_parser);
}

void Parser::error(const char* message)
{
  int erl = XML_GetCurrentLineNumber(expat_parser);
  cerr << filename << ":" << erl << ": " << message << ": " << text << endl;
  result = 1;
}

int main()
{
  /* reading all input files*/
  {
    string f;
    while (cin >> f) { Parser p(f); };
  }

  /* writing header-file language.h */
  {
    ofstream out("language.h");

    out << "#ifndef GNU_gama_local_language_header_file_h\n";
    out << "#define GNU_gama_local_language_header_file_h\n\n";

    out << "namespace GNU_gama { namespace local {      /* slovnikar " << version << " */\n\n";

    out << "enum gama_language {";
    for (int N=sizeof(language)/sizeof(const char*)-1, i=0; i<=N; i++)
      {
        out << " " << language[i];
        if (i != N) out << ",";
      }
    out << " };\n";
    out << "void set_gama_language(gama_language);\n\n";

    for (Dictionary::const_iterator i=dict.begin(); i!=dict.end(); ++i)
      {
        out << "extern const char* " << (*i).first << ";\n";
      }

    out << "\n}}\n\n";
    out << "#endif\n";
  }

  /* language.html */
  {
    ofstream out("language.html");
    out <<
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      "<!DOCTYPE html\n"
      "     PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
      "     \"DTD/xhtml1-strict.dtd\">\n"
      "<!-- slovnikar version " << version << " -->\n"
      "<html xmlns=\"http://www.w3.org/1999/xhtml\""
      " xml:lang=\"en\" lang=\"en\">\n"
      "  <head>\n"
      "    <meta content=\"text/html; charset=utf-8\""
      " http-equiv=\"Content-Type\" />\n"
      "    <title>GNU Gama lang</title>\n"
      "  </head>\n"
      "<body>\n\n"
      "<h1>GNU Gama lang</h1>\n\n"
      "<table border=\"1\">\n\n";

      for (Dictionary::const_iterator i=dict.begin(); i!=dict.end(); ++i)
        {
          out << "<tr><td colspan=\"2\"><tt style=\"color : navy\">"
              << (*i).first << "</tt></td></tr>\n";
          for (int l=0; l<N; l++)
            {
              out << "<tr>"
                  << "<td>" << language[l] << "</td>"
                  << "<td>";

              string s = (*i).second.lang[l];
              for (string::const_iterator c=s.begin(); c!=s.end(); ++c)
                if (*c == '<')
                  out << "&lt;";
                else if (*c == '>')
                  out << "&gt;";
                else if (*c == '&')
                  out << "&amp;";
                else
                  out << *c;

              out << "</td>"
                  << "</tr>\n";
            }
        }

    out << "\n</table>\n</body>\n</html>\n";
  }

  /* writing files language.cpp */
  {
    ofstream out("language.cpp");
    out << "/* slovnikar " << version << " */\n\n"
        << "#include <gnu_gama/local/language.h>\n\n"
        << "namespace GNU_gama { namespace local {\n\n"
        << "const char* T_language_cpp_internal_error = "
        << "\" internal error : "
        << "program must call function set_gama_language() \";\n\n";

    Dictionary::const_iterator i;
    for (i=dict.begin(); i!=dict.end(); ++i)
      {
        out << "const char* "
            << (*i).first
            << " = T_language_cpp_internal_error;\n";
      }
    out << endl;

    out << "void set_gama_language(gama_language lang)\n"
        << "{\n"
        << "   switch(lang)\n"
        << "   {\n";

    out << "   default:\n";
    for (int l=0; l<N; l++)
      {
      out << "   case " << language[l] << ":\n";
      for (i=dict.begin(); i!=dict.end(); ++i)
        {
          string text = (*i).second.lang[l];
          if (text == "")
            text = (*i).second.lang[0];   // English is used implicitly

          out << "\t" << (*i).first << "=\"" << text << "\";\n";
        }
        out << "\treturn;\n\n";

      }
    out << "   }\n\n}\n\n}}   // namespace GNU_gama::local\n";
  }

  return result;
}
