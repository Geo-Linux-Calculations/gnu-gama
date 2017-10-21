/* Ellipsoids_xml is a simple program to transform ellipsoids.xml
 * input file into ellipsoids.[h|cpp|html|texi] output.
 * ==========================================================================
 */

const char* version = "1.00";

/* ---------------------------------------------------------------------------
 *
 * 1.00  2017-11-21
 *
 *       - include guard in ellipsoids.h without double undescores
 *
 * 0.06  2006-08-26
 *
 *       - fixed code to avoid some minor warnings
 *
 * 0.05  2005-09-04
 *
 *       - public data member 'id' was added into class Ellipsoid,
 *       - added table gama_ellipsoid_id a and setting of
 *         Ellipsoid::id in function set(Ellipsoid*, gama_ellipsoid)
 *
 * 0.04  2002-03-22
 *
 *       - changed target directory from gamalib to gnu_gama
 *       - changed namespace from GaMalib to GNU_gama
 *
 * 0.03  2002-09-20
 *
 *       - added output in the Texinfo format
 *
 * 0.02  2002-06-28
 *
 *       - missing `#include <cstring>' in generated code (Jan Pytel)
 *
 * 0.01  2002-06-09
 *
 *       - initial draft
 *
 * ---------------------------------------------------------------------------
 *
 *  GNU Gama -- Adjustment of geodetic networks
 *  Copyright (C) 2002, 2006  Ales Cepek <cepek@fsv.cvut.cz>
 *
 *  This file is part of the GNU Gama.
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * ---------------------------------------------------------------------------
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <list>
#include <gnu_gama/xml/encoding.h>

using namespace std;
using namespace GNU_gama;

// ---------------------------------------------------------------------------

enum parser_state { START, REFERENCES, ITEM, ELLIPSOIDS, ELLIPSOID, END };
int  result = 0;

class Parser {

public:

  struct Entry
  {
    string id, caption, a, b, f, f1, ref;
  };
  parser_state state;
  string       revision;

  Parser(const char* filename);
  ~Parser();

  void error   (const char*);
  void add     (Entry e)        { elist.push_back(e); }
  void xml2h   (ostream&);
  void xml2cpp (ostream&);
  void xml2html(ostream&);
  void xml2texi(ostream&);
  void addlabel(string s)       { label.push_back(s); }
  void additem (string s)       { item .push_back(s); }

  string       item_data;

private:

  list<Entry>  elist;
  list<string> label;
  list<string> item;

  XML_Parser expat_parser;
  string     filename;
  string     errString;
  int        errCode;
  int        errLine;
  string     text;

};



void startElement(void *userData, const char *cname, const char **atts)
{
  Parser* parser = static_cast<Parser*>(userData);
  const string name(cname);

  if (parser->state == START && name == "ellipsoids")
    {
      parser->state = ELLIPSOIDS;

      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);

          if (nam == "revision")  parser->revision = val;
          else
            {
              parser->error("unknown attribute");
            }
        }
    }
  else if (parser->state == ELLIPSOIDS && name == "references")
    {
      parser->state = REFERENCES;

      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);

          {
            parser->error("unknown attribute");
          }
        }
    }
  else if (parser->state == REFERENCES && name == "item")
    {
      parser->state = ITEM;
      parser->item_data.erase();

      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);

          if (nam == "label")
            {
              parser->addlabel(val);
            }
          else
            {
              parser->error("unknown attribute");
            }
        }
    }
  else if (parser->state == ELLIPSOIDS && name == "ellipsoid")
    {
      parser->state = ELLIPSOID;

      string id;
      Parser::Entry  entry;
      while (*atts)
        {
          string nam = string(*atts++);
          string val = string(*atts++);

          if      (nam == "id")      entry.id = val;
          else if (nam == "caption") entry.caption = val;
          else if (nam == "a")       entry.a = val;
          else if (nam == "b")       entry.b = val;
          else if (nam == "f")       entry.f = val;
          else if (nam == "f1")      entry.f1 = val;
          else if (nam == "ref")     entry.ref = val;
          else
            {
              parser->error("unknown attribute");
            }
        }
      parser->add(entry);
    }
  else
    parser->error("unknown tag");
}

void endElement(void *userData, const char * /*cname*/)
{
  Parser* parser = static_cast<Parser*>(userData);

  switch (parser->state)
    {
    case ELLIPSOIDS: parser->state = END;        break;
    case REFERENCES: parser->state = ELLIPSOIDS; break;
    case ITEM      : parser->state = REFERENCES;
                     parser->additem(parser->item_data);
                                                 break;
    case ELLIPSOID : parser->state = ELLIPSOIDS; break;
    default        : break;
    }
}

