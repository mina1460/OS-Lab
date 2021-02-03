#!/bin/bash
src=$1
trgt=$2
k=$3
echo $src $trgt $k
d=`date`
date_no_space=`echo -e $d | sed -e 's/ /_/g'`
date_no_colons=`echo -e $date_no_space | sed -e 's/:/_/g'`
echo "The Date of the backup is: [ $d ]"
echo -e "The modifed date is: [ $date_no_colons ]\n"
if [[ $# -ne 3 ]]; then
	echo "Usage: ./backup.sh source_directory destination_directory backup_key"
else
	mkdir "$trgt/$date_no_colons"
	ntrgt="$trgt""/""$date_no_colons"
	echo $ntrgt
	echo "[+] Working on it Now"
	for d in $src/*/; do
		if [ -d $d ]; then
			dirname=`basename "${d}"`
			new_f="$dirname""_""$date_no_colons"
			tar  -cvzf "$ntrgt""/""$new_f"".tar.gz" -P $d
			tar_name="$ntrgt""/""$new_f"".tar.gz"
			#gpg -e -r mina146@aucegypt.edu $tar_name
			gpg --pinentry-mode loopback --passphrase $k --symmetric $tar_name
			shred -u $tar_name
		fi
	done
	#now I need to iterate over the newly created files and tar them 
	echo -e "[+] Done encrypting and compressing!\n"
	tar -cf "$ntrgt"_Full.tar -T /dev/null
	for f in $ntrgt/*.gpg; do
		[ -f "$f" ] || break
		echo $f
		tar -uvf "$ntrgt"_Full.tar -P $f
	done
	gzip "$ntrgt"_Full.tar
	gpg --pinentry-mode loopback --passphrase $k --symmetric "$ntrgt"_Full.tar.gz
	shred -u "$ntrgt"_Full.tar.gz
fi
