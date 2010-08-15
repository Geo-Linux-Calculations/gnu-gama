/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_Input_Text_Stream_h
#define gama_local_Input_Text_Stream_h

#include <string>
#include <stack>
#include <utility>
#include <cctype>
#include <cstdlib>
#include <cerrno>

namespace GNU_gama { namespace local {

template <typename InputStream> class InputTextStream {

  // template parameter InputStream must implement `bool get(char&)'
  // with the same meaning as std::istream::get(char&)

public:

  InputTextStream(InputStream& inp, bool sc=false, size_t alloc=256) :
    input_stream_(inp), skip_comments_(sc), eot_(false), stop_(false),
    byte_(0), line_(0), index_(0), end_(0)
    {
      buffer_.resize(alloc);
      read_buffer_();
    }

  void push_back(char c) { stack_.push(c); }
  void push_back(const std::string&);

  bool get(char&);                        // get char (' ' on eot() or eol())
  bool get(std::string&);                 //     word
  bool get(int&);                         //     int
  bool get(double&);                      //     double

  bool get_int(std::string&);             //     word representing integer
  bool get_float(std::string&);           //                       float

  bool eot() const { return eot_; }       // end of text
  bool eol() const { return eol_; }       // end of line

  bool skip_comments() const { return skip_comments_; }
  bool skip_comments(bool b) { return skip_comments_ = b; }

  bool skip_ws();                         // skip white spaces

  size_t byte()  const { return byte_;  } // current byte
  size_t line()  const { return line_;  } //         line

  std::pair<const char*, size_t> get_buffer() const {
	  return std::pair<const char*, size_t>(buffer_.c_str(), end_);
  }
  size_t get_buffer_index() const { return index_; }

private:

  InputStream&     input_stream_;
  bool             skip_comments_;
  bool             eot_, eol_, stop_;
  std::string      buffer_;
  size_t           byte_, line_, index_, end_;
  std::stack<char> stack_;

  void read_buffer_();

};

template <typename InputStream>
inline bool InputTextStream<InputStream>::get(char& c)
  {
    if (!stack_.empty())
      {
        c = stack_.top();
        stack_.pop();
        eol_ = false;    // pushed back 'eols' are ignored
        return true;
      }

    for (;;)
      if (eot_)
        {
          c = ' ';
          eol_ = true;
          return false;
        }
      else if (index_ < end_)
        {
          c = buffer_[index_];
          ++index_;
          ++byte_;
          if (skip_comments_ && (c == '#') )
            {
              c = ' ';
              byte_ += end_ - index_ + 1;
              index_ = end_ + 1;
              eol_   = true;
              return true;
            }
          eol_ = false;
          return true;
        }
      else if (index_ == end_)
        {
          c = ' ';
          ++index_;
          ++byte_;
          eol_ = true;
          return true;
        }
      else
        {
          read_buffer_();
        }
  }

template <typename InputStream>
bool InputTextStream<InputStream>::get(int& n)
  {
    using namespace std;
    string word;

    if (!get_int(word)) return false;
    errno = 0;
    n = atoi(word.c_str());

    if (errno)
      {
        push_back(word);
        n = 0;
        return false;
      }

    return true;
  }

template <typename InputStream>
bool InputTextStream<InputStream>::get(double& n)
  {
    using namespace std;
    string word;

    if (!get_float(word)) return false;
    errno = 0;
    n = atof(word.c_str());

    if (errno)
      {
        push_back(word);
        n = 0;
        return false;
      }

    return true;
  }

template <typename InputStream>
bool InputTextStream<InputStream>::get_int(std::string& word)
  {
    word = "";
    skip_ws();

    char sign;
    get(sign);
    switch (sign) {
    case '+':
    case '-':
      word += sign;
      break;
    default:
      push_back(sign);
      break;
    }

    char digit;
    while(get(digit))
      {
        bool eod = false;
        switch (digit) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          word += digit;
          break;
        default:
          push_back(digit);
          eod = true;
          break;
        }
        if (eod) break;
      }

    if (word == "+" || word == "-")
      {
        push_back(word[0]);
        return false;
      }

    return word!="";
  }

template <typename InputStream>
bool InputTextStream<InputStream>::get_float(std::string& word)
  {
    word = "";
    skip_ws();

    char sign;
    get(sign);
    switch (sign) {
    case '+':
    case '-':
      word += sign;
      break;
    default:
      push_back(sign);
      break;
    }

    bool has_digit = false;
    char digit;
    while(get(digit))
      {
        bool eod = false;
        switch(digit) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          word += digit;
          has_digit = true;
          break;
        default:
          push_back(digit);
          eod = true;
          break;
        }
        if (eod) break;
      }

    char dot;
    get(dot);
    switch(dot) {
    case '.':
      word += dot;
      break;
    default:
      push_back(dot);
    }

    while(get(digit))
      {
        bool eod = false;
        switch(digit) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          word += digit;
          has_digit = true;
          break;
        default:
          push_back(digit);
          eod = true;
          break;
        }
        if (eod) break;
      }

    if (!has_digit) {
      push_back(word);
      word = "";
      return false;
    }

    char e;
    get(e);
    if (e != 'e' && e !='E')
      push_back(e);
    else
      {
        std::string exp;
        exp += e;

        get(sign);
        switch(sign) {
        case '+':
        case '-':
          exp += sign;
          break;
        default:
          push_back(sign);
          break;
        }

        has_digit = false;
        while(get(digit))
          {
            bool eod = false;
            switch(digit) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
              exp += digit;
              has_digit = true;
              break;
            default:
              push_back(digit);
              eod = true;
              break;
            }
            if (eod) break;
          }

        word += exp;
        if (!has_digit)
          {
            push_back(word);
            word = "";
            return false;
          }
      }

    return true;
  }

template <typename InputStream>
bool InputTextStream<InputStream>::get(std::string& word)
  {
    word = "";

    if (!skip_ws()) return false;

    char  c;
    while (get(c))
      {
        if (isspace(c))
          {
            push_back(c);
            return true;
          }
        word += c;
      }

    return true;
  }

template <typename InputStream>
void InputTextStream<InputStream>::push_back(const std::string& word )
  {
    using namespace std;
    stack_.push(' ');
    for (string::const_reverse_iterator r=word.rbegin(); r!=word.rend(); ++r)
      stack_.push(*r);
  }

template <typename InputStream>
bool InputTextStream<InputStream>::skip_ws()
  {
    char c;
    while (get(c))
      if (!isspace(c))
        {
          push_back(c);
          break;
        }
    return !eot();
  }

template <typename InputStream>
void InputTextStream<InputStream>::read_buffer_()
  {
    if (eot_) return;
    if (stop_)
      {
        eot_ = true;
        return;
      }

    ++line_;
    eol_ = false;
    index_ = end_ = 0;
    char c;
    while (input_stream_.get(c))
      {
        if (c == '\n')
          {
            if (end_ < buffer_.length())
              buffer_[end_] = ' ';
            else
              buffer_ += ' ';
            return;
          }

        if (end_ < buffer_.length())
          buffer_[end_] = c;
        else
          buffer_ += c;
        end_++;
      }

    stop_ = true;
    eot_  = (index_ == end_);
    if (eot_) --line_;
  }

}}   // namespace GNU_gama::local

#endif
