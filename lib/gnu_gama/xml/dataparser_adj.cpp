/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/xml/dataparser.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/radian.h>
#include <cstring>

using namespace std;
using namespace GNU_gama;


namespace GNU_gama {

  struct DataParser_adj {
  };

}



void DataParser::close_adj()
{
  delete adj;
}

void DataParser::init_adj()
{
  adj = 0;

  adj_sparse_mat = 0;
  adj_block_diagonal = 0;
  adj_array = 0;

  point = 0;

  // .....  <adj-input-data>  ........................................

  init(s_gama_data, t_adj_input_data,
       s_adj_input_data_1, s_adj_input_data_5, 0,
       &DataParser::adj_input_data, 0, &DataParser::adj_input_data,
       s_adj_input_data_4);

  // .....  <sparse-mat>  ............................................

  init(s_adj_input_data_1, t_sparse_mat,
       s_sparse_mat_1, s_sparse_mat_4, s_adj_input_data_2,
       0, 0, &DataParser::sparse_mat);

  init(s_sparse_mat_1, t_rows,
       s_sparse_mat_rows, 0, s_sparse_mat_2,
       0, &DataParser::add_text, 0);

  init(s_sparse_mat_2, t_cols,
       s_sparse_mat_cols, 0, s_sparse_mat_3,
       0, &DataParser::add_text, 0);

  init(s_sparse_mat_3, t_nonz,
       s_sparse_mat_nonz, 0, s_sparse_mat_4,
       0, &DataParser::add_text, &DataParser::sparse_mat_nonz);

  init(s_sparse_mat_4, t_row,
       s_sparse_mat_row_1, s_sparse_mat_row_2, 0,
       &DataParser::sparse_mat_row, 0, &DataParser::sparse_mat_row);

  init(s_sparse_mat_row_1, t_nonz,
       s_sparse_mat_row_nonz, 0, s_sparse_mat_row_2,
       0, &DataParser::add_text, &DataParser::sparse_mat_row_n);

  init(s_sparse_mat_row_2, t_int,
       s_sparse_mat_row_int, 0, s_sparse_mat_row_3,
       0, &DataParser::add_text, 0);

  init(s_sparse_mat_row_3, t_flt,
       s_sparse_mat_row_flt, 0, s_sparse_mat_row_2,
       0, &DataParser::add_text, &DataParser::sparse_mat_row_f);

  // ......  <block-diagonal>  .......................................

  init(s_adj_input_data_2, t_block_diagonal,
       s_block_diagonal_1, s_block_diagonal_3, s_adj_input_data_3,
       0, 0, &DataParser::block_diagonal);

  init(s_block_diagonal_1, t_blocks,
       s_block_diagonal_blocks, 0, s_block_diagonal_2,
       0, &DataParser::add_text, 0);

  init(s_block_diagonal_2, t_nonz,
       s_block_diagonal_nonz, 0, s_block_diagonal_3,
       0, &DataParser::add_text, &DataParser::block_diagonal_nonz);

  init(s_block_diagonal_3, t_block,
       s_block_diagonal_block_1, s_block_diagonal_block_3, 0,
       0, 0, &DataParser::block_diagonal_block);

  init(s_block_diagonal_block_1, t_dim,
       s_block_diagonal_block_d, 0, s_block_diagonal_block_2,
       0, &DataParser::add_text, 0);

  init(s_block_diagonal_block_2, t_width,
       s_block_diagonal_block_w, 0, s_block_diagonal_block_3,
       0, &DataParser::add_text, &DataParser::block_diagonal_block_w);

  init(s_block_diagonal_block_3, t_flt,
       s_block_diagonal_block_f, 0, 0,
       0, &DataParser::add_text, &DataParser::block_diagonal_vec_flt);

  // ......  <vector>  ...............................................

  init(s_adj_input_data_3, t_vector,
       s_vector_1, s_vector_2, s_adj_input_data_4,
       0, 0, &DataParser::vector);

  init(s_vector_1, t_dim,
       s_vector_dim, 0, s_vector_2,
       0, &DataParser::add_text, &DataParser::vector_dim);

  init(s_vector_2, t_flt,
       s_vector_flt, 0, 0,
       0, &DataParser::add_text, &DataParser::vector_flt);

  // ......  <array>  ................................................

  init(s_adj_input_data_4, t_array,
       s_array_1, s_array_2, s_adj_input_data_5,
       0, 0, &DataParser::array);

  init(s_array_1, t_dim,
       s_array_dim, 0, s_array_2,
       0, &DataParser::add_text, &DataParser::array_dim);

  init(s_array_2, t_int,
       s_array_int, 0, 0,
       0, &DataParser::add_text, &DataParser::array_int);

  // .................................................................
}

// ......  <adj-input-data>  ...............................................

int DataParser::adj_input_data(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  adj_sparse_mat = 0;
  adj_block_diagonal = 0;
  adj_vector.reset();
  adj_array = 0;

  return 0;
}

int DataParser::adj_input_data(const char *name)
{
  AdjInputData *data = new AdjInputData;

  if (adj_sparse_mat    ) data->set_mat(adj_sparse_mat);
  if (adj_block_diagonal) data->set_cov(adj_block_diagonal);
  if (adj_vector.dim()  ) data->set_rhs(adj_vector);
  if (adj_array         ) data->set_minx(adj_array);
  objects.push_back( new DataObject::AdjInput(data) );

  adj_sparse_mat = 0;
  adj_block_diagonal = 0;
  adj_array = 0;

  return end_tag(name);
}

// ......  <sparse-mat>  ...................................................

