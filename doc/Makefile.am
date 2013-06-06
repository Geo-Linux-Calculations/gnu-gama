info_TEXINFOS = gama.texi

gama_TEXINFOS =           \
   algorithms.texi        \
   class-g3-model.texi    \
   ellipsoids-1.texi      \
   ellipsoids.texi        \
   fdl.texi               \
   gama-download.texi     \
   gama-g3.texi           \
   gama-install.texi      \
   gama-introduction.texi \
   gama-local-adj.texi    \
   gama-local-macros.texi \
   gama-local.texi        \
   gama-local-input.texi  \
   obsdata.texi           \
   xyz2blh.texi           \
   gama-local-sql.texi

EXTRA_DIST =                         \
   fig/gama-local-adj-ellipse-g.eps  \
   fig/gama-local-adj-ellipse-g.pdf  \
   fig/gama-local-adj-ellipse-g.png  \
   fig/gama-local-adj-ellipse-g.txt  \
   fig/gama-local-input-example.eps  \
   fig/gama-local-input-example.fig  \
   fig/gama-local-input-example.pdf  \
   fig/gama-local-input-example.png  \
   fig/gama-local-input-example.txt  \
   fig/obsdata-fig.eps               \
   fig/obsdata-fig.fig               \
   fig/obsdata-fig.pdf               \
   fig/obsdata-fig.png               \
   fig/obsdata-fig.txt               \
   fig/xyz2blh-fig.eps               \
   fig/xyz2blh-fig.fig               \
   fig/xyz2blh-fig.pdf               \
   fig/xyz2blh-fig.png               \
   fig/xyz2blh-fig.txt

download-gendocs.sh:
	rm -f gendocs.sh* gendocs_template*
	wget http://savannah.gnu.org/cgi-bin/viewcvs/~checkout~/texinfo/texinfo/util/gendocs.sh
	wget http://savannah.gnu.org/cgi-bin/viewcvs/~checkout~/texinfo/texinfo/util/gendocs_template
	chmod +x gendocs.sh

run-gendocs.sh:
	make info
	rm -rf manual
	./gendocs.sh --texi2html --email cepek@gnu.org gama "GNU Gama manual"
	ls -l  manual/html_*/index.html 
	rm -fv manual/html_*/index.html 