void characterDataHandler(void *userData, const char* s, int len)
{
  Parser* parser = static_cast<Parser*>(userData);

  if (parser->state == ITEM)
    {
      if (parser->item_data.empty() && *s == '\n')   // remove leading '\n'
        {
          s++;
          len--;
        }
      if (len > 0) parser->item_data += string(s, len);
    }
  else
    for (int b=0; b <len; b++)
      if (!isspace(s[b]))
        {
          parser->error("ignored junk");
          return;
        }
}



Parser::Parser(const char* fn)
{
  expat_parser = XML_ParserCreate(0);

  XML_SetUserData(expat_parser, this);
  XML_SetElementHandler(expat_parser, startElement, endElement);
  XML_SetCharacterDataHandler(expat_parser, characterDataHandler);
  XML_SetUnknownEncodingHandler(expat_parser, UnknownEncodingHandler, 0);

  filename = string(fn);
  state = START;

  ifstream inp(fn);
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

void Parser::xml2h(ostream& out)
{
  out << "#ifndef GNU_gama_gama_ellipsoids_header_file_h\n"
      << "#define GNU_gama_gama_ellipsoids_header_file_h\n\n"
      << "#include <gnu_gama/ellipsoid.h>\n\n"
      << "// This file was generated by GNU Gama (program ellipsoids_xml"
      << /* version << */ ") from\n"
      << "// http://www.gnu.org/software/gama/xml/ellipsoids.xml"
      << " revision " << revision << "\n\n"
      << "namespace GNU_gama {\n\n"
      << "enum gama_ellipsoid {\n\n"
      << "ellipsoid_unknown,\n";

  for (list<Entry>::iterator i=elist.begin(); i!=elist.end();)
    {
      Entry e = *i;
      out << "ellipsoid_" << e.id;
      if (++i != elist.end()) out << ",";
      out << "\t\t// " << e.a << "   " << e.b+e.f+e.f1 << "\n";
    }

  out << "\n};\n\n"
      << "extern const char * const gama_ellipsoid_caption[];\n"
      << "extern const char * const gama_ellipsoid_id     [];\n\n"
      << "gama_ellipsoid ellipsoid(const char*);\n"
      << "int            set      (Ellipsoid*, gama_ellipsoid);\n"
      << "\n}\n\n"
      << "#endif\n";
}

void Parser::xml2cpp(ostream& out)
{
  out << "#include <gnu_gama/ellipsoids.h>\n"
      << "#include <cstring>\n\n"
      << "// This file was generated by GNU Gama (program ellipsoids_xml"
      << /* version << */ ") from\n"
      << "// http://www.gnu.org/software/gama/xml/ellipsoids.xml"
      << " revision " << revision << "\n\n"
      << "namespace GNU_gama {\n\n";
  out << "const char * const gama_ellipsoid_caption[] = { \"\",\n";
  {
    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); )
      {
        Entry e = *i;
        out << "   \"" << e.caption << "\"";
        if (++i != elist.end()) out << ",";
        out << "\n";
      }
  }
  out << "};\n\n";
  out << "const char * const gama_ellipsoid_id[] = { \"\",\n";
  {
    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); )
      {
        Entry e = *i;
        out << "   \"" << e.id << "\"";
        if (++i != elist.end()) out << ",";
        out << "\n";
      }
  }
  out << "};\n\n";

  out << "int set(Ellipsoid* E, gama_ellipsoid T)\n"
      << "{\n"
      << "   switch(T) {\n";
  {
    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); ++i)
      {
        Entry e = *i;
        out << "   case ellipsoid_" << e.id  << " :\n";
        out << "      E->set_a";
        if (!e.b.empty())
          out << "b( " << e.a << ", " << e.b  << " );\n";
        else if (!e.f.empty())
          out << "f( " << e.a << ", " << e.f  << " );\n";
        else if (!e.f1.empty())
          out << "f1( " << e.a << ", " << e.f1 << " );\n";
        out << "      E->id = ellipsoid_" << e.id << ";\n";
        out << "      break;\n";
      }
  }
  out <<
    "   default :\n"
    "      E->id = ellipsoid_unknown;\n"
    "      return 1;\n   }\n\n   return 0;\n}\n\n";

  out << "gama_ellipsoid ellipsoid(const char* s)\n"
      << "{\n"
      << "   using namespace std;\n\n"
      << "   gama_ellipsoid T = ellipsoid_unknown;\n\n";
  bool first = true;
  for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); ++i)
    {
      Entry e = *i;
      out << "   ";
      if (!first) out << "else ";
      out << "if ";
      if (first)
        {
          out << "     ";
          first = false;
        }
      out << "(!strcmp(\""<< e.id << "\", s))"
          << "  T = ellipsoid_" << e.id << ";\n";
    }
  out << "\n   return T;\n}\n\n}      // namespace GNU_gama\n";
}

