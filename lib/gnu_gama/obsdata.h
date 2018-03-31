/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003, 2012, 2014  Ales Cepek <cepek@gnu.org>

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

#include <list>
#include <cmath>

#ifndef GNU_gama_obsdata_h_gnugamaobsdata_observation_data_gnu_gama_obsdata
#define GNU_gama_obsdata_h_gnugamaobsdata_observation_data_gnu_gama_obsdata


namespace GNU_gama {


  template <typename Observation>
    class ObservationData;


  template <typename Observation>
    class Cluster
    {
    public:

      typedef Observation                     ObservationType;
      const ObservationData<Observation>*     observation_data;
      std::list<Observation*>                 observation_list;
      typename Observation::CovarianceMatrix  covariance_matrix;


      Cluster(const ObservationData<Observation>* od)
        : observation_data(od), act_obs(0), act_dim(0), act_nonz(0)
        {
        }
      virtual ~Cluster();

      virtual Cluster* clone(const ObservationData<Observation>*) const = 0;

      double stdDev(int i) const
        {
          i++; return std::sqrt(covariance_matrix(i,i));
        }
      int size() const
        {
          return observation_list.size();
        }

      void update();

      int  activeObs()  const { return act_obs;  }
      int  activeDim()  const { return act_dim;  }
      int  activeNonz() const { return act_nonz; }
      typename Observation::CovarianceMatrix
           activeCov() const;
      void scaleCov(int i, double sc);

    private:    // no copy ctor and no assignment

      Cluster(const Cluster&);
      void operator=(const Cluster&);

      int act_obs, act_dim, act_nonz;
    };



  template <typename Observation>
    class ObservationData
    {
    public:

      typedef Observation             ObservationType;
      typedef Cluster<Observation>    ClusterType;
      typedef std::list<ClusterType*> ClusterList;

      ClusterList                   clusters;

      ObservationData() {}
      ObservationData(const ObservationData& cod) { deepCopy(cod); }
      ~ObservationData();

      ObservationData& operator=(const ObservationData& cod);


      // ------------------------------------------------------------------

      class iterator;

      class const_iterator
        // : public std::iterator <std::forward_iterator_tag, Observation*>
        {
        public:

          bool operator==(const const_iterator& x) const
            {
              const bool t1 =  cluster   ==   OD->clusters.end();
              const bool t2 =  x.cluster == x.OD->clusters.end();
              if (t1 || t2) return t1 == t2;

              return cluster == x.cluster && obs == x.obs;
            }
          bool operator!=(const const_iterator& x) const
            {
              return !this->operator==(x);
            }
          const_iterator& operator++()
            {
                goto next_cycle;

                while (cluster != OD->clusters.end())
                  {
                    obs = (*cluster)->observation_list.begin();
                    while (obs != (*cluster)->observation_list.end())
                      {
                        return *this;

                      next_cycle:
                        ++obs;
                      }
                    ++cluster;
                  }

                return *this;
            }
          const_iterator operator++(int)
            {
              const_iterator tmp = *this;
              operator++();
              return tmp;
            }
          const Observation* operator*() const
            {
              return *obs;
            }

        private:
          friend class GNU_gama::ObservationData<Observation>;
          friend class iterator;

          const ObservationData*  OD;
          typename ObservationData::ClusterList::const_iterator  cluster;
          typename std::list<Observation*>::const_iterator       obs;
        };

      const_iterator  begin() const
        {
          const_iterator  iter;
          iter.OD       = this;
          iter.cluster  = iter.OD->clusters.begin();

          while (iter.cluster != iter.OD->clusters.end())
          {
            iter.obs     = (*iter.cluster)->observation_list.begin();

            if (iter.obs != (*iter.cluster)->observation_list.end())
              return iter;

            ++iter.cluster;
          }

          return iter;
        }

      const_iterator  end() const
        {
          const_iterator  iter;
          iter.OD       = this;
          iter.cluster  = iter.OD->clusters.end();

          return iter;
        }


      // ------------------------------------------------------------------

      class iterator
        // : public std::iterator <std::forward_iterator_tag, Observation*>
        {
        public:

          operator const_iterator() const
            {
              const_iterator ci;
              ci.OD      = OD;
              ci.cluster = cluster;
              ci.obs     = obs;
              return ci;
            }
          bool operator==(const iterator& x) const
            {
              const bool t1 =  cluster   ==   OD->clusters.end();
              const bool t2 =  x.cluster == x.OD->clusters.end();
              if (t1 || t2) return t1 == t2;

              return cluster == x.cluster && obs == x.obs;
            }
          bool operator!=(const iterator& x) const
            {
              return !this->operator==(x);
            }
          iterator& operator++()
            {
                goto next_cycle;

                while (cluster != OD->clusters.end())
                  {
                    obs = (*cluster)->observation_list.begin();
                    while (obs != (*cluster)->observation_list.end())
                      {
                        return *this;

                      next_cycle:
                        ++obs;
                      }
                    ++cluster;
                  }

                return *this;
            }
          iterator operator++(int)
            {
              iterator tmp = *this;
              operator++();
              return tmp;
            }
          Observation* operator*() const
            {
              return *obs;
            }

        private:
          friend class GNU_gama::ObservationData<Observation>;

          ObservationData*  OD;
          typename ObservationData::ClusterList::iterator  cluster;
          typename std::list<Observation*>::iterator       obs;
        };

