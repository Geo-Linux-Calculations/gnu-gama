# README upload gama

priklad:

```
./gnupload --to ftp.gnu.org:gama gama-2.14.tar.gz
```


Skript ```gnupload``` je z adresare build-aux, ```git clone git://git.sv.gnu.org/gnulib.git```
http://git.savannah.gnu.org/cgit/gnulib.git/tree/build-aux/gnupload

je nutne mit nainstalovan balik ```ncftp```


## Oprava

```
./gnupload --replace --to ftp.gnu.org:gama gama-2.05.tar.gz
```


## Subdirectory gama-qt

Archivni soubory projektu ```qgama``` ukládám do podadresáře gama/gama-qt 

```
./gnupload --to ftp.gnu.org:gama/gama-qt qgama-2.01.tar.gz
```

* Before version 2.00 the archive tarballs corresponded to lightweight tags
  from git server https://git.savannah.gnu.org/cgit/gama/qt.git
   with names like ```qt-gama-qt-1-minor.tar.gz```


### Oprava README_qgama.md  (--dry-run)

```
./gnupload --dry-run --replace --to ftp.gnu.org:gama/gama-qt README_qgama.md
```