void Parser::xml2html(ostream& out)
{
    out <<
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      "<!DOCTYPE html\n"
      "     PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
      "     \"DTD/xhtml1-strict.dtd\">\n"
      "<html xmlns=\"http://www.w3.org/1999/xhtml\""
      " xml:lang=\"en\" lang=\"en\">\n"
      "  <head>\n"
      "    <title>GNU Gama ellipsoids</title>\n"
      "  </head>\n"
      "<body>\n\n"
      "<h1><a href=\"http://www.gnu.org/software/gama/xml/ellipsoids.xml\">"
      "GNU Gama ellipsoids</a></h1>\n\n"
      "<h2>revision " << revision << "</h2>\n\n"
      "<table border=\"1\">\n\n";

    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); ++i)
      {
        Entry e = *i;

        out << "<tr>\n";
        out << "<td>" << e.id         << "</td>\n";
        out << "<td>" << e.a          << "</td>\n";
        out << "<td>" << e.b+e.f+e.f1 << "</td>\n";
        out << "<td>" << e.caption    << "</td>\n";
        out << "<td>" << e.ref        << "</td>\n";
        out << "</tr>\n";
      }

    out << "\n</table>\n\n";

    out << "<h2>References</h2>\n\n";

    out << "<dl>\n";
    list<string>::iterator li=label.begin(), ti=item.begin();
    while (li != label.end() && ti != item.end())
      {
        out << "<dt><strong>" << *li << "</strong></dt>";
        out << "<dd>" << *ti << "</dd>\n\n";
        ++li, ++ti;
      }
    out << "</dl>\n\n";

    out << "</body>\n</html>\n";
}

void Parser::xml2texi(ostream& out)
{
  out <<
    "@comment GNU Gama ellipsoids (revision " << revision << ")\n"
    "@comment http://www.gnu.org/software/gama/xml/ellipsoids.xml\n"
    "@multitable @columnfractions .20 .20 .20 .30 .10\n"
    "@item id @tab a  @tab b, 1/f, f @tab description\n\n";

    for (list<Entry>::iterator i=elist.begin(); i!=elist.end(); ++i)
      {
        Entry e = *i;

        out
          << "@item " << e.id
          << " @tab " << e.a
          << " @tab " << (e.b+e.f+e.f1)
          << " @tab " << e.caption
          << " @tab " << e.ref
          << "\n";
      }

  out << "@end multitable\n\n";


  out << "@multitable @columnfractions .07 .93\n";
  list<string>::iterator li=label.begin(), ti=item.begin();
  while (li != label.end() && ti != item.end())
    {
      out
        << "@item " << *li++
        << " @tab " << *ti++
        << "\n";
      }
  out << "@end multitable\n";

}

int main(int argc, char* argv[])
{
  if (argc != 3 ||
      (argc == 3 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help") ||
                    !strcmp(argv[2],"-h") || !strcmp(argv[2],"--help"))) )
    {
      cout << "\nusage: ellipsoids_xml  input.xml  output_directory\n\n";
      return 1;
    }

  Parser parser(argv[1]);

  string path(argv[2]);
  if (*path.rbegin() != '/') path = path + '/';

  {
    string file = path + "ellipsoids.h";
    ofstream out(file.c_str());

    parser.xml2h(out);
  }
  {
    string file = path + "ellipsoids.cpp";
    ofstream out(file.c_str());

    parser.xml2cpp(out);
  }
  {
    string file = path + "ellipsoids.html";
    ofstream out(file.c_str());

    parser.xml2html(out);
  }
  {
    string file = path + "ellipsoids.texi";
    ofstream out(file.c_str());

    parser.xml2texi(out);
  }

  return result;
}