      iterator  begin()
        {
          iterator  iter;
          iter.OD       = this;
          iter.cluster  = iter.OD->clusters.begin();

          while (iter.cluster != iter.OD->clusters.end())
          {
            iter.obs     = (*iter.cluster)->observation_list.begin();

            if (iter.obs != (*iter.cluster)->observation_list.end())
              return iter;

            ++iter.cluster;
          }

          return iter;
        }

      iterator  end()
        {
          iterator  iter;
          iter.OD       = this;
          iter.cluster  = iter.OD->clusters.end();

          return iter;
        }


    private:
      void deepCopy(const ObservationData& at);

    };


  // =====================================================================


  template <typename Observation>
    Cluster<Observation>::~Cluster()
    {
      for (typename std::list<Observation*>::iterator
             i=observation_list.begin(); i!=observation_list.end() ;  ++i)
        {
          delete *i;
        }
    }



  template <typename Observation>
    void Cluster<Observation>::update()
    {
      act_obs   = 0;
      act_dim   = 0;
      act_nonz  = 0;
      int index = 0;
      Observation* p;
      for (typename std::list<Observation*>::iterator
             i=observation_list.begin(); i!=observation_list.end(); ++i)
        {
          p = (*i);
          p->cluster = this;
          p->cluster_index = index++;
          if (p->active())
            {
              act_obs++;
              act_dim += p->dimension();
            }
        }

      if (act_dim)
        {
          int b = covariance_matrix.bandWidth();
          if (act_dim - 1 < b) b = act_dim - 1;
          act_nonz = act_dim*(b+1) - b*(b+1)/2;
        }
    }



  template <typename Observation>
    typename Observation::CovarianceMatrix
       Cluster<Observation>::activeCov() const
    {
      const int N     = activeDim();
      int active_band = covariance_matrix.bandWidth();

      if (N)
        {
          if (N-1 < active_band) active_band = N-1;
        }
      else
        active_band = 0;

      typename Observation::CovarianceMatrix C(N, active_band);
      int* ind = new int[act_dim + 1];
      int  k=1, n=1;

      for (typename std::list<Observation*>::const_iterator
             i=observation_list.begin(),
             e=observation_list.end(); i!=e; ++i)
        {
          const Observation* obs = *i;

          if (obs->active())
            for (int d=0; d < obs->dimension(); d++)
              {
                ind[k++] = n + d;
              }

          n += obs->dimension();
        }

      for (int i=1; i<=N; i++)
        for (int j=0; j<=active_band && i+j<=N; j++)
          C(i, i+j) = covariance_matrix(ind[i], ind[i+j]);

      delete[] ind;
      return C;
    }



  template <typename Observation>
    void Cluster<Observation>::scaleCov(int p, double sc)
    {
      const int N = covariance_matrix.dim();
      const int B = covariance_matrix.bandWidth();
      int k = p + B;
      if (k > N) k = N;
      // int  q = p - B;
      // if (q < 1) q = 1;
      /* unsigned safe version: */
      int q = (p < B+1) ? 1 : p-B;

      // scaling upper part of symmetric band matrix
      covariance_matrix(p, p) *= sc;
      for (int i=q; i<=k; i++)
        {
          covariance_matrix(p, i) *= sc;
        }
    }



  template <typename Observation>
    ObservationData<Observation>::~ObservationData()
    {
      for (typename std::list<Cluster<Observation>*>::iterator
             c=clusters.begin(); c!=clusters.end(); ++c)
        {
          delete *c;
        }
    }



  template <typename Observation>
    ObservationData<Observation>&
    ObservationData<Observation>::operator=(const ObservationData& cod)
    {
      if (this != &cod)
        {
          for (typename std::list<Cluster<Observation>*>::iterator
                 c=clusters.begin(); c!=clusters.end(); ++c) delete *c;
          {
            deepCopy(cod);
          }
        }

      return *this;
    }



  template <typename Observation>
    void ObservationData<Observation>::deepCopy(const ObservationData& cod)
    {
      for (typename std::list<Cluster<Observation>*>::const_iterator
             ci=cod.clusters.begin(); ci!=cod.clusters.end(); ++ci)
        {
          Cluster<Observation>* current = (*ci)->clone(this);

          typename std::list<Observation*>::const_iterator
            begin = (*ci)->observation_list.begin();
          typename std::list<Observation*>::const_iterator
            end   = (*ci)->observation_list.end();
          for (typename std::list<Observation*>::const_iterator
                 m=begin; m!=end; ++m)
            {
              current->observation_list.push_back( (*m)->clone() );
            }

          current->covariance_matrix = (*ci)->covariance_matrix;
          current->update();
          clusters.push_back( current );
        }
    }


}

#endif
