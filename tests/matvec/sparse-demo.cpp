#include <gnu_gama/sparse/intlist.h>
#include <gnu_gama/sparse/sbdiagonal.h>
#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/smatrix_graph.h>
//       <gnu_gama/sparse/smatrix_graph_connected.h>
#include <gnu_gama/sparse/smatrix_ordering.h>
#include <gnu_gama/sparse/svector.h>

#include <matvec/matvec.h>
#include <iostream>

using namespace std;
using namespace GNU_gama;


void write(ostream& cout, BlockDiagonal<>* bd)
{
  // cout.precision(7);
  cout << endl;
  for (unsigned long i=1; i<=bd->blocks(); i++)
    {
      cout << i << " : [" << bd->dim(i) << " | " << bd->width(i) << "]\n";
      double* b = bd->begin(i);
      double* e = bd->end(i);
      cout << "    ";
      while (b != e) cout << *b++ << ' ';
      cout << endl;
    }
}

void write(ostream& cout, SparseMatrix<>* sgm)
{
  cout << endl;
  for (auto k=1; k<=sgm->rows(); k++)
    {
      cout << k << " : ";
      double* n = sgm->begin(k);
      double* e = sgm->end  (k);
      for (auto i=sgm->ibegin(k) ; n!=e; n++, i++)
        {
          cout << *n << " [" << *i << "]  ";
        }
      cout << endl;
    }
}



int main()
{
  {
    IntegerList<int> ilist(1000000);

    Vec<> v {1,2,3,4,5,6,7,8,9,10};
    ilist.reset(v.dim());
    int n=0;
    for (auto i : v) {
      ilist(n++) = i;
    }

    IntegerList<int>::const_iterator vit = ilist.begin();
    IntegerList<int>::const_iterator eit = ilist.end();

    cout << "\n---  IntegerList<int>    // zero based indices  \n\n";
    for (int i=1; vit != eit; vit++, i++) {
      if (*vit != v(i)) {
        cout << *vit << " != " << v(i) << endl;
        return 1;
      }
      cout << *vit << " ";
    }
    cout << endl;
  }

  {
    cout << "\n---  Symmetric Block Diagonal Matrix demo  --------------\n";

    double b1[] = {1.1, 1.2, 1.3};
    double b2[] = {44.2, 5.2, 66.2, 7.2, 88.2};
    double b3[] = {19, 1, 2, 3, 18, 1, 2, 17, 1, 16};
    double b4[] = {81, 81, 81, 81, 145, 145, 145, 64, 194, 194, 113, 49,
                   230, 149, 85, 36, 174, 110, 61, 25, 126, 77, 41, 16, 86,
                   50, 25, 54, 29, 30};

    BlockDiagonal<>* m1 = new BlockDiagonal<>(20, 5000);
    m1->add_block(3, 0, b1);
    m1->add_block(3, 1, b2);
    m1->add_block(4, 3, b3);
    m1->add_block(9, 3, b4);
    write(cout, m1);

    BlockDiagonal<>* m2 = m1->replicate();
    m2->cholDec();
    write(cout, m2);

    cout << "\n---  Upper Triangular Block Diagonal Matrix  ------------\n\n";

    UpperBlockDiagonal<> upper(m1);

    cout << "dimension = " << upper.dim() << endl
         << "nonzeroes = " << upper.nonzeroes() << "\n\n";

    for (unsigned i=1; i<=upper.dim(); i++)
      {
        cout << i << " : ";
        const double* b = upper.begin(i);
        const double* e = upper.end(i);
        while (b!=e)
          {
            cout << *b++ << " ";
          }
        cout << endl;
      }

    delete m1;
    delete m2;
  }

  {
    cout << "\n---  Sparse General Matrix demo  ------------------------\n";

    SparseMatrix<>* inp = new SparseMatrix<>(500, 6, 5);

    for (int n=1; n<=6; n++)
      {
        inp->new_row();
        switch (n)
          {
          case 1:
            inp->add_element(12.3, 3);
            break;
          case 2:
            break;
          case 3:
            inp->add_element(4.2, 1);
            inp->add_element(9.4, 5);
            inp->add_element(0.2, 2);
            break;
          case 4:
            inp->add_element(7.3, 4);
            break;
          case 5:
            inp->add_element(2.3, 5);
            inp->add_element(3.7, 3);
            break;
          case 6:
            inp->add_element(2.4, 3);
            inp->add_element(6.7, 2);
            inp->add_element(2.9, 1);
            break;
          };
      }

    SparseMatrix<>* sgm = inp->replicate();
    delete inp;

    write(cout, sgm);

    inp  = sgm->transpose();
    delete sgm;
    sgm  = inp->transpose();
    delete inp;

    write(cout, sgm);

    delete sgm;
  }

  {
    cout << "\n---  Sparse Vector demo  ----------------\n\n";
    SparseVector<> svec;

    for (int i=1; i<=9; i++)
      {
        svec.reset();

        for (int j=1; j<=3+i/3; j++) svec.add(j, 10.0*i + 0.01*j);

        cout << i << " (nonz " << svec.nonzeroes() << ") : ";
        double* mm = svec.begin ();
        double* me = svec.end   ();
        std::size_t* ii = svec.ibegin();
        std::size_t* ie = svec.iend  ();
        while (mm != me && ii != ie)
          {
            cout << *ii++ << " " << *mm++ << "   ";
          }
        cout << endl;
      }
  }
}
