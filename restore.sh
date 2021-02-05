#!/bin/bash
src=$1
dest=$2
k=$3
mkdir $dest/temp

if [[ $# -ne 3 ]]; then
	echo "Usage: ./restore.sh backup_directory restore_directory restoration_key"
else
	for f in $src/*.gpg; do
	[ -f "$f" ] || break
		echo $f
	name=`basename $f .gpg`
	echo $name
	gpg --pinentry-mode loopback --passphrase $k --output $dest/temp/"$name" --decrypt $f
	echo $dest/temp/"$name"
	tar -xf $dest/temp/"$name" -C $dest/temp/
	done

	for f in $dest/temp/*.gpg; do
	[ -f "$f" ] || break
		echo $f
	nom=$(basename $f .gpg)
	gpg --pinentry-mode loopback --passphrase $k --output $dest/temp/"$nom" --decrypt $f
	echo "[+] Decrypting $nom"
	echo -e "Will now delete $f \n"
	shred -u $f
	done

	cd $dest/temp/
	rm -rf *.gpg
	echo "currently at: "
	pwd
	for f in $dest/temp/*.gz; do
		tar -xf $f -C $dest/temp
		echo "[-] Deleting $f"
		rm -rf $f
		rm -rf $f.gpg
	done



fi
