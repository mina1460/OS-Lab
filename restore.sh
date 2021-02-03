#!/bin/bash
src=$1
dest=$2
k=$3
mkdir $dest/temp

if [[ $# -ne 3 ]]; then
	echo "Usage: ./backup.sh source_directory destination_directory backup_key"
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
	done
fi
