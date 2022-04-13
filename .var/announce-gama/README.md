# gnulib

* cd gama.work.ac
* git clone git://git.sv.gnu.org/gnulib.git
* ln -s gnulib/build-aux/announce-gen        # create a symlink or copy ...

# new release

* copy or update file NEWS from the new release
* copy new release tar ball
* update gama-announce.sh (ie. update current and previous releases versions)
* run ./gama-announce.sh > announce-version.txt
* edit announce-version.txt

# when done

* git add announce-version.txt 
* git commit