int DataParser::sparse_mat(const char *name)
{
  if (adj_sparse_mat && !adj_sparse_mat->check())
    error("### bad data in <sparse-mat>>");

  return end_tag(name);
}

int DataParser::sparse_mat_nonz(const char *name)
{
  std::size_t  rows, cols;
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> rows >> cols >> adj_sparse_mat_nonz))
    {
      text_buffer.erase();
      adj_sparse_mat =
        new SparseMatrix<>(adj_sparse_mat_nonz, rows, cols);
      return end_tag(name);
    }
  return error("### bad data in tags <rows> / <cols> / <nonz>");
}

int DataParser::sparse_mat_row(const char *name, const char **atts)
{
  no_attributes(name, atts);
  state = next[state][tag(name)];

  adj_sparse_mat->new_row();
  adj_sparse_mat_row_nonz = 0;
  return 0;
}

int DataParser::sparse_mat_row(const char *name)
{
  return end_tag(name);
}

int DataParser::sparse_mat_row_n(const char *name)
{
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> adj_sparse_mat_row_nonz))
    {
      text_buffer.erase();
      return end_tag(name);
    }
  return error("### bad data in tag <nonz>");
}

int DataParser::sparse_mat_row_f(const char *name)
{
  istringstream inp(text_buffer.c_str());
  std::size_t  indx;
  double       flt;
  if (adj_sparse_mat_nonz-- && adj_sparse_mat_row_nonz--)
    if (pure_data(inp >> indx >> flt))
      {
        adj_sparse_mat->add_element(flt, indx);
        text_buffer.erase();
        return end_tag(name);
      }
  return error("### bad data in tags <nonz> / <int> / <flt>");
}

// ......  <block-diagonal>  ...............................................

int DataParser::block_diagonal(const char *name)
{
  if (block_diagonal_blocks_)
    return error("### not enough <block> elements in <block-diagonal>");

  return end_tag(name);
}

int DataParser::block_diagonal_nonz(const char *name)
{
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> block_diagonal_blocks_ >> block_diagonal_nonz_))
    {
      text_buffer.erase();
      adj_block_diagonal = new BlockDiagonal<>
        (block_diagonal_blocks_, block_diagonal_nonz_);
      return end_tag(name);
    }
  return error("### bad data in tags <blocks> / <nonz>");
}

int DataParser::block_diagonal_block_w(const char *name)
{
  istringstream inp(text_buffer.c_str());
  std::size_t dim, width;
  if (pure_data(inp >> dim >> width) && dim>0 && width>=0 && width<dim)
    {
      block_diagonal_dim   = dim;
      block_diagonal_width = width;

      text_buffer.erase();
      bd_vector_dim = dim*(width+1) - width*(width+1)/2;
      bd_vector.reset(bd_vector_dim);
      bd_vector_iterator = bd_vector.begin();
      return end_tag(name);
    }
  return error("### bad data in tags <dim> / <width>");
}

int DataParser::block_diagonal_vec_flt(const char *name)
{
  if (bd_vector_dim == 0 || block_diagonal_nonz_ == 0)
    return error("### too many <flt> elements in <block-diagonal>");

  double flt;
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> flt))
    {
      bd_vector_dim--;
      block_diagonal_nonz_--;
      text_buffer.erase();
      *bd_vector_iterator++ = flt;

      return end_tag(name);
    }

  return error("### bad data format in a <flt> element in <block-diagonal>");
}

int DataParser::block_diagonal_block(const char *name)
{
  if (bd_vector_dim)
    return error("### not enough <flt> elements in <block-diagonal>");

  if (block_diagonal_blocks_ == 0)
    return error("### too many <block> elements in <block-diagonal>");

  block_diagonal_blocks_--;
  adj_block_diagonal->add_block(block_diagonal_dim,
                                block_diagonal_width, bd_vector.begin());

  return end_tag(name);
}

// ......  <vector>  .......................................................

int DataParser::vector(const char *name)
{
  if (adj_vector_dim)
    return error("### not enough <flt> elements in <vector>");

  return end_tag(name);
}

int DataParser::vector_dim(const char *name)
{
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> adj_vector_dim))
    {
      text_buffer.erase();
      adj_vector.reset(adj_vector_dim);
      adj_vector_iterator = adj_vector.begin();

      return end_tag(name);
    }

  return error("### bad vector dimension in tag <dim>");
}

int DataParser::vector_flt(const char *name)
{
  if (adj_vector_dim == 0)
    return error("### too many <flt> elements in <vector>");

  double flt;
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> flt))
    {
      adj_vector_dim--;
      text_buffer.erase();
      *adj_vector_iterator++ = flt;

      return end_tag(name);
    }

  return error("### bad vector data in tag <flt>");
}

int DataParser::array(const char *name)
{
  if (adj_array_dim)
    return error("### not enough <int> elements in <array>");

  return end_tag(name);
}

int DataParser::array_dim(const char *name)
{
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> adj_array_dim))
    {
      text_buffer.erase();
      adj_array = new IntegerList<>(adj_array_dim);
      adj_array_iterator = adj_array->begin();
      return end_tag(name);
    }

  return error("### bad array dimension in tag <dim>");
}

int DataParser::array_int(const char *name)
{
  if (adj_array_dim == 0)
    return error("### too many <int> elements in <array>");

  int index;
  istringstream inp(text_buffer.c_str());
  if (pure_data(inp >> index))
    {
      adj_array_dim--;
      text_buffer.erase();
      *adj_array_iterator++ = index;

      return end_tag(name);
    }

  return error("### bad array data in tag <int>");
}
